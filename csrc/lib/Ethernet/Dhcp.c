#include "Dhcp.h"

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
	dhcpClass->_dhcpTransactionId = random(1UL, 2000UL);
	dhcpClass->_dhcpInitialTransactionId = dhcpClass->_dhcpTransactionId;


	dhcpClass->_dhcpUdpSocket.stop(&dhcpClass->_dhcpUdpSocket);
	if (dhcpClass->_dhcpUdpSocket.begin(&dhcpClass->_dhcpUdpSocket, DHCP_CLIENT_PORT) == 0) {
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
	dhcpClass->_dhcpUdpSocket.stop(&dhcpClass->_dhcpUdpSocket);
	dhcpClass->_dhcpTransactionId++;

	dhcpClass->_lastCheckLeaseMillis = millis();
	return result;
}

void send_DHCP_MESSAGE(struct DhcpClass *dhcpClass, uint8_t messageType, uint16_t secondsElapsed)
{
	uint8_t buffer[32];
	memset(buffer, 0, 32);
	struct IPAddress dest_addr = {255, 255, 255, 255}; // Broadcast address

	if (dhcpClass->_dhcpUdpSocket.beginPacketIP(&dhcpClass->_dhcpUdpSocket, dest_addr, DHCP_SERVER_PORT) == -1) {
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
	dhcpClass->_dhcpUdpSocket.writeBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 28);

	memset(buffer, 0, 32); // clear local buffer

	memcpy(buffer, dhcpClass->_dhcpMacAddr, 6); // chaddr

	//put data in W5100 transmit buffer
	dhcpClass->_dhcpUdpSocket.writeBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 16);

	memset(buffer, 0, 32); // clear local buffer

	// leave zeroed out for sname && file
	// put in W5100 transmit buffer x 6 (192 bytes)

	for(int i = 0; i < 6; i++) {
		dhcpClass->_dhcpUdpSocket.writeBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 32);
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
	dhcpClass->_dhcpUdpSocket.writeBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 30);

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
		dhcpClass->_dhcpUdpSocket.writeBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 12);
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
	dhcpClass->_dhcpUdpSocket.writeBuffer(&dhcpClass->_dhcpUdpSocket, buffer, 9);

	dhcpClass->_dhcpUdpSocket.endPacket(&dhcpClass->_dhcpUdpSocket);
}

uint8_t parseDHCPResponse(struct DhcpClass *dhcpClass, unsigned long responseTimeout, uint32_t transactionId)
{
	uint8_t type = 0;
	uint8_t opt_len = 0;

	unsigned long startTime = millis();

	while (dhcpClass->_dhcpUdpSocket.parsePacket(&dhcpClass->_dhcpUdpSocket) <= 0) {
		if ((millis() - startTime) > responseTimeout) {
			return 255;
		}
		delay(50);
	}
	// start reading in the packet
	RIP_MSG_FIXED fixedMsg;
	dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket,(uint8_t*)&fixedMsg, sizeof(RIP_MSG_FIXED));

	if (fixedMsg.op == DHCP_BOOTREPLY && dhcpClass->_dhcpUdpSocket.remotePort(&dhcpClass->_dhcpUdpSocket) == DHCP_SERVER_PORT) {
		transactionId = ntohl(fixedMsg.xid);
		if (memcmp(fixedMsg.chaddr, dhcpClass->_dhcpMacAddr, 6) != 0 ||
		  (transactionId < dhcpClass->_dhcpInitialTransactionId) ||
		  (transactionId > dhcpClass->_dhcpTransactionId)) {
			// Need to read the rest of the packet here regardless
			dhcpClass->_dhcpUdpSocket.flush(&dhcpClass->_dhcpUdpSocket); // FIXME
			return 0;
		}

		memcpy(dhcpClass->_dhcpLocalIp, fixedMsg.yiaddr, 4);

		// Skip to the option part
		dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket,(uint8_t *)NULL, 240 - (int)sizeof(RIP_MSG_FIXED));

		while (dhcpClass->_dhcpUdpSocket.available(&dhcpClass->_dhcpUdpSocket) > 0) {
			switch (dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket)) {
			case endOption :
				break;

			case padOption :
				break;

			case dhcpMessageType :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				type = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				break;

			case subnetMaskEnum :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.readChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpSubnetMask, 4);
				break;

			case routersOnSubnet :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.readChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpGatewayIp, 4);
				dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len - 4);
				break;

			case dns :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.readChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpDnsServerIp, 4);
				dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len - 4);
				break;

			case dhcpServerIdentifier :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				if ( dhcpClass->_dhcpDhcpServerIp == (uint32_t)0 ||
				  dhcpClass->_dhcpDhcpServerIp == getIP(dhcpClass->_dhcpUdpSocket.remoteIP(&dhcpClass->_dhcpUdpSocket)) ) {
					dhcpClass->_dhcpUdpSocket.readChars(&dhcpClass->_dhcpUdpSocket, dhcpClass->_dhcpDhcpServerIp, sizeof(dhcpClass->_dhcpDhcpServerIp));
				} else {
					// Skip over the rest of this option
					dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len);
				}
				break;

			case dhcpT1value :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t*)&dhcpClass->_dhcpT1, sizeof(dhcpClass->_dhcpT1));
				dhcpClass->_dhcpT1 = ntohl(dhcpClass->_dhcpT1);
				break;

			case dhcpT2value :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t*)&dhcpClass->_dhcpT2, sizeof(dhcpClass->_dhcpT2));
				dhcpClass->_dhcpT2 = ntohl(dhcpClass->_dhcpT2);
				break;

			case dhcpIPaddrLeaseTime :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t*)&dhcpClass->_dhcpLeaseTime, sizeof(dhcpClass->_dhcpLeaseTime));
				dhcpClass->_dhcpLeaseTime = ntohl(dhcpClass->_dhcpLeaseTime);
				dhcpClass->_renewInSec = dhcpClass->_dhcpLeaseTime;
				break;

			default :
				opt_len = dhcpClass->_dhcpUdpSocket.read(&dhcpClass->_dhcpUdpSocket);
				// Skip over the rest of this option
				dhcpClass->_dhcpUdpSocket.readBytes(&dhcpClass->_dhcpUdpSocket, (uint8_t *)NULL, opt_len);
				break;
			}
		}
	}
	// Need to skip to end of the packet regardless here
	dhcpClass->_dhcpUdpSocket.flush(&dhcpClass->_dhcpUdpSocket); // FIXME

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
