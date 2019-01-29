//#pragma once
#ifndef ethernet_h_
#define ethernet_h_

#include "socket.h"

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

/*____________EthernetClass_______________________*/
typedef struct EthernetClass {
  struct IPAddress*  _dnsServerAddress;
  struct DhcpClass* _dhcp;
} EthernetClass;

extern EthernetClass *ethernetClass;

uint32_t micros( void );
int begin(EthernetClass *ethernetClass, uint8_t *mac, unsigned long timeout, unsigned long responseTimeout);
int maintain( EthernetClass *ethernetClass);
EthernetLinkStatus linkStatus(EthernetClass *ethernetClass);
EthernetHardwareStatus hardwareStatus(EthernetClass *ethernetClass);
void beginIP(EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress * ip);
void beginDNS(EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress * ip, struct IPAddress *dns);
void beginGW(EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress *ip, struct IPAddress* dns, struct IPAddress *gateway);
void beginSN(EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress *ip, struct IPAddress *dns, struct IPAddress *gateway, struct IPAddress *subnet);
void ethernetClassInit(EthernetClass *ethernetClass, uint8_t sspin);
void MACAddress(EthernetClass *ethernetClass, uint8_t *mac_address);
struct IPAddress * localIP(EthernetClass *ethernetClass);
struct IPAddress * subnetMask(EthernetClass *ethernetClass);
struct IPAddress * gatewayIP(EthernetClass *ethernetClass);
struct IPAddress * dnsServerIP(EthernetClass *ethernetClass);
void setMACAddress(EthernetClass *ethernetClass, const uint8_t *mac_address);
void setLocalIP(EthernetClass *ethernetClass, struct IPAddress * local_ip);
void setSubnetMask(EthernetClass *ethernetClass, struct IPAddress * subnet);
void setGatewayIP(EthernetClass *ethernetClass, struct IPAddress  * gateway);
void setDnsServerIP(EthernetClass *ethernetClass, struct IPAddress * dns_server);
void setRetransmissionTimeout(EthernetClass *ethernetClass, uint16_t milliseconds);
void setRetransmissionCount(EthernetClass *ethernetClass, uint8_t num);

/*____________EthernetClient_______________________*/
typedef struct EthernetClient EthernetClient;
struct EthernetClient {
  uint8_t sockindex;
  uint16_t _timeout;
  int (*ethClientConnectIP) (EthernetClient *, struct IPAddress * ip, uint16_t port);
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

/*____________EthernetUdp________________*/
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

/*____________DHCP_______________________*/
#define STATE_DHCP_START	0
#define	STATE_DHCP_DISCOVER	1
#define	STATE_DHCP_REQUEST	2
#define	STATE_DHCP_LEASED	3
#define	STATE_DHCP_REREQUEST	4
#define	STATE_DHCP_RELEASE	5
#define DHCP_FLAGSBROADCAST	0x8000
#define	DHCP_SERVER_PORT	67
#define DHCP_CLIENT_PORT	68
#define DHCP_BOOTREQUEST	1
#define DHCP_BOOTREPLY		2
#define	DHCP_DISCOVER		1
#define DHCP_OFFER		2
#define	DHCP_REQUEST		3
#define	DHCP_DECLINE		4
#define	DHCP_ACK		5
#define DHCP_NAK		6
#define	DHCP_RELEASE		7
#define DHCP_INFORM		8
#define DHCP_HTYPE10MB		1
#define DHCP_HTYPE100MB		2
#define DHCP_HLENETHERNET	6
#define DHCP_HOPS		0
#define DHCP_SECS		0
#define MAGIC_COOKIE		0x63825363
#define MAX_DHCP_OPT		16
#define HOST_NAME "WIZnet"
#define DEFAULT_LEASE	(900)

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

extern DhcpClass * dhcpClass;

enum
{
	padOption		=	0,
	subnetMaskEnum		=	1,
	timerOffset		=	2,
	routersOnSubnet		=	3,
	dns			=	6,
	hostName		=	12,
	domainName		=	15,
	dhcpRequestedIPaddr	=	50,
	dhcpIPaddrLeaseTime	=	51,
	dhcpMessageType		=	53,
	dhcpServerIdentifier	=	54,
	dhcpParamRequest	=	55,
	dhcpT1value		=	58,
	dhcpT2value		=	59,
	dhcpClientIdentifier	=	61,
	endOption		=	255
};

typedef struct _RIP_MSG_FIXED
{
	uint8_t  op;
	uint8_t  htype;
	uint8_t  hlen;
	uint8_t  hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint8_t  ciaddr[4];
	uint8_t  yiaddr[4];
	uint8_t  siaddr[4];
	uint8_t  giaddr[4];
	uint8_t  chaddr[6];
} RIP_MSG_FIXED;

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

#define DHCP_CHECK_NONE         (0)
#define DHCP_CHECK_RENEW_FAIL   (1)
#define DHCP_CHECK_RENEW_OK     (2)
#define DHCP_CHECK_REBIND_FAIL  (3)
#define DHCP_CHECK_REBIND_OK    (4)

/*____________DNS_______________________*/
typedef struct DNSClient {
  struct IPAddress * iDNSServer;
	uint16_t iRequestId;
	struct EthernetUDP iUdp;
} DNSClient;

extern DNSClient * dnsClient;

void dnsClientBegin(struct DNSClient *dnsClient, struct IPAddress * aDNSServer);
int dnsInet_aton(struct DNSClient *dnsClient, const char *aIPAddrString, struct IPAddress * aResult);
int dnsGetHostByName(struct DNSClient *dnsClient, const char* aHostname, struct IPAddress * aResult, uint16_t timeout);
uint16_t dnsBuildRequest(struct DNSClient *dnsClient, const char* aName);
uint16_t dnsProcessResponse(struct DNSClient *dnsClient, uint16_t aTimeout, struct IPAddress * aAddress);


#endif
