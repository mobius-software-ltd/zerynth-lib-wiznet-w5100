
#include "zerynth.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "inttypes.h"

#include "lib/Ethernet/Ethernet.h"
//#include "lib/Ethernet/utility/w5100.h"
//#include "lib/Ethernet/Dhcp.h"

SPISettings * spiSettings;
W5100Class * w5100Class;
SPIClass * spiClass;
spiClassConstructor(spiClass, SPI_INTERFACE, SPI_INTERFACE_ID);
DhcpClass * dhcpClass;

#define SPI_ETHERNET_SETTINGS spiSettings

struct EthernetClass *ethernetClass;
uint8_t *mac;
unsigned long timeout;
unsigned long responseTimeout;
struct IPAddress * dnsIP;

#define SOCK_NUM 0
#define SOCK_OK  1

typedef struct wiz_NetInfo_t
{
   uint8_t mac[6];  ///< Source Mac Address
   uint8_t ip[4];   ///< Source IP Address
   uint8_t sn[4];   ///< Subnet Mask
   uint8_t gw[4];   ///< Gateway IP Address
   uint8_t dns[4];  ///< DNS server IP Address
} wiz_NetInfo;
struct wiz_NetInfo* getWIZNETINFO;

C_NATIVE(_w5100_init)
{
    NATIVE_UNWARN();
    spiSettingsConstructor(spiSettings);
    spiClassConstructor(spiClass, SPI_INTERFACE, SPI_INTERFACE_ID);
    int ret = begin(ethernetClass, mac, timeout, responseTimeout);
    return ret;
}


C_NATIVE(w5100_eth_link)
{
    NATIVE_UNWARN();
    beginIP(ethernetClass, mac, dnsIP);
    return SOCK_OK;
}

C_NATIVE(w5100_eth_unlink)
{
    NATIVE_UNWARN();
    RELEASE_GIL();
    socketDisconnect(ethernetClass, SOCK_NUM);
    ACQUIRE_GIL();
    return SOCK_OK;
}

C_NATIVE(w5100_eth_is_linked)
{
    NATIVE_UNWARN();
    EthernetLinkStatus status = linkStatus(ethernetClass);
    if (status == LinkON){
      *res = PBOOL_TRUE();
    }
    *res = PBOOL_FALSE();
    return SOCK_OK;
}

C_NATIVE(w5100_net_link_info)
{
    NATIVE_UNWARN();
    w5100ClassGetMACAddress(w5100Class, getWIZNETINFO->mac);
    w5100ClassGetIPAddress(w5100Class, getWIZNETINFO->ip);
    w5100ClassGetSubnetMask(w5100Class, getWIZNETINFO->sn);
    w5100ClassGetGatewayIp(w5100Class, getWIZNETINFO->gw);

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

    w5100ClassSetMACAddress(w5100Class, getWIZNETINFO->mac);
    w5100ClassSetIPAddress(w5100Class, getWIZNETINFO->ip);
    w5100ClassSetSubnetMask(w5100Class, getWIZNETINFO->sn);
    w5100ClassSetGatewayIp(w5100Class, getWIZNETINFO->gw);

    *res = MAKE_NONE();
    return SOCK_OK;
}
/*
C_NATIVE(w5100_net_resolve)
{
    C_NATIVE_UNWARN();
    uint8_t* url;
    uint32_t len;
    if (parse_py_args("s", nargs, args, &url, &len) != 1)
        return ERR_TYPE_EXC;
    uint8_t * ip_from_dns;
    DNS_init(SOCK_DNS, DNS_BUF);
    DNS_run(gWIZNETINFO.dns, url, ip_from_dns);
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
*/
