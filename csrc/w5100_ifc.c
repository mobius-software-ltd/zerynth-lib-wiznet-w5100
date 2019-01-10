#include "zerynth.h"

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/zerynth_hwcrypto.h"

#include "HAL_Config.h"
#include "HALInit.h"
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#include "inttypes.h"
#include "stm_lib/inc/stm32f10x_gpio.h"
#include "stm_lib/inc/stm32f10x_exti.h"
#include "W5100RelFunctions.h"

#include "ioLibrary_Driver/Internet/DHCP/dhcp.h"
#include "ioLibrary_Driver/Internet/DNS/dns.h"
#include "ioLibrary_Driver/Ethernet/socket.h"

#undef printf
#define printf(...)

#define STATUS_IDLE 0
#define STATUS_LINKING 1
#define STATUS_UNLINKING 2
#define STATUS_STOPPING 3
#define ERROR_CANT_CONNECT 1

typedef struct _ethdrv {
    VSemaphore link_lock;
    VSemaphore ssl_lock;
    uint8_t* ip;
    uint8_t* mask;
    uint8_t* gw;
    uint8_t* dns;
    uint8_t status;
    uint8_t error;
    uint8_t connected;
    uint8_t has_link_info;
} EthDrv;
EthDrv drv;

#define SOCK_DHCP   6
#define SOCK_DNS    7
#define SOCK_NUM    0

#define ETH_MAX_BUF_SIZE   2048
uint8_t DHCP_BUF[ETH_MAX_BUF_SIZE];
uint8_t DNS_BUF[ETH_MAX_BUF_SIZE];

wiz_NetInfo gWIZNETINFO = { .mac = MAC,
#ifndef DHCP
							.ip = IP,
							.sn = MASK,
							.gw = GW,
							.dns = DNS,
              .dhcp = NETINFO_STATIC};
drv.ip = IP;
drv.gw = GW;
drv.mask = MASK;
drv.dns = DNS;
#else
    .dhcp = NETINFO_DHCP};
    // set MAC address before using DHCP
    setSHAR(MAC);
    /* DHCP client Initialization */
    DHCP_init(SOCK_DHCP,DHCP_BUF);
#endif

void delay(unsigned int count)
{
	int temp;
	temp = count + TIM2_gettimer();
	while(temp > TIM2_gettimer()){}
}

C_NATIVE(_w5100_init)
{
    NATIVE_UNWARN();

    memset(&drv, 0, sizeof(EthDrv));
    drv.ssl_lock = vosSemCreate(1);
    drv.link_lock = vosSemCreate(0);
    gpioInitialize();
    timerInitialize();
    /* SPI method callback registration */
	reg_wizchip_spi_cbfunc(spiReadByte, spiWriteByte);
	/* CS function register */
    reg_wizchip_cs_cbfunc(csEnable,csDisable);
    spiInitailize();
    resetAssert();
	  delay(10);
	  resetDeassert();
	  delay(10);
    W5100Initialze();
    return SOCK_OK;
}

C_NATIVE(w5100_eth_link)
{
    NATIVE_UNWARN();
    *res = MAKE_NONE();
    RELEASE_GIL();
    drv.status = STATUS_LINKING;
    printf("HAS LINK INFO %i\n",drv.has_link_info);

    if (!drv.has_link_info)
    {
       DHCP_run();
       getIPfromDHCP(gWIZNETINFO.ip);
       getGWfromDHCP(gWIZNETINFO.gw);
       getSNfromDHCP(gWIZNETINFO.sn);
       getDNSfromDHCP(gWIZNETINFO.dns);
       drv.ip = gWIZNETINFO.ip;
       drv.gw = gWIZNETINFO.gw;
       drv.mask = gWIZNETINFO.sn;
       drv.dns = gWIZNETINFO.dns;
    }
    ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
    vosSemWait(drv.link_lock);
    drv.status = STATUS_IDLE;
    ACQUIRE_GIL();

    if (drv.error) {
        drv.error = 0;
        printf("**** %x\n", esp_err);
        return ERR_IOERROR_EXC;
    }
    return SOCK_OK;
}

C_NATIVE(w5100_eth_unlink)
{
    NATIVE_UNWARN();
    *res = MAKE_NONE();
    RELEASE_GIL();
    drv.status = STATUS_UNLINKING;
    disconnect(SOCK_NUM);
    vosSemWait(drv.link_lock);
    ACQUIRE_GIL();
    return SOCK_OK;
}

C_NATIVE(w5100_eth_is_linked)
{
    NATIVE_UNWARN();
    if (!drv.connected) {
        *res = PBOOL_FALSE();
    }
    else {
        *res = PBOOL_TRUE();
    }
    return SOCK_OK;
}

C_NATIVE(w5100_net_link_info)
{
    NATIVE_UNWARN();
    wiz_NetInfo* getWIZNETINFO;
    wizchip_getnetinfo(getWIZNETINFO);
    *res = getWIZNETINFO;
    return SOCK_OK;
}

C_NATIVE(w5100_net_set_link_info)
{
    C_NATIVE_UNWARN();
    
    uint8_t* ip;
    uint32_t iplen;
    uint8_t* mask;
    uint32_t masklen;
    uint8_t* gw;
    uint32_t gwlen;
    uint8_t* dns;
    uint32_t dnslen;
    
    if (parse_py_args("ssss", nargs, args, 
            &ip, &iplen,
            &mask, &masklen,
            &gw, &gwlen,
            &dns, &dnslen) 
            != 4)
        return ERR_TYPE_EXC;
        
    drv.ip = ip;
    drv.gw = gw;
    drv.dns = dns;
    drv.mask = mask;
    if (ip != 0)
        drv.has_link_info = 1;
    else
        drv.has_link_info = 0;
    *res = MAKE_NONE();
    return SOCK_OK;
}

C_NATIVE(w5100_net_resolve)
{
    C_NATIVE_UNWARN();
    uint8_t* url;
    uint32_t len;
    if (parse_py_args("s", nargs, args, &url, &len) != 1)
        return ERR_TYPE_EXC;
    uint8_t * ip_from_dns;
    /* DNS client Initialization */
    DNS_init(SOCK_DNS, DNS_BUF);
    /* DNS client  Send DNS query and receive DNS response
      @param dns_ip        : DNS server ip
      @param name          : Domain name to be queryed
      @param ip_from_dns   : IP address from DNS server
    */
    DNS_run(drv.dns, url, ip_from_dns);
    *res = ip_from_dns;
    return SOCK_OK;
}

C_NATIVE(w5100_net_socket)
{
    C_NATIVE_UNWARN();
    uint8_t protocol;
    uint16_t port;
    uint8_t flag;
    int8_t sock;

    if (parse_py_args("BHB", nargs, args, &protocol, &port, &flag) != 3)
      return ERR_TYPE_EXC;
    RELEASE_GIL();
    sock = socket(SOCK_NUM, protocol, port, flag);
    ACQUIRE_GIL();
    if (sock < 0)
        return ERR_IOERROR_EXC;
    *res = PSMALLINT_NEW(sock);
    return SOCK_OK;
}

C_NATIVE(w5100_net_connect)
{
    C_NATIVE_UNWARN();
    int8_t sock;
    uint8_t* addr;
    uint32_t addrlen;
    uint16_t port;
    if (parse_py_args("BsH", nargs, args, &sock, &addr, &addrlen, &port) != 3)
      return ERR_TYPE_EXC;
    RELEASE_GIL();
    connect(sock, addr, port);
    ACQUIRE_GIL();
    *res = PSMALLINT_NEW(sock);
    return SOCK_OK;
}

C_NATIVE(w5100_net_close)
{
    C_NATIVE_UNWARN();
    int8_t sock;
    if (parse_py_args("B", nargs, args, &sock) != 1)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    close(sock);
    ACQUIRE_GIL();
    *res = PSMALLINT_NEW(sock);
    return SOCK_OK;
}

C_NATIVE(w5100_net_send)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    uint16_t len;
    uint8_t sock;
    if (parse_py_args("Bs", nargs, args, &sock, &buf, &len) != 2)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    send(sock, buf, len);
    ACQUIRE_GIL();
    *res = PSMALLINT_NEW(sock);
    return SOCK_OK;
}

C_NATIVE(w5100_net_send_all)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    uint16_t len;
    uint8_t sock;
    int32_t wrt;
    int32_t w;
    if (parse_py_args("Bs", nargs, args, &sock, &buf, &len) != 2)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    wrt = 0;
    while (wrt < len){
      w = send(sock, buf + wrt, len - wrt);
      if (w < 0)
        break;
      wrt += w;
    }
    ACQUIRE_GIL();
    *res = MAKE_NONE();
    return SOCK_OK;
}

C_NATIVE(w5100_net_sendto)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    uint16_t len;
    uint8_t sock;
    uint8_t* addr;
    uint8_t* addrlen;
    uint16_t port;
    if (parse_py_args("BssH", nargs, args, &sock, &buf, &len, &addr, &addrlen, &port) != 4)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    sendto(sock, buf, len, addr, port);
    ACQUIRE_GIL();
    *res = PSMALLINT_NEW(sock);
    return SOCK_OK;
}

C_NATIVE(w5100_net_recv_into)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    uint16_t len;
    uint8_t sock;
    int32_t ofs;

    if (parse_py_args("Bsi", nargs, args, &sock, &buf, &len, &ofs)!= 3)
      return ERR_TYPE_EXC;

      buf += ofs;
      len -= ofs;

      RELEASE_GIL();
      int rb = 0;
      int r;
      while (rb < len) {
          r = recv(sock, buf + rb, len - rb);
          if (r <= 0)
              break;
          rb += r;
      }
      ACQUIRE_GIL();
      if (r <= 0) {
        return SOCK_ERROR;
      }
    *res = PSMALLINT_NEW(rb);

    return SOCK_OK;
}

C_NATIVE(w5100_net_recvfrom_into)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    uint16_t len;
    uint8_t sock;
    int32_t ofs;
    uint8_t* addr;
    uint16_t addrlen;
    uint16_t port;

    if (parse_py_args("BssHi", nargs, args, &sock, &buf, &len, &addr, &addrlen, &port, &ofs)!= 5)
      return ERR_TYPE_EXC;

      buf += ofs;
      len -= ofs;

      RELEASE_GIL();
      int r;
      r = recvfrom(sock, buf, len, addr, port);
      ACQUIRE_GIL();

      if (r <= 0) {
        return SOCK_ERROR;
      }

      PTuple* tpl = (PTuple*)psequence_new(PTUPLE, 2);
      PTUPLE_SET_ITEM(tpl, 0, PSMALLINT_NEW(r));
      PTUPLE_SET_ITEM(tpl, 1, addr);
      *res = tpl;
      return SOCK_OK;
}

C_NATIVE(w5100_net_setsockopt)
{
    C_NATIVE_UNWARN();
    uint8_t sock;
    sockopt_type sotype;
    int32_t optname;
    int32_t optvalue;

    if (parse_py_args("BIii", nargs, args, &sock, &sotype, &optname, &optvalue) != 4)
      return ERR_TYPE_EXC;

    RELEASE_GIL();
    sock = setsockopt(sock, sotype, optvalue);
    ACQUIRE_GIL();

    if (sock < 0)
        return ERR_IOERROR_EXC;

     *res = MAKE_NONE();
     return SOCK_OK;
}

C_NATIVE(w5100_net_listen)
{
    C_NATIVE_UNWARN();
    uint8_t sock;
    if (parse_py_args("B", nargs, args, &sock) != 1)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    listen(sock);
    ACQUIRE_GIL();
    *res = MAKE_NONE();
    return SOCK_OK;
}

C_NATIVE(w5100_net_bind)
{
  C_NATIVE_UNWARN();
  *res = MAKE_NONE();
  return SOCK_OK;
}

C_NATIVE(w5100_net_accept)
{
  C_NATIVE_UNWARN();
  *res = MAKE_NONE();
  return SOCK_OK;
}
