#include "EthernetUdp.h"
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

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t udpClientBegin(EthernetUDP * ethernetUDP, uint16_t port)
{
	if (ethernetUDP->sockindex < MAX_SOCK_NUM) socketClose(ethernetClass, ethernetUDP->sockindex);
	ethernetUDP->sockindex = socketBegin(ethernetClass, SnMR_UDP, port);
	if (ethernetUDP->sockindex >= MAX_SOCK_NUM) return 0;
	ethernetUDP->_port = port;
	ethernetUDP->_remaining = 0;
	return 1;
}

/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int udpClientAvailable(EthernetUDP * ethernetUDP)
{
	return ethernetUDP->_remaining;
}

/* Release any resources being used by this EthernetUDP instance */
void udpClientStop(EthernetUDP * ethernetUDP)
{
	if (ethernetUDP->sockindex < MAX_SOCK_NUM) {
		socketClose(ethernetClass, ethernetUDP->sockindex);
		ethernetUDP->sockindex = MAX_SOCK_NUM;
	}
}

int udpClientBeginPacketHost(EthernetUDP * ethernetUDP, const char *host, uint16_t port)
{
	// Look up the host first
	int ret = 0;
	struct IPAddress * remote_addr;

  dnsClientBegin(dnsClient, dnsServerIP(ethernetClass));

  ret = dnsGetHostByName(dnsClient, host, remote_addr, 5000);
	if (ret != 1) return ret;
	return udpClientBeginPacketIP(ethernetUDP, remote_addr, port);
}

void udpClientBeginPacketIP(EthernetUDP * ethernetUDP, struct IPAddress * ip, uint16_t port)
{
	ethernetUDP->_offset = 0;
	return socketStartUDP(ethernetClass, ethernetUDP->sockindex, rawIPAddress(ip), port);
}

int udpClientEndPacket(EthernetUDP * ethernetUDP)
{
	return socketSendUDP(ethernetClass, ethernetUDP->sockindex);
}

size_t udpClientWriteByte(EthernetUDP * ethernetUDP, uint8_t byte)
{
	return udpClientWriteBuffer(ethernetUDP, &byte, 1);
}

void udpClientWriteBuffer(EthernetUDP * ethernetUDP, const uint8_t *buffer, size_t size)
{
	uint16_t bytes_written = socketBufferData(ethernetClass, ethernetUDP->sockindex, ethernetUDP->_offset, buffer, size);
	ethernetUDP->_offset += bytes_written;
	return bytes_written;
}

int udpClientParsePacket(EthernetUDP * ethernetUDP)
{
	// discard any remaining bytes in the last packet
	while (ethernetUDP->_remaining) {
		// could this fail (loop endlessly) if _remaining > 0 and recv in read fails?
		// should only occur if recv fails after telling us the data is there, lets
		// hope the w5100 always behaves :)
		udpClientReadBytes(ethernetUDP, (uint8_t *)NULL, ethernetUDP->_remaining);
	}

	if (socketRecvAvailable(ethernetClass, ethernetUDP->sockindex) > 0) {
		//HACK - hand-parse the UDP packet using TCP recv method
		uint8_t tmpBuf[8];
		int ret=0;
		//read 8 header bytes and get IP and port from it
		ret = socketRecv(ethernetClass, ethernetUDP->sockindex, tmpBuf, 8);
		if (ret > 0) {
			intIPAddress(&ethernetUDP->_remoteIP, tmpBuf);
			ethernetUDP->_remotePort = tmpBuf[4];
			ethernetUDP->_remotePort = (ethernetUDP->_remotePort << 8) + tmpBuf[5];
			ethernetUDP->_remaining = tmpBuf[6];
			ethernetUDP->_remaining = (ethernetUDP->_remaining << 8) + tmpBuf[7];

			// When we get here, any remaining bytes are the data
			ret = ethernetUDP->_remaining;
		}
		return ret;
	}
	// There aren't any packets available
	return 0;
}

int udpClientRead(EthernetUDP * ethernetUDP)
{
	uint8_t byte;
	if ((ethernetUDP->_remaining > 0) && (socketRecv(ethernetClass, ethernetUDP->sockindex, &byte, 1) > 0)) {
		// We read things without any problems
		ethernetUDP->_remaining--;
		return byte;
	}
	// If we get here, there's no data available
	return -1;
}

int udpClientReadBytes(EthernetUDP * ethernetUDP, unsigned char *buffer, size_t len)
{
	if (ethernetUDP->_remaining > 0) {
		int got;
		if (ethernetUDP->_remaining <= len) {
			// data should fit in the buffer
			got = socketRecv(ethernetClass, ethernetUDP->sockindex, buffer, ethernetUDP->_remaining);
		} else {
			// too much data for the buffer,
			// grab as much as will fit
			got = socketRecv(ethernetClass, ethernetUDP->sockindex, buffer, len);
		}
		if (got > 0) {
			ethernetUDP->_remaining -= got;
			//Serial.printf("UDP read %d\n", got);
			return got;
		}
	}
	// If we get here, there's no data available or recv failed
	return -1;
}

int udpClientPeek(EthernetUDP * ethernetUDP)
{
	// Unlike recv, peek doesn't check to see if there's any data available, so we must.
	// If the user hasn't called parsePacket yet then return nothing otherwise they
	// may get the UDP header
	if (ethernetUDP->sockindex >= MAX_SOCK_NUM || ethernetUDP->_remaining == 0) return -1;
	return socketPeek(ethernetClass, ethernetUDP->sockindex);
}

void udpClientFlush(EthernetUDP * ethernetUDP)
{
	// TODO: we should wait for TX buffer to be emptied
}

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t udpClientBeginMulticast(EthernetUDP * ethernetUDP, struct IPAddress * ip, uint16_t port)
{
	if (ethernetUDP->sockindex < MAX_SOCK_NUM) socketClose(ethernetClass, ethernetUDP->sockindex);
	ethernetUDP->sockindex = socketBeginMulticast(ethernetClass, SnMR_UDP | SnMR_MULTI, ip, port);
	if (ethernetUDP->sockindex >= MAX_SOCK_NUM) return 0;
	ethernetUDP->_port = port;
	ethernetUDP->_remaining = 0;
	return 1;
}
