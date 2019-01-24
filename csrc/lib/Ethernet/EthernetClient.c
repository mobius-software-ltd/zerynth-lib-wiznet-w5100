#include "EthernetClient.h"
/*
#include "Dns.h"
#include "utility/w5100.h"
*/

/*
W5100Class * w5100Class;
SPIClass * spiClass;
struct EthernetClass * ethernetClass;
struct DNSClient *dnsClient;
SPISettings * spiSettings;
spiSettingsConstructor(spiSettings);
#define SPI_ETHERNET_SETTINGS spiSettings
*/

int connectHost(struct EthernetClient * ethernetClient, const char * host, uint16_t port)
{
	struct IPAddress * remote_addr;

	if (ethernetClient->sockindex < MAX_SOCK_NUM) {
		if (socketStatus(ethernetClass, ethernetClient->sockindex) != SnSR_CLOSED) {
			socketDisconnect(ethernetClass, ethernetClient->sockindex);
		}
		ethernetClient->sockindex = MAX_SOCK_NUM;
	}
	dnsClientBegin(dnsClient, dnsServerIP(ethernetClass));
	if (!dnsGetHostByName(dnsClient, host, remote_addr, 5000)) return 0;
	return connectIP(remote_addr, port);
}

int connect(struct EthernetClient * ethernetClient, struct IPAddress * ip, uint16_t port)
{
	if (ethernetClient->sockindex < MAX_SOCK_NUM) {
		if (socketStatus(ethernetClass, ethernetClient->sockindex) != SnSR_CLOSED) {
			socketDisconnect(ethernetClass, ethernetClient->sockindex); // TODO: should we call stop()?
		}
		ethernetClient->sockindex = MAX_SOCK_NUM;
	}

  struct IPAddress * ip0, * ipf;
  dwordIPAddress(ip0, 0ul);
  dwordIPAddress(ipf, 0xFFFFFFFFul);

	if (ip->_address.bytes == ip0->_address.bytes || ip->_address.bytes == ipf->_address.bytes) return 0;

	ethernetClient->sockindex = socketBegin(ethernetClass, SnMR_TCP, 0);
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	socketConnect(ethernetClass, ethernetClient->sockindex, rawIPAddress(ip), port);
	uint32_t start = millis();
	while (1) {
		uint8_t stat = socketStatus(ethernetClass, ethernetClient->sockindex);
		if (stat == SnSR_ESTABLISHED) return 1;
		if (stat == SnSR_CLOSE_WAIT) return 1;
		if (stat == SnSR_CLOSED) return 0;
		if (millis() - start > ethernetClient->_timeout) break;
		delay(1);
	}
	socketClose(ethernetClass, ethernetClient->sockindex);
	ethernetClient->sockindex = MAX_SOCK_NUM;
	return 0;
}

int availableForWrite(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	return socketSendAvailable(ethernetClass, ethernetClient->sockindex);
}

size_t ethClientWriteByte(struct EthernetClient * ethernetClient, uint8_t b)
{
	return ethClientWriteBuffer(ethernetClient, &b, 1);
}

void ethClientWriteBuffer(struct EthernetClient * ethernetClient, const uint8_t *buf, size_t size)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	if (socketSend(ethernetClass, ethernetClient->sockindex, buf, size)) return size;
	setWriteError();
	return 0;
}

int available(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	return socketRecvAvailable(ethernetClass, ethernetClient->sockindex);
	// TODO: do the Wiznet chips automatically retransmit TCP ACK
	// packets if they are lost by the network?  Someday this should
	// be checked by a man-in-the-middle test which discards certain
	// packets.  If ACKs aren't resent, we would need to check for
	// returning 0 here and after a timeout do another Sock_RECV
	// command to cause the Wiznet chip to resend the ACK packet.
}

int ethClientReadBuffer(struct EthernetClient * ethernetClient, uint8_t *buf, size_t size)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	return socketRecv(ethernetClass, ethernetClient->sockindex, buf, size);
}

int ethClientPeek(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return -1;
	if (!available(ethernetClient)) return -1;
	return socketPeek(ethernetClass, ethernetClient->sockindex);
}

int ethClientRead(struct EthernetClient * ethernetClient)
{
	uint8_t b;
	if (socketRecv(ethernetClass, ethernetClient->sockindex, &b, 1) > 0) return b;
	return -1;
}

void ethClientFlush(struct EthernetClient * ethernetClient)
{
	while (ethernetClient->sockindex < MAX_SOCK_NUM) {
		uint8_t stat = socketStatus(ethernetClass, ethernetClient->sockindex);
		if (stat != SnSR_ESTABLISHED && stat != SnSR_CLOSE_WAIT) return;
		if (socketSendAvailable(ethernetClass, ethernetClient->sockindex) >= w5100Class->SSIZE) return;
	}
}

void ethClientStop(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return;

	// attempt to close the connection gracefully (send a FIN to other side)
	socketDisconnect(ethernetClass, ethernetClient->sockindex);
	unsigned long start = millis();

	// wait up to a second for the connection to close
	do {
		if (socketStatus(ethernetClass, ethernetClient->sockindex) == SnSR_CLOSED) {
			ethernetClient->sockindex = MAX_SOCK_NUM;
			return; // exit the loop
		}
		delay(1);
	} while (millis() - start < ethernetClient->_timeout);

	// if it hasn't closed, close it forcefully
	socketClose(ethernetClass, ethernetClient->sockindex);
	ethernetClient->sockindex = MAX_SOCK_NUM;
}

uint8_t ethClientConnected(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;

	uint8_t s = socketStatus(ethernetClass, ethernetClient->sockindex);
	return !(s == SnSR_LISTEN || s == SnSR_CLOSED || s == SnSR_FIN_WAIT ||
		(s == SnSR_CLOSE_WAIT && !available(ethernetClient)));
}

uint8_t ethClientStatus(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return SnSR_CLOSED;
	return socketStatus(ethernetClass, ethernetClient->sockindex);
}

uint16_t ethClientLocalPort(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	uint16_t port;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	port = readSnPORT(w5100Class, ethernetClient->sockindex);
	spiClassEndTransaction(spiClass);
	return port;
}

struct IPAddress * ethClientRemoteIP(struct EthernetClient * ethernetClient)
{
  struct ipAddress * ip;

  if (ethernetClient->sockindex >= MAX_SOCK_NUM) {
    dwordIPAddress(ip, (uint32_t)0);
    return ip;
  }
	uint8_t remoteIParray[4];
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	readSnDIPR(w5100Class, ethernetClient->sockindex, remoteIParray);
	spiClassEndTransaction(spiClass);

  octetsIPAddress(ip, remoteIParray[0],remoteIParray[1],remoteIParray[2],remoteIParray[3]);
	return ip;
}

uint16_t ethClientRemotePort(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	uint16_t port;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	port = readSnDPORT(w5100Class, ethernetClient->sockindex);
	spiClassEndTransaction(spiClass);
	return port;
}
