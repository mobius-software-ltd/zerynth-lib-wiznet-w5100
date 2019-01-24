#ifndef DNSClient_h
#define DNSClient_h

#include "Ethernet.h"
//#include "utility/w5100.h"

struct DNSClient {
  struct IPAddress * iDNSServer;
	uint16_t iRequestId;
	struct EthernetUDP iUdp;
};

struct DNSClient *dnsClient;

void dnsClientBegin(struct DNSClient *dnsClient, struct IPAddress * aDNSServer);
int dnsInet_aton(struct DNSClient *dnsClient, const char *aIPAddrString, struct IPAddress * aResult);
int dnsGetHostByName(struct DNSClient *dnsClient, const char* aHostname, struct IPAddress * aResult, uint16_t timeout);
uint16_t dnsBuildRequest(struct DNSClient *dnsClient, const char* aName);
uint16_t dnsProcessResponse(struct DNSClient *dnsClient, uint16_t aTimeout, struct IPAddress * aAddress);

#endif
