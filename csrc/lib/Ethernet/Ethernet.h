#ifndef ethernet_h_
#define ethernet_h_

#include "../cores/utils.h"
#include "utility/w5100.h"
#include "../cores/Udp.h"
#include "../cores/Client.h"

#if defined(RAMEND) /*&& defined(RAMSTART) && ((RAMEND - RAMSTART) <= 2048)*/
#define MAX_SOCK_NUM 4
#else
#define MAX_SOCK_NUM 8
#endif

typedef int EthernetLinkStatus;
enum {
	Unknown,
	LinkON,
	LinkOFF
};

typedef int EthernetHardwareStatus;
enum {
	EthernetNoHardware,
	EthernetW5100
};

typedef struct EthernetUDP EthernetUDP;
struct EthernetUDP {
  uint16_t _port; 
  struct IPAddress _remoteIP; 
  uint16_t _remotePort; 
  uint16_t _offset;
  uint8_t sockindex;
  uint16_t _remaining; 
  uint8_t (*udpClientBegin)             (EthernetUDP *, uint16_t); 
  uint8_t (*udpClientBeginMulticast)    (EthernetUDP *, struct IPAddress *, uint16_t);  
  void    (*udpClientStop)              (EthernetUDP *);  
  int    (*udpClientBeginPacketIP)      (EthernetUDP *, struct IPAddress *, uint16_t port);
  int    (*udpClientBeginPacketHost)    (EthernetUDP *, const char *host, uint16_t port);
  int    (*udpClientEndPacket)          (EthernetUDP *);
  size_t (*udpClientWriteByte)          (EthernetUDP *, uint8_t);
  size_t (*udpClientWriteBuffer)        (EthernetUDP *, const uint8_t *buffer, size_t size);
  int (*udpClientParsePacket)           (EthernetUDP *);
  int (*udpClientAvailable)             (EthernetUDP *);
  int (*udpClientRead)                  (EthernetUDP *);
  int (*udpClientReadBytes)             (EthernetUDP *, unsigned char* buffer, size_t len);
  int (*udpClientReadChars)             (EthernetUDP *, char* buffer, size_t len);
  int (*udpClientPeek)                  (EthernetUDP *);
  void (*udpClientFlush)                (EthernetUDP *);
  struct IPAddress (*udpClientRemoteIP) (EthernetUDP *);
  uint16_t (*udpClientRemotePort)       (EthernetUDP *);
  uint16_t (*udpClientLocalPort)        (EthernetUDP *);
};

void ethernetUDP_init(struct EthernetUDP *ethernetUDP);

typedef struct EthernetClass EthernetClass;
struct EthernetClass {
  struct IPAddress*  _dnsServerAddress;
  struct DhcpClass* _dhcp;
};

int begin(struct EthernetClass *ethernetClass, uint8_t *mac, unsigned long timeout, unsigned long responseTimeout);
int maintain(struct EthernetClass *ethernetClass);
EthernetLinkStatus linkStatus(struct EthernetClass *ethernetClass);
EthernetHardwareStatus hardwareStatus(struct EthernetClass *ethernetClass);
void beginIP(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress * ip);
void beginDNS(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress * ip, struct IPAddress *dns);
void beginGW(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress *ip, struct IPAddress* dns, struct IPAddress *gateway);
void beginSN(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress *ip, struct IPAddress *dns, struct IPAddress *gateway, struct IPAddress *subnet);
void ethernetClassInit(struct EthernetClass *ethernetClass, uint8_t sspin);
void MACAddress(struct EthernetClass *ethernetClass, uint8_t *mac_address);
struct IPAddress * localIP(struct EthernetClass *ethernetClass);
struct IPAddress * subnetMask(struct EthernetClass *ethernetClass);
struct IPAddress * gatewayIP(struct EthernetClass *ethernetClass);
struct IPAddress * dnsServerIP(struct EthernetClass *ethernetClass);
void setMACAddress(struct EthernetClass *ethernetClass, const uint8_t *mac_address);
void setLocalIP(struct EthernetClass *ethernetClass, struct IPAddress * local_ip);
void setSubnetMask(struct EthernetClass *ethernetClass, struct IPAddress * subnet);
void setGatewayIP(struct EthernetClass *ethernetClass, struct IPAddress  * gateway);
void setDnsServerIP(struct EthernetClass *ethernetClass, struct IPAddress * dns_server);
void setRetransmissionTimeout(struct EthernetClass *ethernetClass, uint16_t milliseconds);
void setRetransmissionCount(struct EthernetClass *ethernetClass, uint8_t num);
uint8_t socketBegin(struct EthernetClass *ethernetClass, uint8_t protocol, uint16_t port);
uint8_t socketBeginMulticast(struct EthernetClass *ethernetClass, uint8_t protocol, struct IPAddress * ip,uint16_t port);
uint8_t socketStatus(struct EthernetClass *ethernetClass, uint8_t s);
void socketClose(struct EthernetClass *ethernetClass, uint8_t s);
void socketConnect(struct EthernetClass *ethernetClass, uint8_t s, uint8_t * addr, uint16_t port);
void socketDisconnect(struct EthernetClass *ethernetClass, uint8_t s);
uint8_t socketListen(struct EthernetClass *ethernetClass, uint8_t s);
uint16_t socketSend(struct EthernetClass *ethernetClass, uint8_t s, const uint8_t * buf, uint16_t len);
uint16_t socketSendAvailable(struct EthernetClass *ethernetClass, uint8_t s);
int socketRecv(struct EthernetClass *ethernetClass, uint8_t s, uint8_t * buf, int16_t len);
uint16_t socketRecvAvailable(struct EthernetClass *ethernetClass, uint8_t s);
uint8_t socketPeek(struct EthernetClass *ethernetClass, uint8_t s);
bool socketStartUDP(struct EthernetClass *ethernetClass, uint8_t s, uint8_t* addr, uint16_t port);
uint16_t socketBufferData(struct EthernetClass *ethernetClass, uint8_t s, uint16_t offset, const uint8_t* buf, uint16_t len);
bool socketSendUDP(struct EthernetClass *ethernetClass, uint8_t s);
void socketPortRand(struct EthernetClass *ethernetClass, uint16_t n);

typedef struct EthernetClient EthernetClient;
struct EthernetClient {
  uint8_t sockindex; 
  uint16_t _timeout;
  int (*ethClientConnectIP) (EthernetClient *, struct IPAddress ip, uint16_t port);
  int (*ethClientConnectHost) (EthernetClient *, const char *host, uint16_t port);
  int (*ethClientAvailableForWrite) (EthernetClient *);
  size_t (*ethClientWriteByte) (EthernetClient *, uint8_t);
  size_t (*ethClientWriteBuffer) (EthernetClient *, const uint8_t *buf, size_t size);
  int (*ethClientAvailable) (EthernetClient *);
  int (*ethClientRead)    (EthernetClient *);
  int (*ethClientReadBuffer) (EthernetClient *, uint8_t *buf, size_t size);
  int (*ethClientPeek) (EthernetClient *);
  void (*ethClientFlush) (EthernetClient *);
  void (*ethClientStop) (EthernetClient *);
  uint8_t (*ethClientConnected) (EthernetClient *);
  uint8_t (*ethClientGetSocketNumber) (EthernetClient *);
  uint16_t (*ethClientLocalPort) (EthernetClient *);
  struct IPAddress * (*ethClientRemoteIP) (EthernetClient *);
  uint16_t (*ethClientRemotePort) (EthernetClient *);
  void (*ethClientSetConnectionTimeout) (EthernetClient *, uint16_t timeout);
  uint8_t (*ethClientStatus) (EthernetClient *);
};

void ethernetClient_init(struct EthernetClient *ethernetClient, uint8_t s);

typedef struct DhcpClass DhcpClass;
struct DhcpClass {
  uint32_t _dhcpInitialTransactionId;
	uint32_t _dhcpTransactionId;
	uint8_t  _dhcpMacAddr[6];
	uint8_t  _dhcpLocalIp[4];
	uint8_t  _dhcpSubnetMask[4];
	uint8_t  _dhcpGatewayIp[4];
	uint8_t  _dhcpDhcpServerIp[4];
	uint8_t  _dhcpDnsServerIp[4];
	uint32_t _dhcpLeaseTime;
	uint32_t _dhcpT1, _dhcpT2;
	uint32_t _renewInSec;
	uint32_t _rebindInSec;
	unsigned long _timeout;
	unsigned long _responseTimeout;
	unsigned long _lastCheckLeaseMillis;
	uint8_t _dhcp_state;
	struct EthernetUDP _dhcpUdpSocket;
};

int request_DHCP_lease(struct DhcpClass *dhcpClass);
void reset_DHCP_lease(struct DhcpClass *dhcpClass);
void presend_DHCP(struct DhcpClass *dhcpClass);
void send_DHCP_MESSAGE(struct DhcpClass *dhcpClass, uint8_t, uint16_t);
void printByte(char *, uint8_t);
uint8_t parseDHCPResponse(struct DhcpClass *dhcpClass, unsigned long responseTimeout, uint32_t transactionId);
struct IPAddress * getLocalIp(struct DhcpClass *dhcpClass);
struct IPAddress * getSubnetMask(struct DhcpClass *dhcpClass);
struct IPAddress * getGatewayIp(struct DhcpClass *dhcpClass);
struct IPAddress * getDhcpServerIp(struct DhcpClass *dhcpClass);
struct IPAddress * getDnsServerIp(struct DhcpClass *dhcpClass);
int beginWithDHCP(struct DhcpClass *dhcpClass, uint8_t *, unsigned long timeout, unsigned long responseTimeout);
int checkLease(struct DhcpClass *dhcpClass);

#endif
