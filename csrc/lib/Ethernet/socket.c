#include "socket.h"

#if ARDUINO >= 156 && !defined(ARDUINO_ARCH_PIC32)
extern void yield(void);
#else
#define yield()
#endif

// TODO: randomize this when not using DHCP, but how?
uint16_t local_port = 49152;  // 49152 to 65535

typedef struct {
	uint16_t RX_RSR; // Number of bytes received
	uint16_t RX_RD;  // Address to read
	uint16_t TX_FSR; // Free space ready for transmit
	uint8_t  RX_inc; // how much have we advanced RX_RD
} socketstate_t;

socketstate_t state[MAX_SOCK_NUM];
uint16_t getSnTX_FSR(uint8_t s);
uint16_t getSnRX_RSR(uint8_t s);
void write_data(uint8_t s, uint16_t offset, const uint8_t *data, uint16_t len);
void read_data(uint8_t s, uint16_t src, uint8_t *dst, uint16_t len);

/*****************************************/
/*          Socket management            */
/*****************************************/
void delayMicroseconds(uint32_t usec){
    if (usec == 0) return;
    uint32_t n = usec * (VARIANT_MCK / 3000000);
    __asm__ volatile(
        "L_%=_delayMicroseconds:"       "\n\t"
        "subs   %0, #1"                 "\n\t"
        "bne    L_%=_delayMicroseconds" "\n"
        : "+r" (n) :
    );
}

void socketPortRand(uint16_t n)
{
	n &= 0x3FFF;
	local_port ^= n;
}

uint8_t socketBegin(uint8_t protocol, uint16_t port)
{
	uint8_t s, status[MAX_SOCK_NUM], chip, maxindex=MAX_SOCK_NUM;

	// first check hardware compatibility
	chip = getChip(w5100Clazz);
	if (!chip) return MAX_SOCK_NUM; // immediate error if no hardware detected
#if MAX_SOCK_NUM > 4
	if (chip == 51) maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
	//Serial.printf("W5000socket begin, protocol=%d, port=%d\n", protocol, port);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	// look at all the hardware sockets, use any that are closed (unused)
	for (s=0; s < maxindex; s++) {
		status[s] = readSnSR(w5100Clazz, s);
		if (status[s] == SnSR_CLOSED) goto makesocket;
	}
	//Serial.printf("W5000socket step2\n");
	// as a last resort, forcibly close any already closing
	for (s=0; s < maxindex; s++) {
		uint8_t stat = status[s];
		if (stat == SnSR_LAST_ACK) goto closemakesocket;
		if (stat == SnSR_TIME_WAIT) goto closemakesocket;
		if (stat == SnSR_FIN_WAIT) goto closemakesocket;
		if (stat == SnSR_CLOSING) goto closemakesocket;
	}
#if 0
	Serial.printf("W5000socket step3\n");
	// next, use any that are effectively closed
	for (s=0; s < MAX_SOCK_NUM; s++) {
		uint8_t stat = status[s];
		// TODO: this also needs to check if no more data
		if (stat == SnSR::CLOSE_WAIT) goto closemakesocket;
	}
#endif
	spiClassEndTransaction(spiClass);
	return MAX_SOCK_NUM; // all sockets are in use
closemakesocket:
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_CLOSE);
makesocket:
	delayMicroseconds(250); // TODO: is this needed??
	writeSnMR(w5100Clazz, s, protocol);
	writeSnIR(w5100Clazz, s, 0xFF);
	if (port > 0) {
		writeSnPORT(w5100Clazz, s, port);
	} else {
		// if don't set the source port, set local_port number.
		if (++local_port < 49152) local_port = 49152;
		writeSnPORT(w5100Clazz, s, local_port);
	}
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_OPEN);
	state[s].RX_RSR = 0;
	state[s].RX_RD  = readSnRX_RD(w5100Clazz, s); // always zero?
	state[s].RX_inc = 0;
	state[s].TX_FSR = 0;
	spiClassEndTransaction(spiClass);
	return s;
}

// multicast version to set fields before open  thd
uint8_t socketBeginMulticast(uint8_t protocol, struct IPAddress * ip, uint16_t port)
{
	uint8_t s, status[MAX_SOCK_NUM], chip, maxindex=MAX_SOCK_NUM;

	// first check hardware compatibility
	chip = getChip(w5100Clazz);
	if (!chip) return MAX_SOCK_NUM; // immediate error if no hardware detected
#if MAX_SOCK_NUM > 4
	if (chip == 51) maxindex = 4; // W5100 chip never supports more than 4 sockets
#endif
	//Serial.printf("W5000socket begin, protocol=%d, port=%d\n", protocol, port);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	// look at all the hardware sockets, use any that are closed (unused)
	for (s=0; s < maxindex; s++) {
		status[s] = readSnSR(w5100Clazz, s);
		if (status[s] == SnSR_CLOSED) goto makesocket;
	}
	//Serial.printf("W5000socket step2\n");
	// as a last resort, forcibly close any already closing
	for (s=0; s < maxindex; s++) {
		uint8_t stat = status[s];
		if (stat == SnSR_LAST_ACK) goto closemakesocket;
		if (stat == SnSR_TIME_WAIT) goto closemakesocket;
		if (stat == SnSR_FIN_WAIT) goto closemakesocket;
		if (stat == SnSR_CLOSING) goto closemakesocket;
	}
#if 0
	Serial.printf("W5000socket step3\n");
	// next, use any that are effectively closed
	for (s=0; s < MAX_SOCK_NUM; s++) {
		uint8_t stat = status[s];
		// TODO: this also needs to check if no more data
		if (stat == SnSR::CLOSE_WAIT) goto closemakesocket;
	}
#endif
	spiClassEndTransaction(spiClass);
	return MAX_SOCK_NUM; // all sockets are in use
closemakesocket:
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_CLOSE);
makesocket:
	delayMicroseconds(250); // TODO: is this needed??
  writeSnMR(w5100Clazz, s, protocol);
  writeSnIR(w5100Clazz, s, 0xFF);
	if (port > 0) {
		writeSnPORT(w5100Clazz, s, port);
	} else {
		// if don't set the source port, set local_port number.
		if (++local_port < 49152) local_port = 49152;
		writeSnPORT(w5100Clazz, s, local_port);
	}
	// Calculate MAC address from Multicast IP Address
    	uint8_t mac[] = {  0x01, 0x00, 0x5E, 0x00, 0x00, 0x00 };
      /*
    	mac[3] = ip[1] & 0x7F;
    	mac[4] = ip[2];
    	mac[5] = ip[3];
      */
      mac[3] = getOctetIPAddress(ip, 1) & 0x7F;

      uint8_t* addr = raw_address(ip);
    	writeSnDIPR(w5100Clazz, s, addr);   //239.255.0.1
    	writeSnDPORT(w5100Clazz, s, port);
    	writeSnDHAR(w5100Clazz, s, mac);
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_OPEN);
	state[s].RX_RSR = 0;
	state[s].RX_RD  = readSnRX_RD(w5100Clazz, s); // always zero?
	state[s].RX_inc = 0;
	state[s].TX_FSR = 0;
	spiClassEndTransaction(spiClass);
	return s;
}

// Return the socket's status
uint8_t socketStatus(uint8_t s)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint8_t status = readSnSR(w5100Clazz, s);
	spiClassEndTransaction(spiClass);
	return status;
}

// Immediately close.  If a TCP connection is established, the
// remote host is left unaware we closed.
void socketClose(uint8_t s)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
  w5100ClassExecCmdSn(w5100Clazz, s, Sock_CLOSE);
	spiClassEndTransaction(spiClass);
}

// Place the socket in listening (server) mode
uint8_t socketListen(uint8_t s)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	if (readSnSR(w5100Clazz, s) != SnSR_INIT) {
		spiClassEndTransaction(spiClass);
		return 0;
	}
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_LISTEN);
	spiClassEndTransaction(spiClass);
	return 1;
}

// establish a TCP connection in Active (client) mode.
void socketConnect(uint8_t s, uint8_t * addr, uint16_t port)
{
	// set destination IP
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	writeSnDIPR(w5100Clazz, s, addr);
	writeSnDPORT(w5100Clazz, s, port);
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_CONNECT);
	spiClassEndTransaction(spiClass);
}

// Gracefully disconnect a TCP connection.
void socketDisconnect(uint8_t s)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
  w5100ClassExecCmdSn(w5100Clazz, s, Sock_DISCON);
	spiClassEndTransaction(spiClass);
}

/*****************************************/
/*    Socket Data Receive Functions      */
/*****************************************/
uint16_t getSnRX_RSR(uint8_t s)
{
#if 1
        uint16_t val, prev;

        prev = readSnRX_RSR(w5100Clazz, s);
        while (1) {
                val = readSnRX_RSR(w5100Clazz, s);
                if (val == prev) {
			return val;
		}
                prev = val;
        }
#else
	uint16_t val = W5100.readSnRX_RSR(s);
	return val;
#endif
}

void read_data(uint8_t s, uint16_t src, uint8_t *dst, uint16_t len)
{
	uint16_t size;
	uint16_t src_mask;
	uint16_t src_ptr;

	//Serial.printf("read_data, len=%d, at:%d\n", len, src);
	src_mask = (uint16_t)src & w5100Clazz->SMASK;
	src_ptr = RBASE(w5100Clazz, s) + src_mask;

	if (hasOffsetAddressMapping() || src_mask + len <= w5100Clazz->SSIZE) {
		readBuffer(w5100Clazz, src_ptr, dst, len);
	} else {
		size = w5100Clazz->SSIZE - src_mask;
		readBuffer(w5100Clazz, src_ptr, dst, size);
		dst += size;
		readBuffer(w5100Clazz, RBASE(w5100Clazz, s), dst, len - size);
	}
}

// Receive data.  Returns size, or -1 for no data, or 0 if connection closed
int socketRecv(uint8_t s, uint8_t *buf, int16_t len)
{
	// Check how much data is available
	int ret = state[s].RX_RSR;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	if (ret < len) {
		uint16_t rsr = getSnRX_RSR(s);
		ret = rsr - state[s].RX_inc;
		state[s].RX_RSR = ret;
		//Serial.printf("Sock_RECV, RX_RSR=%d, RX_inc=%d\n", ret, state[s].RX_inc);
	}
	if (ret == 0) {
		// No data available.
		uint8_t status = readSnSR(w5100Clazz, s);
		if ( status == SnSR_LISTEN || status == SnSR_CLOSED ||
		  status == SnSR_CLOSE_WAIT ) {
			// The remote end has closed its side of the connection,
			// so this is the eof state
			ret = 0;
		} else {
			// The connection is still up, but there's no data waiting to be read
			ret = -1;
		}
	} else {
		if (ret > len) ret = len; // more data available than buffer length
		uint16_t ptr = state[s].RX_RD;
		if (buf) read_data(s, ptr, buf, ret);
		ptr += ret;
		state[s].RX_RD = ptr;
		state[s].RX_RSR -= ret;
		uint16_t inc = state[s].RX_inc + ret;
		if (inc >= 250 || state[s].RX_RSR == 0) {
			state[s].RX_inc = 0;
		  writeSnRX_RD(w5100Clazz, s, ptr);
			w5100ClassExecCmdSn(w5100Clazz, s, Sock_RECV);
		} else {
			state[s].RX_inc = inc;
		}
	}
	spiClassEndTransaction(spiClass);
	return ret;
}

uint16_t socketRecvAvailable(uint8_t s)
{
	uint16_t ret = state[s].RX_RSR;
	if (ret == 0) {
		spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
		uint16_t rsr = getSnRX_RSR(s);
		spiClassEndTransaction(spiClass);
		ret = rsr - state[s].RX_inc;
		state[s].RX_RSR = ret;
	}
	return ret;
}

// get the first byte in the receive queue (no checking)
uint8_t socketPeek(uint8_t s)
{
	uint8_t b;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint16_t ptr = state[s].RX_RD;
	readBuffer(w5100Clazz, (ptr & w5100Clazz->SMASK) + RBASE(w5100Clazz, s), &b, 1);
	spiClassEndTransaction(spiClass);
	return b;
}

/*****************************************/
/*    Socket Data Transmit Functions     */
/*****************************************/

uint16_t getSnTX_FSR(uint8_t s)
{
        uint16_t val, prev;

        prev = readSnTX_FSR(w5100Clazz, s);
        while (1) {
                val = readSnTX_FSR(w5100Clazz, s);
                if (val == prev) {
			state[s].TX_FSR = val;
			return val;
		}
                prev = val;
        }
}

void write_data(uint8_t s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
	uint16_t ptr = readSnTX_WR(w5100Clazz, s);
	ptr += data_offset;
	uint16_t offset = ptr & w5100Clazz->SMASK;
	uint16_t dstAddr = offset + SBASE(w5100Clazz, s);

	if (hasOffsetAddressMapping() || offset + len <= w5100Clazz->SSIZE) {
		writeBuffer(w5100Clazz, dstAddr, data, len);
	} else {
		// Wrap around circular buffer
		uint16_t size = w5100Clazz->SSIZE - offset;
		writeBuffer(w5100Clazz, dstAddr, data, size);
		writeBuffer(w5100Clazz, SBASE(w5100Clazz, s), data + size, len - size);
	}
	ptr += len;
	writeSnTX_WR(w5100Clazz, s, ptr);
}

/**
 * @brief	This function used to send the data in TCP mode
 * @return	1 for success else 0.
 */
uint16_t socketSend(uint8_t s, const uint8_t * buf, uint16_t len)
{
	uint8_t status=0;
	uint16_t ret=0;
	uint16_t freesize=0;

	if (len > w5100Clazz->SSIZE) {
		ret = w5100Clazz->SSIZE; // check size not to exceed MAX size.
	} else {
		ret = len;
	}

	do {
		spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
		freesize = getSnTX_FSR(s);
		status = readSnSR(w5100Clazz, s);
		spiClassEndTransaction(spiClass);
		if ((status != SnSR_ESTABLISHED) && (status != SnSR_CLOSE_WAIT)) {
			ret = 0;
			break;
		}
		yield();
	} while (freesize < ret);

	// copy data
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	write_data(s, 0, (uint8_t *)buf, ret);
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_SEND);

	while ((readSnIR(w5100Clazz, s) & SnIR_SEND_OK) != SnIR_SEND_OK ) {
		if (readSnSR(w5100Clazz, s) == SnSR_CLOSED ) {
			spiClassEndTransaction(spiClass);
			return 0;
		}
		spiClassEndTransaction(spiClass);
		yield();
		spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	}

	writeSnIR(w5100Clazz, s, SnIR_SEND_OK);
	spiClassEndTransaction(spiClass);
	return ret;
}

uint16_t socketSendAvailable(uint8_t s)
{
	uint8_t status=0;
	uint16_t freesize=0;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	freesize = getSnTX_FSR(s);
	status = readSnSR(w5100Clazz, s);
	spiClassEndTransaction(spiClass);
	if ((status == SnSR_ESTABLISHED) || (status == SnSR_CLOSE_WAIT)) {
		return freesize;
	}
	return 0;
}

uint16_t socketBufferData(uint8_t s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
	uint16_t ret =0;
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	uint16_t txfree = getSnTX_FSR(s);
	if (len > txfree) {
		ret = txfree; // check size not to exceed MAX size.
	} else {
		ret = len;
	}
	write_data(s, offset, buf, ret);
	spiClassEndTransaction(spiClass);
	return ret;
}

bool socketStartUDP(uint8_t s, uint8_t* addr, uint16_t port)
{
	if ( ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
	  ((port == 0x00)) ) {
		return false;
	}
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	writeSnDIPR(w5100Clazz, s, addr);
	writeSnDPORT(w5100Clazz, s, port);
	spiClassEndTransaction(spiClass);
	return true;
}

bool socketSendUDP(uint8_t s)
{
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	w5100ClassExecCmdSn(w5100Clazz, s, Sock_SEND);

	while ( (readSnIR(w5100Clazz, s) & SnIR_SEND_OK) != SnIR_SEND_OK ) {
		if (readSnIR(w5100Clazz, s) & SnIR_TIMEOUT) {
			writeSnIR(w5100Clazz, s, (SnIR_SEND_OK|SnIR_TIMEOUT));
			spiClassEndTransaction(spiClass);
			return false;
		}
		spiClassEndTransaction(spiClass);
		yield();
		spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);
	}

	writeSnIR(w5100Clazz, s, SnIR_SEND_OK);
	spiClassEndTransaction(spiClass);

	/* Sent ok */
	return true;
}
