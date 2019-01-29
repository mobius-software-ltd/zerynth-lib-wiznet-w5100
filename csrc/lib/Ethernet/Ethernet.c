#include "Ethernet.h"

DNSClient * dnsClient;
DhcpClass * dhcpClass;
EthernetClass *ethernetClass;

uint32_t micros( void )
{
    uint32_t ticks, ticks2;
    uint32_t pend, pend2;
    uint32_t count, count2;
    ticks2  = SysTick->VAL;
    pend2   = !!((SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)||((SCB->SHCSR & SCB_SHCSR_SYSTICKACT_Msk)))  ;
    count2  = GetTickCount();

    do {
        ticks=ticks2;
        pend=pend2;
        count=count2;
        ticks2  = SysTick->VAL;
        pend2   = !!((SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)||((SCB->SHCSR & SCB_SHCSR_SYSTICKACT_Msk)))  ;
        count2  = GetTickCount();
    } while ((pend != pend2) || (count != count2) || (ticks < ticks2));

    return ((count+pend) * 1000) + (((SysTick->LOAD  - ticks)*(1048576/(F_CPU/1000000)))>>20) ;
}

int begin(struct EthernetClass *ethernetClass, uint8_t *mac, unsigned long timeout, unsigned long responseTimeout)
{
	DhcpClass s_dhcp;
	ethernetClass->_dhcp = &s_dhcp;

	// Initialise the basic info
	if (w5100ClassInit(w5100Clazz) == 0) return 0;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetMACAddress(w5100Clazz, mac);

	struct IPAddress * addr = {0,0,0,0};
	uint8_t* addrInt = raw_address(addr);
	w5100ClassSetIPAddress(w5100Clazz, addrInt);
	spiClassEndTransaction(spiClass);

	// Now try to get our config info from a DHCP server
	int ret = beginWithDHCP(ethernetClass->_dhcp, mac, timeout, responseTimeout);
	if (ret == 1) {
		// We've successfully found a DHCP server and got our configuration
		// info, so set things accordingly
		spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
		w5100ClassSetIPAddress(w5100Clazz, raw_address(getLocalIp(ethernetClass->_dhcp)));
		w5100ClassSetIPAddress(w5100Clazz, raw_address(getGatewayIp(ethernetClass->_dhcp)));
		w5100ClassSetIPAddress(w5100Clazz, raw_address(getSubnetMask(ethernetClass->_dhcp)));
    spiClassEndTransaction(spiClass);
		ethernetClass->_dnsServerAddress = getDnsServerIp(ethernetClass->_dhcp);
		socketPortRand(micros());
	}
	return ret;
}

void beginIP(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress * ip)
{
	// Assume the DNS server will be the machine on the same network as the local IP
	// but with last octet being '1'
	struct IPAddress * dns = ip;
	setOctetIPAddress(dns, 3, 1);
	beginDNS(ethernetClass, mac, ip, dns);
}

void beginDNS(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress *ip, struct IPAddress *dns)
{
	// Assume the gateway will be the machine on the same network as the local IP
	// but with last octet being '1'
	struct IPAddress * gateway = ip;
	setOctetIPAddress(gateway, 3, 1);
	beginGW(ethernetClass, mac, ip, dns, gateway);
}

void beginGW(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress *ip, struct IPAddress *dns, struct IPAddress *gateway)
{
	struct IPAddress * subnet = {255, 255, 255, 0};
	beginSN(ethernetClass, mac, ip, dns, gateway, subnet);
}

void beginSN(struct EthernetClass *ethernetClass, uint8_t *mac, struct IPAddress *ip, struct IPAddress *dns, struct IPAddress *gateway, struct IPAddress *subnet)
{
	//if (w5100ClassInit(w5100Clazz) == 0) return 0;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetMACAddress(w5100Clazz, mac);

	uint8_t* addr = raw_address(ip);
	w5100ClassSetIPAddress(w5100Clazz, addr);
	addr = raw_address(gateway);
	w5100ClassSetGatewayIp(w5100Clazz, addr);
	addr = raw_address(subnet);
	w5100ClassSetSubnetMask(w5100Clazz, addr);

	spiClassEndTransaction(spiClass);
	ethernetClass->_dnsServerAddress = dns;
}

void ethernetClassInit(struct EthernetClass *ethernetClass, uint8_t sspin)
{
	w5100Clazz->ss_pin = sspin;
	setSS(w5100Clazz);
}

EthernetLinkStatus linkStatus(struct EthernetClass *ethernetClass)
{
	switch (getLinkStatus(w5100Clazz)) {
		case UNKNOWN:  return Unknown;
		case LINK_ON:  return LinkON;
		case LINK_OFF: return LinkOFF;
		default:       return Unknown;
	}
}

EthernetHardwareStatus hardwareStatus(struct EthernetClass *ethernetClass)
{
	switch (getChip(w5100Clazz)) {
		case 51: return EthernetW5100;
		default: return EthernetNoHardware;
	}
}

int maintain(struct EthernetClass *ethernetClass)
{
	int rc = DHCP_CHECK_NONE;
	if (ethernetClass->_dhcp != NULL) {
		// we have a pointer to dhcp, use it
		rc = checkLease(ethernetClass->_dhcp);
		switch (rc) {
		case DHCP_CHECK_NONE:
			//nothing done
			break;
		case DHCP_CHECK_RENEW_OK:
		case DHCP_CHECK_REBIND_OK:
			//we might have got a new IP.
			spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
			w5100ClassSetIPAddress(w5100Clazz, raw_address(getLocalIp(ethernetClass->_dhcp)));
			w5100ClassSetIPAddress(w5100Clazz, raw_address(getGatewayIp(ethernetClass->_dhcp)));
			w5100ClassSetIPAddress(w5100Clazz, raw_address(getSubnetMask(ethernetClass->_dhcp)));
			spiClassEndTransaction(spiClass);
			ethernetClass->_dnsServerAddress = getDnsServerIp(ethernetClass->_dhcp);
			break;
		default:
			//this is actually an error, it will retry though
			break;
		}
	}
	return rc;
}

void MACAddress(struct EthernetClass *ethernetClass, uint8_t *mac_address)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassGetMACAddress(w5100Clazz, mac_address);
	spiClassEndTransaction(spiClass);
}

struct IPAddress * localIP(struct EthernetClass *ethernetClass)
{
	struct IPAddress * ret;
	uint8_t* addr = raw_address(ret);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassGetIPAddress(w5100Clazz, addr);
	spiClassEndTransaction(spiClass);
	return ret;
}

struct IPAddress * subnetMaskMeth(struct EthernetClass *ethernetClass)
{
	struct IPAddress * ret;
	uint8_t* addr = raw_address(ret);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassGetSubnetMask(w5100Clazz, addr);
	spiClassEndTransaction(spiClass);
	return ret;
}

struct IPAddress * gatewayIP(struct EthernetClass *ethernetClass)
{
	struct IPAddress * ret;
	uint8_t* addr = raw_address(ret);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassGetGatewayIp(w5100Clazz, addr);
	spiClassEndTransaction(spiClass);
	return ret;
}

struct IPAddress * dnsServerIP(struct EthernetClass *ethernetClass) { return ethernetClass -> _dnsServerAddress; }

void setMACAddress(struct EthernetClass *ethernetClass, const uint8_t *mac_address)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetMACAddress(w5100Clazz, mac_address);
	spiClassEndTransaction(spiClass);
}

void setLocalIP(struct EthernetClass *ethernetClass, struct IPAddress * local_ip)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint8_t* addr = raw_address(local_ip);
	w5100ClassSetIPAddress(w5100Clazz, addr);
	spiClassEndTransaction(spiClass);
}

void setSubnetMask(struct EthernetClass *ethernetClass, struct IPAddress * subnet)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint8_t* addr = raw_address(subnet);
	w5100ClassSetSubnetMask(w5100Clazz, addr);
	spiClassEndTransaction(spiClass);
}

void setGatewayIP(struct EthernetClass *ethernetClass, struct IPAddress * gateway)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint8_t* addr = raw_address(gateway);
	w5100ClassSetGatewayIp(w5100Clazz, addr);
	spiClassEndTransaction(spiClass);
}

void setDnsServerIP(struct EthernetClass *ethernetClass, struct IPAddress * dns_server) { ethernetClass -> _dnsServerAddress = dns_server; }

void setRetransmissionTimeout(struct EthernetClass *ethernetClass, uint16_t milliseconds)
{
	if (milliseconds > 6553) milliseconds = 6553;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetRetransmissionTime(w5100Clazz, milliseconds * 10);
	spiClassEndTransaction(spiClass);
}

void setRetransmissionCount(struct EthernetClass *ethernetClass, uint8_t num)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetRetransmissionCount(w5100Clazz, num);
	spiClassEndTransaction(spiClass);
}

void ethernetClient_init(struct EthernetClient *ethernetClient, uint8_t s) { ethernetClient -> sockindex = MAX_SOCK_NUM; ethernetClient -> _timeout = s;}

/*____________EthernetUdp________________*/
void ethernetUDP_init(struct EthernetUDP *ethernetUDP) { ethernetUDP -> sockindex = MAX_SOCK_NUM; }

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t udpClientBegin(EthernetUDP * ethernetUDP, uint16_t port)
{
	if (ethernetUDP->sockindex < MAX_SOCK_NUM) socketClose(ethernetUDP->sockindex);
	ethernetUDP->sockindex = socketBegin(SnMR_UDP, port);
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
		socketClose(ethernetUDP->sockindex);
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
	return socketStartUDP(ethernetUDP->sockindex, udpRawIPAddress(ethernetUDP, ip), port);
}

int udpClientEndPacket(EthernetUDP * ethernetUDP)
{
	return socketSendUDP(ethernetUDP->sockindex);
}

size_t udpClientWriteByte(EthernetUDP * ethernetUDP, uint8_t byte)
{
	return udpClientWriteBuffer(ethernetUDP, &byte, 1);
}

void udpClientWriteBuffer(EthernetUDP * ethernetUDP, const uint8_t *buffer, size_t size)
{
	uint16_t bytes_written = socketBufferData(ethernetUDP->sockindex, ethernetUDP->_offset, buffer, size);
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

	if (socketRecvAvailable(ethernetUDP->sockindex) > 0) {
		//HACK - hand-parse the UDP packet using TCP recv method
		uint8_t tmpBuf[8];
		int ret=0;
		//read 8 header bytes and get IP and port from it
		ret = socketRecv(ethernetUDP->sockindex, tmpBuf, 8);
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
	if ((ethernetUDP->_remaining > 0) && (socketRecv(ethernetUDP->sockindex, &byte, 1) > 0)) {
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
			got = socketRecv(ethernetUDP->sockindex, buffer, ethernetUDP->_remaining);
		} else {
			// too much data for the buffer,
			// grab as much as will fit
			got = socketRecv(ethernetUDP->sockindex, buffer, len);
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
	return socketPeek(ethernetUDP->sockindex);
}

void udpClientFlush(EthernetUDP * ethernetUDP)
{
	// TODO: we should wait for TX buffer to be emptied
}

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t udpClientBeginMulticast(EthernetUDP * ethernetUDP, struct IPAddress * ip, uint16_t port)
{
	if (ethernetUDP->sockindex < MAX_SOCK_NUM) socketClose(ethernetUDP->sockindex);
	ethernetUDP->sockindex = socketBeginMulticast(SnMR_UDP | SnMR_MULTI, ip, port);
	if (ethernetUDP->sockindex >= MAX_SOCK_NUM) return 0;
	ethernetUDP->_port = port;
	ethernetUDP->_remaining = 0;
	return 1;
}

/*____________DHCP_______________________*/
int beginWithDHCP(struct DhcpClass *dhcpClass, uint8_t *mac, unsigned long timeout, unsigned long responseTimeout)
{
	dhcpClass->_dhcpLeaseTime=0;
	dhcpClass->_dhcpT1=0;
	dhcpClass->_dhcpT2=0;
	dhcpClass->_timeout = timeout;
	dhcpClass->_responseTimeout = responseTimeout;

	// zero out _dhcpMacAddr
	memset(dhcpClass->_dhcpMacAddr, 0, 6);
	reset_DHCP_lease(dhcpClass);

	memcpy((void*)dhcpClass->_dhcpMacAddr, (void*)mac, 6);
	dhcpClass->_dhcp_state = STATE_DHCP_START;
	return request_DHCP_lease(dhcpClass);
}

void presend_DHCP(struct DhcpClass *dhcpClass)
{
}

void reset_DHCP_lease(struct DhcpClass *dhcpClass)
{
	// zero out _dhcpSubnetMask, _dhcpGatewayIp, _dhcpLocalIp, _dhcpDhcpServerIp, _dhcpDnsServerIp
	memset(dhcpClass->_dhcpLocalIp, 0, 20);
}

	//return:0 on error, 1 if request is sent and response is received
int request_DHCP_lease(struct DhcpClass *dhcpClass)
{
	uint8_t messageType = 0;

	// Pick an initial transaction ID
	dhcpClass->_dhcpTransactionId = rand();
	dhcpClass->_dhcpInitialTransactionId = dhcpClass->_dhcpTransactionId;


	dhcpClass->_dhcpUdpSocket.udpClientStop(&dhcpClass->_dhcpUdpSocket);
	if (dhcpClass->_dhcpUdpSocket.udpClientBegin(&dhcpClass->_dhcpUdpSocket, DHCP_CLIENT_PORT) == 0) {
		// Couldn't get a socket
		return 0;
	}

	presend_DHCP(dhcpClass);

	int result = 0;

	unsigned long startTime = millis();

	while (dhcpClass->_dhcp_state != STATE_DHCP_LEASED) {
		if (dhcpClass->_dhcp_state == STATE_DHCP_START) {
			dhcpClass->_dhcpTransactionId++;
			send_DHCP_MESSAGE(dhcpClass, DHCP_DISCOVER, ((millis() - startTime) / 1000));
			dhcpClass->_dhcp_state = STATE_DHCP_DISCOVER;
		} else if (dhcpClass->_dhcp_state == STATE_DHCP_REREQUEST) {
			dhcpClass->_dhcpTransactionId++;
			send_DHCP_MESSAGE(dhcpClass, DHCP_REQUEST, ((millis() - startTime)/1000));
			dhcpClass->_dhcp_state = STATE_DHCP_REQUEST;
		} else if (dhcpClass->_dhcp_state == STATE_DHCP_DISCOVER) {
			uint32_t respId;
			messageType = parseDHCPResponse(dhcpClass, dhcpClass->_responseTimeout, respId);
			if (messageType == DHCP_OFFER) {
				// We'll use the transaction ID that the offer came with,
				// rather than the one we were up to
				dhcpClass->_dhcpTransactionId = respId;
				send_DHCP_MESSAGE(dhcpClass, DHCP_REQUEST, ((millis() - startTime) / 1000));
				dhcpClass->_dhcp_state = STATE_DHCP_REQUEST;
			}
		} else if (dhcpClass->_dhcp_state == STATE_DHCP_REQUEST) {
			uint32_t respId;
			messageType = parseDHCPResponse(dhcpClass, dhcpClass->_responseTimeout, respId);
			if (messageType == DHCP_ACK) {
				dhcpClass->_dhcp_state = STATE_DHCP_LEASED;
				result = 1;
				//use default lease time if we didn't get it
				if (dhcpClass->_dhcpLeaseTime == 0) {
					dhcpClass->_dhcpLeaseTime = DEFAULT_LEASE;
				}
				// Calculate T1 & T2 if we didn't get it
				if (dhcpClass->_dhcpT1 == 0) {
					// T1 should be 50% of _dhcpLeaseTime
					dhcpClass->_dhcpT1 = dhcpClass->_dhcpLeaseTime >> 1;
				}
				if (dhcpClass->_dhcpT2 == 0) {
					// T2 should be 87.5% (7/8ths) of _dhcpLeaseTime
					dhcpClass->_dhcpT2 = dhcpClass->_dhcpLeaseTime - (dhcpClass->_dhcpLeaseTime >> 3);
				}
				dhcpClass->_renewInSec = dhcpClass->_dhcpT1;
				dhcpClass->_rebindInSec = dhcpClass->_dhcpT2;
			} else if (messageType == DHCP_NAK) {
				dhcpClass->_dhcp_state = STATE_DHCP_START;
			}
		}

		if (messageType == 255) {
			messageType = 0;
			dhcpClass->_dhcp_state = STATE_DHCP_START;
		}

		if (result != 1 && ((millis() - startTime) > dhcpClass->_timeout))
			break;
	}

	// We're done with the socket now
	dhcpClass->_dhcpUdpSocket.udpClientStop(&dhcpClass->_dhcpUdpSocket);
	dhcpClass->_dhcpTransactionId++;

	dhcpClass->_lastCheckLeaseMillis = millis();
	return result;
}

void send_DHCP_MESSAGE(struct DhcpClass *dhcpClass, uint8_t messageType, uint16_t secondsElapsed)
{
	uint8_t buffer[32];
	memset(buffer, 0, 32);
	struct IPAddress * dest_addr = {255, 255, 255, 255}; // Broadcast address

	if (dhcpClass->_dhcpUdpSocket.udpClientBeginPacketIP(&dhcpClass->_dhcpUdpSocket, dest_addr, DHCP_SERVER_PORT) == -1) {
		//Serial.printf("DHCP transmit error\n");
		// FIXME Need to return errors
		return;
	}

	buffer[0] = DHCP_BOOTREQUEST;   // op
	buffer[1] = DHCP_HTYPE10MB;     // htype
	buffer[2] = DHCP_HLENETHERNET;  // hlen
	buffer[3] = DHCP_HOPS;          // hops

	// xid
	unsigned long xid = htonl(dhcpClass->_dhcpTransactionId);
	memcpy(buffer + 4, &(xid), 4);

	// 8, 9 - seconds elapsed
	buffer[8] = ((secondsElapsed & 0xff00) >> 8);
	buffer[9] = (secondsElapsed & 0x00ff);

	// flags
	unsigned short flags = htons(DHCP_FLAGSBROADCAST);
	memcpy(buffer + 10, &(flags), 2);

	// ciaddr: already zeroed
	// yiaddr: already zeroed
	// siaddr: already zeroed
	// giaddr: already zeroed

	//put data in W5100 transmit buffer
	dhcpClass->_dhcpUdpSocket.udpClientWriteBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 28);

	memset(buffer, 0, 32); // clear local buffer

	memcpy(buffer, dhcpClass->_dhcpMacAddr, 6); // chaddr

	//put data in W5100 transmit buffer
	dhcpClass->_dhcpUdpSocket.udpClientWriteBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 16);

	memset(buffer, 0, 32); // clear local buffer

	// leave zeroed out for sname && file
	// put in W5100 transmit buffer x 6 (192 bytes)

	for(int i = 0; i < 6; i++) {
		dhcpClass->_dhcpUdpSocket.udpClientWriteBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 32);
	}

	// OPT - Magic Cookie
	buffer[0] = (uint8_t)((MAGIC_COOKIE >> 24)& 0xFF);
	buffer[1] = (uint8_t)((MAGIC_COOKIE >> 16)& 0xFF);
	buffer[2] = (uint8_t)((MAGIC_COOKIE >> 8)& 0xFF);
	buffer[3] = (uint8_t)(MAGIC_COOKIE& 0xFF);

	// OPT - message type
	buffer[4] = dhcpMessageType;
	buffer[5] = 0x01;
	buffer[6] = messageType; //DHCP_REQUEST;

	// OPT - client identifier
	buffer[7] = dhcpClientIdentifier;
	buffer[8] = 0x07;
	buffer[9] = 0x01;
	memcpy(buffer + 10, dhcpClass->_dhcpMacAddr, 6);

	// OPT - host name
	buffer[16] = hostName;
	buffer[17] = strlen(HOST_NAME) + 6; // length of hostname + last 3 bytes of mac address
	strcpy((char*)&(buffer[18]), HOST_NAME);

	printByte((char*)&(buffer[24]), dhcpClass->_dhcpMacAddr[3]);
	printByte((char*)&(buffer[26]), dhcpClass->_dhcpMacAddr[4]);
	printByte((char*)&(buffer[28]), dhcpClass->_dhcpMacAddr[5]);

	//put data in W5100 transmit buffer
	dhcpClass->_dhcpUdpSocket.udpClientWriteBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 30);

	if (messageType == DHCP_REQUEST) {
		buffer[0] = dhcpRequestedIPaddr;
		buffer[1] = 0x04;
		buffer[2] = dhcpClass->_dhcpLocalIp[0];
		buffer[3] = dhcpClass->_dhcpLocalIp[1];
		buffer[4] = dhcpClass->_dhcpLocalIp[2];
		buffer[5] = dhcpClass->_dhcpLocalIp[3];

		buffer[6] = dhcpServerIdentifier;
		buffer[7] = 0x04;
		buffer[8] = dhcpClass->_dhcpDhcpServerIp[0];
		buffer[9] = dhcpClass->_dhcpDhcpServerIp[1];
		buffer[10] = dhcpClass->_dhcpDhcpServerIp[2];
		buffer[11] = dhcpClass->_dhcpDhcpServerIp[3];

		//put data in W5100 transmit buffer
		dhcpClass->_dhcpUdpSocket.udpClientWriteBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 12);
	}

	buffer[0] = dhcpParamRequest;
	buffer[1] = 0x06;
	buffer[2] = subnetMaskEnum;
	buffer[3] = routersOnSubnet;
	buffer[4] = dns;
	buffer[5] = domainName;
	buffer[6] = dhcpT1value;
	buffer[7] = dhcpT2value;
	buffer[8] = endOption;

	//put data in W5100 transmit buffer
	dhcpClass->_dhcpUdpSocket.udpClientWriteBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 9);

	dhcpClass->_dhcpUdpSocket.udpClientEndPacket(&dhcpClass->_dhcpUdpSocket);
}

uint8_t parseDHCPResponse(struct DhcpClass *dhcpClass, unsigned long responseTimeout, uint32_t transactionId)
{
	uint8_t type = 0;
	uint8_t opt_len = 0;

	unsigned long startTime = millis();

	while (dhcpClass->_dhcpUdpSocket.udpClientParsePacket(&dhcpClass->_dhcpUdpSocket) <= 0) {
		if ((millis() - startTime) > responseTimeout) {
			return 255;
		}
		delay(50);
	}
	// start reading in the packet
	RIP_MSG_FIXED fixedMsg;
	dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket,(uint8_t*)&fixedMsg, sizeof(RIP_MSG_FIXED));

	if (fixedMsg.op == DHCP_BOOTREPLY && dhcpClass->_dhcpUdpSocket.udpClientRemotePort(&dhcpClass->_dhcpUdpSocket) == DHCP_SERVER_PORT) {
		transactionId = ntohl(fixedMsg.xid);
		if (memcmp(fixedMsg.chaddr, dhcpClass->_dhcpMacAddr, 6) != 0 ||
		  (transactionId < dhcpClass->_dhcpInitialTransactionId) ||
		  (transactionId > dhcpClass->_dhcpTransactionId)) {
			// Need to read the rest of the packet here regardless
			dhcpClass->_dhcpUdpSocket.udpClientFlush(&dhcpClass->_dhcpUdpSocket); // FIXME
			return 0;
		}

		memcpy(dhcpClass->_dhcpLocalIp, fixedMsg.yiaddr, 4);

		// Skip to the option part
		dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket,(uint8_t *)NULL, 240 - (int)sizeof(RIP_MSG_FIXED));

		while (dhcpClass->_dhcpUdpSocket.udpClientAvailable(&dhcpClass->_dhcpUdpSocket) > 0) {
			switch (dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket)) {
			case endOption :
				break;

			case padOption :
				break;

			case dhcpMessageType :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				type = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				break;

			case subnetMaskEnum :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.udpClientReadChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpSubnetMask, 4);
				break;

			case routersOnSubnet :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.udpClientReadChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpGatewayIp, 4);
				dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len - 4);
				break;

			case dns :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.udpClientReadChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpDnsServerIp, 4);
				dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len - 4);
				break;

			case dhcpServerIdentifier :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				if ( dhcpClass->_dhcpDhcpServerIp == (uint32_t)0 ||
				  dhcpClass->_dhcpDhcpServerIp == getIP(dhcpClass->_dhcpUdpSocket.udpClientRemoteIP(&dhcpClass->_dhcpUdpSocket)) ) {
					dhcpClass->_dhcpUdpSocket.udpClientReadChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpDhcpServerIp, sizeof(dhcpClass->_dhcpDhcpServerIp));
				} else {
					// Skip over the rest of this option
					dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len);
				}
				break;

			case dhcpT1value :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t*)&dhcpClass->_dhcpT1, sizeof(dhcpClass->_dhcpT1));
				dhcpClass->_dhcpT1 = ntohl(dhcpClass->_dhcpT1);
				break;

			case dhcpT2value :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t*)&dhcpClass->_dhcpT2, sizeof(dhcpClass->_dhcpT2));
				dhcpClass->_dhcpT2 = ntohl(dhcpClass->_dhcpT2);
				break;

			case dhcpIPaddrLeaseTime :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t*)&dhcpClass->_dhcpLeaseTime, sizeof(dhcpClass->_dhcpLeaseTime));
				dhcpClass->_dhcpLeaseTime = ntohl(dhcpClass->_dhcpLeaseTime);
				dhcpClass->_renewInSec = dhcpClass->_dhcpLeaseTime;
				break;

			default :
				opt_len = dhcpClass->_dhcpUdpSocket.udpClientRead(&dhcpClass->_dhcpUdpSocket);
				// Skip over the rest of this option
				dhcpClass->_dhcpUdpSocket.udpClientReadBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len);
				break;
			}
		}
	}
	// Need to skip to end of the packet regardless here
	dhcpClass->_dhcpUdpSocket.udpClientFlush(&dhcpClass->_dhcpUdpSocket); // FIXME

	return type;
}

int checkLease(struct DhcpClass *dhcpClass)
{
	int rc = DHCP_CHECK_NONE;

	unsigned long now = millis();
	unsigned long elapsed = now - dhcpClass->_lastCheckLeaseMillis;

	// if more then one sec passed, reduce the counters accordingly
	if (elapsed >= 1000) {
		// set the new timestamps
		dhcpClass->_lastCheckLeaseMillis = now - (elapsed % 1000);
		elapsed = elapsed / 1000;

		// decrease the counters by elapsed seconds
		// we assume that the cycle time (elapsed) is fairly constant
		// if the remainder is less than cycle time * 2
		// do it early instead of late
		if (dhcpClass->_renewInSec < elapsed * 2) {
			dhcpClass->_renewInSec = 0;
		} else {
			dhcpClass->_renewInSec -= elapsed;
		}
		if (dhcpClass->_rebindInSec < elapsed * 2) {
			dhcpClass->_rebindInSec = 0;
		} else {
			dhcpClass->_rebindInSec -= elapsed;
		}
	}
	// if we have a lease but should renew, do it
	if (dhcpClass->_renewInSec == 0 && dhcpClass->_dhcp_state == STATE_DHCP_LEASED) {
		dhcpClass->_dhcp_state = STATE_DHCP_REREQUEST;
		rc = 1 + request_DHCP_lease(dhcpClass);
	}
	// if we have a lease or is renewing but should bind, do it
	if (dhcpClass->_rebindInSec == 0 && (dhcpClass->_dhcp_state == STATE_DHCP_LEASED ||
	  dhcpClass->_dhcp_state == STATE_DHCP_START)) {
		// this should basically restart completely
		dhcpClass->_dhcp_state = STATE_DHCP_START;
		reset_DHCP_lease(dhcpClass);
		rc = 3 + request_DHCP_lease(dhcpClass);
	}
	return rc;
}

struct IPAddress * getLocalIp(struct DhcpClass *dhcpClass)
{
	struct IPAddress * address;
	dwordIPAddress(address, dhcpClass->_dhcpLocalIp);
	return address;
}

struct IPAddress * getSubnetMask(struct DhcpClass *dhcpClass)
{
	struct IPAddress * address;
	dwordIPAddress(address, dhcpClass->_dhcpSubnetMask);
	return address;
}

struct IPAddress * getGatewayIp(struct DhcpClass *dhcpClass)
{
	struct IPAddress * address;
	dwordIPAddress(address, dhcpClass->_dhcpGatewayIp);
	return address;
}

struct IPAddress * getDhcpServerIp(struct DhcpClass *dhcpClass)
{
	struct IPAddress * address;
	dwordIPAddress(address, dhcpClass->_dhcpDhcpServerIp);
	return address;
}

struct IPAddress * getDnsServerIp(struct DhcpClass *dhcpClass)
{
	struct IPAddress * address;
	dwordIPAddress(address, dhcpClass->_dhcpDnsServerIp);
	return address;
}

void printByte(char * buf, uint8_t n )
{
	char *str = &buf[1];
	buf[0]='0';
	do {
		unsigned long m = n;
		n /= 16;
		char c = m - 16 * n;
		*str-- = c < 10 ? c + '0' : c + 'A' - 10;
	} while(n);
}

/*____________DNS_______________________*/
#define SOCKET_NONE              255
// Various flags and header field values for a DNS message
#define UDP_HEADER_SIZE          8
#define DNS_HEADER_SIZE          12
#define TTL_SIZE                 4
#define QUERY_FLAG               (0)
#define RESPONSE_FLAG            (1<<15)
#define QUERY_RESPONSE_MASK      (1<<15)
#define OPCODE_STANDARD_QUERY    (0)
#define OPCODE_INVERSE_QUERY     (1<<11)
#define OPCODE_STATUS_REQUEST    (2<<11)
#define OPCODE_MASK              (15<<11)
#define AUTHORITATIVE_FLAG       (1<<10)
#define TRUNCATION_FLAG          (1<<9)
#define RECURSION_DESIRED_FLAG   (1<<8)
#define RECURSION_AVAILABLE_FLAG (1<<7)
#define RESP_NO_ERROR            (0)
#define RESP_FORMAT_ERROR        (1)
#define RESP_SERVER_FAILURE      (2)
#define RESP_NAME_ERROR          (3)
#define RESP_NOT_IMPLEMENTED     (4)
#define RESP_REFUSED             (5)
#define RESP_MASK                (15)
#define TYPE_A                   (0x0001)
#define CLASS_IN                 (0x0001)
#define LABEL_COMPRESSION_MASK   (0xC0)
// Port number that DNS servers listen on
#define DNS_PORT        53

// Possible return codes from ProcessResponse
#define SUCCESS          1
#define TIMED_OUT        -1
#define INVALID_SERVER   -2
#define TRUNCATED        -3
#define INVALID_RESPONSE -4

void dnsClientBegin(struct DNSClient *dnsClient, struct IPAddress * aDNSServer)
{
	dnsClient->iDNSServer = aDNSServer;
	dnsClient->iRequestId = 0;
}

int dnsInet_aton(struct DNSClient *dnsClient, const char* address, struct IPAddress * result)
{
	uint16_t acc = 0; // Accumulator
	uint8_t dots = 0;

	while (*address) {
		char c = *address++;
		if (c >= '0' && c <= '9') {
			acc = acc * 10 + (c - '0');
			if (acc > 255) {
				// Value out of [0..255] range
				return 0;
			}
		} else if (c == '.') {
			if (dots == 3) {
				// Too much dots (there must be 3 dots)
				return 0;
			}
    	setOctetIPAddress(result, dots++, acc);
			acc = 0;
		} else {
			// Invalid char
			return 0;
		}
	}

	if (dots != 3) {
		// Too few dots (there must be 3 dots)
		return 0;
	}
  setOctetIPAddress(result, 3, acc);
	return 1;
}

uint16_t dnsProcessResponse(struct DNSClient *dnsClient, uint16_t aTimeout, struct IPAddress * aAddress)
{
	uint32_t startTime = millis();

	// Wait for a response packet
	while (dnsClient->iUdp.udpClientParsePacket(&dnsClient->iUdp) <= 0) {
		if ((millis() - startTime) > aTimeout) {
			return TIMED_OUT;
		}
		delay(50);
	}

	// We've had a reply!
	// Read the UDP header
	//uint8_t header[DNS_HEADER_SIZE]; // Enough space to reuse for the DNS header
	union {
		uint8_t  byte[DNS_HEADER_SIZE]; // Enough space to reuse for the DNS header
		uint16_t word[DNS_HEADER_SIZE/2];
	} header;

	// Check that it's a response from the right server and the right port
	if ( (dnsClient->iDNSServer != (dnsClient->iUdp.udpClientRemoteIP(&dnsClient->iUdp))._address.bytes) || (dnsClient->iUdp.udpClientRemotePort(&dnsClient->iUdp) != DNS_PORT)) {
		// It's not from who we expected
		return INVALID_SERVER;
	}

	// Read through the rest of the response
	if (dnsClient->iUdp.udpClientAvailable(&dnsClient->iUdp) < DNS_HEADER_SIZE) {
		return TRUNCATED;
	}
	dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, header.byte, DNS_HEADER_SIZE);

	uint16_t header_flags = htons(header.word[1]);
	// Check that it's a response to this request
	if ((dnsClient->iRequestId != (header.word[0])) ||
	  ((header_flags & QUERY_RESPONSE_MASK) != (uint16_t)RESPONSE_FLAG) ) {
		// Mark the entire packet as read
		dnsClient->iUdp.udpClientFlush(&dnsClient->iUdp); // FIXME
		return INVALID_RESPONSE;
	}
	// Check for any errors in the response (or in our request)
	// although we don't do anything to get round these
	if ( (header_flags & TRUNCATION_FLAG) || (header_flags & RESP_MASK) ) {
		// Mark the entire packet as read
		dnsClient->iUdp.udpClientFlush(&dnsClient->iUdp); // FIXME
		return -5; //INVALID_RESPONSE;
	}

	// And make sure we've got (at least) one answer
	uint16_t answerCount = htons(header.word[3]);
	if (answerCount == 0) {
		// Mark the entire packet as read
		dnsClient->iUdp.udpClientFlush(&dnsClient->iUdp); // FIXME
		return -6; //INVALID_RESPONSE;
	}

	// Skip over any questions
	for (uint16_t i=0; i < htons(header.word[2]); i++) {
		// Skip over the name
		uint8_t len;
		do {
			dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, &len, sizeof(len));
			if (len > 0) {
				// Don't need to actually read the data out for the string, just
				// advance ptr to beyond it
				dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t *)NULL, (size_t)len);
			}
		} while (len != 0);

		// Now jump over the type and class
		dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t *)NULL, 4);
	}

	// Now we're up to the bit we're interested in, the answer
	// There might be more than one answer (although we'll just use the first
	// type A answer) and some authority and additional resource records but
	// we're going to ignore all of them.

	for (uint16_t i=0; i < answerCount; i++) {
		// Skip the name
		uint8_t len;
		do {
			dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, &len, sizeof(len));
			if ((len & LABEL_COMPRESSION_MASK) == 0) {
				// It's just a normal label
				if (len > 0) {
					// And it's got a length
					// Don't need to actually read the data out for the string,
					// just advance ptr to beyond it
					dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t *)NULL, len);
				}
			} else {
				// This is a pointer to a somewhere else in the message for the
				// rest of the name.  We don't care about the name, and RFC1035
				// says that a name is either a sequence of labels ended with a
				// 0 length octet or a pointer or a sequence of labels ending in
				// a pointer.  Either way, when we get here we're at the end of
				// the name
				// Skip over the pointer
				dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t *)NULL, 1); // we don't care about the byte
				// And set len so that we drop out of the name loop
				len = 0;
			}
		} while (len != 0);

		// Check the type and class
		uint16_t answerType;
		uint16_t answerClass;
		dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t*)&answerType, sizeof(answerType));
		dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t*)&answerClass, sizeof(answerClass));

		// Ignore the Time-To-Live as we don't do any caching
		dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t *)NULL, TTL_SIZE); // don't care about the returned bytes

		// And read out the length of this answer
		// Don't need header_flags anymore, so we can reuse it here
		dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t*)&header_flags, sizeof(header_flags));

		if ( (htons(answerType) == TYPE_A) && (htons(answerClass) == CLASS_IN) ) {
			if (htons(header_flags) != 4) {
				// It's a weird size
				// Mark the entire packet as read
				dnsClient->iUdp.udpClientFlush(&dnsClient->iUdp); // FIXME
				return -9;//INVALID_RESPONSE;
			}
			// FIXME: seeems to lock up here on ESP8266, but why??
    	uint8_t* addr = raw_address(aAddress);
			dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, addr, 4);
			return SUCCESS;
		} else {
			// This isn't an answer type we're after, move onto the next one
			dnsClient->iUdp.udpClientReadBytes(&dnsClient->iUdp, (uint8_t *)NULL, htons(header_flags));
		}
	}

	// Mark the entire packet as read
	dnsClient->iUdp.udpClientFlush(&dnsClient->iUdp); // FIXME

	// If we get here then we haven't found an answer
	return -10; //INVALID_RESPONSE;
}

int dnsGetHostByName(struct DNSClient *dnsClient, const char* aHostname, struct IPAddress * aResult, uint16_t timeout)
{
	int ret = 0;

	// See if it's a numeric IP address
	if (dnsInet_aton(dnsClient, aHostname, aResult)) {
		// It is, our work here is done
		return 1;
	}

	// Check we've got a valid DNS server to use
	if (dnsClient->iDNSServer == INADDR_NONE._address.bytes) {
		return INVALID_SERVER;
	}

	// Find a socket to use
	if (dnsClient->iUdp.udpClientBegin(&dnsClient->iUdp, 1024+(millis() & 0xF)) == 1) {
		// Try up to three times
		int retries = 0;
		// while ((retries < 3) && (ret <= 0)) {
		// Send DNS request
		ret = dnsClient->iUdp.udpClientBeginPacketIP(&dnsClient->iUdp, dnsClient->iDNSServer, DNS_PORT);
		if (ret != 0) {
			// Now output the request data
			ret = dnsBuildRequest(dnsClient, aHostname);
			if (ret != 0) {
				// And finally send the request
				ret = dnsClient->iUdp.udpClientEndPacket(&dnsClient->iUdp);
				if (ret != 0) {
					// Now wait for a response
					int wait_retries = 0;
					ret = TIMED_OUT;
					while ((wait_retries < 3) && (ret == TIMED_OUT)) {
						ret = dnsProcessResponse(dnsClient, timeout, aResult);
						wait_retries++;
					}
				}
			}
		}
		retries++;
		//}

		// We're done with the socket now
		dnsClient->iUdp.udpClientStop(&dnsClient->iUdp);
	}

	return ret;
}

uint16_t dnsBuildRequest(struct DNSClient *dnsClient, const char* aName)
{
	// Build header
	//                                    1  1  1  1  1  1
	//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
	//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	//    |                      ID                       |
	//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	//    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
	//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	//    |                    QDCOUNT                    |
	//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	//    |                    ANCOUNT                    |
	//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	//    |                    NSCOUNT                    |
	//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	//    |                    ARCOUNT                    |
	//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
	// As we only support one request at a time at present, we can simplify
	// some of this header
	dnsClient->iRequestId = millis(); // generate a random ID
	uint16_t twoByteBuffer;

	// FIXME We should also check that there's enough space available to write to, rather
	// FIXME than assume there's enough space (as the code does at present)
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&dnsClient->iRequestId, sizeof(dnsClient->iRequestId));

	twoByteBuffer = htons(QUERY_FLAG | OPCODE_STANDARD_QUERY | RECURSION_DESIRED_FLAG);
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));

	twoByteBuffer = htons(1);  // One question record
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));

	twoByteBuffer = 0;  // Zero answer records
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));

	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
	// and zero additional records
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));

	// Build question
	const char* start =aName;
	const char* end =start;
	uint8_t len;
	// Run through the name being requested
	while (*end) {
		// Find out how long this section of the name is
		end = start;
		while (*end && (*end != '.') ) {
			end++;
		}

		if (end-start > 0) {
			// Write out the size of this section
			len = end-start;
			dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, &len, sizeof(len));
			// And then write out the section
			dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)start, end-start);
		}
		start = end+1;
	}
	// We've got to the end of the question name, so
	// terminate it with a zero-length section
	len = 0;
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, &len, sizeof(len));
	// Finally the type and class of question
	twoByteBuffer = htons(TYPE_A);
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));

	twoByteBuffer = htons(CLASS_IN);  // Internet class of question
	dnsClient->iUdp.udpClientWriteBuffer(&dnsClient->iUdp, (uint8_t*)&twoByteBuffer, sizeof(twoByteBuffer));
	// Success!  Everything buffered okay
	return 1;
}



/*____________EthernetClient_______________________*/
int connectHost(EthernetClient * ethernetClient, const char * host, uint16_t port)
{
	struct IPAddress * remote_addr;

	if (ethernetClient->sockindex < MAX_SOCK_NUM) {
		if (socketStatus(ethernetClient->sockindex) != SnSR_CLOSED) {
			socketDisconnect(ethernetClient->sockindex);
		}
		ethernetClient->sockindex = MAX_SOCK_NUM;
	}
	dnsClientBegin(dnsClient, dnsServerIP(ethernetClass));
	if (!dnsGetHostByName(dnsClient, host, remote_addr, 5000)) return 0;

	return ethernetClient->ethClientConnectIP(&ethernetClient, remote_addr, port);
}

int ethClientConnectIP(EthernetClient * ethernetClient, struct IPAddress * ip, uint16_t port)
{
	if (ethernetClient->sockindex < MAX_SOCK_NUM) {
		if (socketStatus(ethernetClient->sockindex) != SnSR_CLOSED) {
			socketDisconnect(ethernetClient->sockindex); // TODO: should we call stop()?
		}
		ethernetClient->sockindex = MAX_SOCK_NUM;
	}

  struct IPAddress * ip0, * ipf;
  dwordIPAddress(ip0, 0ul);
  dwordIPAddress(ipf, 0xFFFFFFFFul);

	if (ip->_address.bytes == ip0->_address.bytes || ip->_address.bytes == ipf->_address.bytes) return 0;

	ethernetClient->sockindex = socketBegin(SnMR_TCP, 0);
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	socketConnect(ethernetClient->sockindex, clientRawIPAddress(ethernetClient, ip), port);
	uint32_t start = millis();
	while (1) {
		uint8_t stat = socketStatus(ethernetClient->sockindex);
		if (stat == SnSR_ESTABLISHED) return 1;
		if (stat == SnSR_CLOSE_WAIT) return 1;
		if (stat == SnSR_CLOSED) return 0;
		if (millis() - start > ethernetClient->_timeout) break;
		delay(1);
	}
	socketClose(ethernetClient->sockindex);
	ethernetClient->sockindex = MAX_SOCK_NUM;
	return 0;
}

int availableForWrite(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	return socketSendAvailable(ethernetClient->sockindex);
}

size_t ethClientWriteByte(struct EthernetClient * ethernetClient, uint8_t b)
{
	return ethClientWriteBuffer(ethernetClient, &b, 1);
}

void ethClientWriteBuffer(struct EthernetClient * ethernetClient, const uint8_t *buf, size_t size)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	if (socketSend(ethernetClient->sockindex, buf, size)) return size;
	//setWriteError();
	return 0;
}

int available(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	return socketRecvAvailable(ethernetClient->sockindex);
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
	return socketRecv(ethernetClient->sockindex, buf, size);
}

int ethClientPeek(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return -1;
	if (!available(ethernetClient)) return -1;
	return socketPeek(ethernetClient->sockindex);
}

int ethClientRead(struct EthernetClient * ethernetClient)
{
	uint8_t b;
	if (socketRecv(ethernetClient->sockindex, &b, 1) > 0) return b;
	return -1;
}

void ethClientFlush(struct EthernetClient * ethernetClient)
{
	while (ethernetClient->sockindex < MAX_SOCK_NUM) {
		uint8_t stat = socketStatus(ethernetClient->sockindex);
		if (stat != SnSR_ESTABLISHED && stat != SnSR_CLOSE_WAIT) return;
		if (socketSendAvailable(ethernetClient->sockindex) >= w5100Clazz->SSIZE) return;
	}
}

void ethClientStop(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return;

	// attempt to close the connection gracefully (send a FIN to other side)
	socketDisconnect(ethernetClient->sockindex);
	unsigned long start = millis();

	// wait up to a second for the connection to close
	do {
		if (socketStatus(ethernetClient->sockindex) == SnSR_CLOSED) {
			ethernetClient->sockindex = MAX_SOCK_NUM;
			return; // exit the loop
		}
		delay(1);
	} while (millis() - start < ethernetClient->_timeout);

	// if it hasn't closed, close it forcefully
	socketClose(ethernetClient->sockindex);
	ethernetClient->sockindex = MAX_SOCK_NUM;
}

uint8_t ethClientConnected(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;

	uint8_t s = socketStatus(ethernetClient->sockindex);
	return !(s == SnSR_LISTEN || s == SnSR_CLOSED || s == SnSR_FIN_WAIT ||
		(s == SnSR_CLOSE_WAIT && !available(ethernetClient)));
}

uint8_t ethClientStatus(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return SnSR_CLOSED;
	return socketStatus(ethernetClient->sockindex);
}

uint16_t ethClientLocalPort(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	uint16_t port;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	port = readSnPORT(w5100Clazz, ethernetClient->sockindex);
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
	readSnDIPR(w5100Clazz, ethernetClient->sockindex, remoteIParray);
	spiClassEndTransaction(spiClass);

  octetsIPAddress(ip, remoteIParray[0],remoteIParray[1],remoteIParray[2],remoteIParray[3]);
	return ip;
}

uint16_t ethClientRemotePort(struct EthernetClient * ethernetClient)
{
	if (ethernetClient->sockindex >= MAX_SOCK_NUM) return 0;
	uint16_t port;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	port = readSnDPORT(w5100Clazz, ethernetClient->sockindex);
	spiClassEndTransaction(spiClass);
	return port;
}
