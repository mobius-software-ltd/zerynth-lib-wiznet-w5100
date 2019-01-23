#include "Ethernet.h"
#include "Dhcp.h"

void ethernetUDP_init(struct EthernetUDP *ethernetUDP) { ethernetUDP -> sockindex = MAX_SOCK_NUM; }

int begin(struct EthernetClass *ethernetClass, uint8_t *mac, unsigned long timeout, unsigned long responseTimeout)
{
	DhcpClass s_dhcp;
	ethernetClass->_dhcp = &s_dhcp;

	// Initialise the basic info
	if (w5100ClassInit(w5100Class) == 0) return 0;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetMACAddress(w5100Class, mac);

	struct IPAddress * addr = {0,0,0,0};
	uint8_t* addrInt = raw_address(addr);
	w5100ClassSetIPAddress(w5100Class, addrInt);
	spiClassEndTransaction(spiClass);

	// Now try to get our config info from a DHCP server
	int ret = beginWithDHCP(ethernetClass->_dhcp, mac, timeout, responseTimeout);
	if (ret == 1) {
		// We've successfully found a DHCP server and got our configuration
		// info, so set things accordingly
		spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
		w5100ClassSetIPAddress(w5100Class, raw_address(getLocalIp(ethernetClass->_dhcp)));
		w5100ClassSetIPAddress(w5100Class, raw_address(getGatewayIp(ethernetClass->_dhcp)));
		w5100ClassSetIPAddress(w5100Class, raw_address(getSubnetMask(ethernetClass->_dhcp)));
		spiClassEndTransaction(spiClass);
		ethernetClass->_dnsServerAddress = getDnsServerIp(ethernetClass->_dhcp);
		socketPortRand(ethernetClass, micros());
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
	if (w5100ClassInit(w5100Class) == 0) return 0;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetMACAddress(w5100Class, mac);

	uint8_t* addr = raw_address(ip);
	w5100ClassSetIPAddress(w5100Class, addr);
	addr = raw_address(gateway);
	w5100ClassSetGatewayIp(w5100Class, addr);
	addr = raw_address(subnet);
	w5100ClassSetSubnetMask(w5100Class, addr);

	spiClassEndTransaction(spiClass);
	ethernetClass->_dnsServerAddress = dns;
}

void ethernetClassInit(struct EthernetClass *ethernetClass, uint8_t sspin)
{
	w5100Class->ss_pin = sspin;
	setSS(w5100Class);
}

EthernetLinkStatus linkStatus(struct EthernetClass *ethernetClass)
{
	switch (getLinkStatus(w5100Class)) {
		case UNKNOWN:  return Unknown;
		case LINK_ON:  return LinkON;
		case LINK_OFF: return LinkOFF;
		default:       return Unknown;
	}
}

EthernetHardwareStatus hardwareStatus(struct EthernetClass *ethernetClass)
{
	switch (getChip(w5100Class)) {
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
			w5100ClassSetIPAddress(w5100Class, raw_address(getLocalIp(ethernetClass->_dhcp)));
			w5100ClassSetIPAddress(w5100Class, raw_address(getGatewayIp(ethernetClass->_dhcp)));
			w5100ClassSetIPAddress(w5100Class, raw_address(getSubnetMask(ethernetClass->_dhcp)));
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
	w5100ClassGetMACAddress(w5100Class, mac_address);
	spiClassEndTransaction(spiClass);
}

struct IPAddress * localIP(struct EthernetClass *ethernetClass)
{
	struct IPAddress * ret;
	uint8_t* addr = raw_address(ret);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassGetIPAddress(w5100Class, addr);
	spiClassEndTransaction(spiClass);
	return ret;
}

struct IPAddress * subnetMaskMeth(struct EthernetClass *ethernetClass)
{
	struct IPAddress * ret;
	uint8_t* addr = raw_address(ret);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassGetSubnetMask(w5100Class, addr);
	spiClassEndTransaction(spiClass);
	return ret;
}

struct IPAddress * gatewayIP(struct EthernetClass *ethernetClass)
{
	struct IPAddress * ret;
	uint8_t* addr = raw_address(ret);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassGetGatewayIp(w5100Class, addr);
	spiClassEndTransaction(spiClass);
	return ret;
}

struct IPAddress * dnsServerIP(struct EthernetClass *ethernetClass) { return ethernetClass -> _dnsServerAddress; }

void setMACAddress(struct EthernetClass *ethernetClass, const uint8_t *mac_address)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetMACAddress(w5100Class, mac_address);
	spiClassEndTransaction(spiClass);
}

void setLocalIP(struct EthernetClass *ethernetClass, struct IPAddress * local_ip)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint8_t* addr = raw_address(local_ip);
	w5100ClassSetIPAddress(w5100Class, addr);
	spiClassEndTransaction(spiClass);
}

void setSubnetMask(struct EthernetClass *ethernetClass, struct IPAddress * subnet)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint8_t* addr = raw_address(subnet);
	w5100ClassSetSubnetMask(w5100Class, addr);
	spiClassEndTransaction(spiClass);
}

void setGatewayIP(struct EthernetClass *ethernetClass, struct IPAddress * gateway)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint8_t* addr = raw_address(gateway);
	w5100ClassSetGatewayIp(w5100Class, addr);
	spiClassEndTransaction(spiClass);
}

void setDnsServerIP(struct EthernetClass *ethernetClass, struct IPAddress * dns_server) { ethernetClass -> _dnsServerAddress = dns_server; }

void setRetransmissionTimeout(struct EthernetClass *ethernetClass, uint16_t milliseconds)
{
	if (milliseconds > 6553) milliseconds = 6553;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetRetransmissionTime(w5100Class, milliseconds * 10);
	spiClassEndTransaction(spiClass);
}

void setRetransmissionCount(struct EthernetClass *ethernetClass, uint8_t num)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassSetRetransmissionCount(w5100Class, num);
	spiClassEndTransaction(spiClass);
}

void ethernetClient_init(struct EthernetClient *ethernetClient, uint8_t s) { ethernetClient -> sockindex = MAX_SOCK_NUM; ethernetClient -> _timeout = s;}
