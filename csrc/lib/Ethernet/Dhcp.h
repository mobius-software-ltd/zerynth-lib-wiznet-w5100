#ifndef Dhcp_h
#define Dhcp_h

#include "inttypes.h"

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
#define DHCP_CHECK_NONE         (0)
#define DHCP_CHECK_RENEW_FAIL   (1)
#define DHCP_CHECK_RENEW_OK     (2)
#define DHCP_CHECK_REBIND_FAIL  (3)
#define DHCP_CHECK_REBIND_OK    (4)

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

#endif
