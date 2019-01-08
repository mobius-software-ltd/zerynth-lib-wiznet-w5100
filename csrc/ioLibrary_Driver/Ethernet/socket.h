//*****************************************************************************
//
//! \file socket.h
//! \brief SOCKET APIs Header file.
//! \details SOCKET APIs like as berkeley socket api.
//! \version 1.0.2
//! \date 2013/10/21
//! \par  Revision history
//!       <2015/02/05> Notice
//!        The version history is not updated after this point.
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//!       <2014/05/01> V1.0.2. Refer to M20140501
//!         1. Modify the comment : SO_REMAINED -> PACK_REMAINED
//!         2. Add the comment as zero byte udp data reception in getsockopt().
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
/**
 * @defgroup WIZnet_socket_APIs 1. WIZnet socket APIs
 * @brief WIZnet socket APIs are based on Berkeley socket APIs,  thus it has much similar name and interface.
 *        But there is a little bit of difference.
 * @details
 * <b> Comparison between WIZnet and Berkeley SOCKET APIs </b>
 * <table>
 *    <tr>   <td><b>API</b></td> <td><b>WIZnet</b></td> <td><b>Berkeley</b></td>   </tr>
 *    <tr>   <td>socket()</td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>bind()</b></td> <td>X</td> <td>O</td>   </tr>
 *    <tr>   <td><b>listen()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>connect()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>accept()</b></td> <td>X</td> <td>O</td>   </tr>
 *    <tr>   <td><b>recv()</b></td> <td>O</td> <td>O</td>    </tr>
 *    <tr>   <td><b>send()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>recvfrom()</b></td> <td>O</td> <td>O</td>   </tr>
 *    <tr>   <td><b>sendto()</b></td> <td>O</td> <td>O</td>    </tr>
 *    <tr>   <td><b>closesocket()</b></td> <td>O<br>close() & disconnect()</td> <td>O</td>   </tr>
 * </table>
 * There are @b bind() and @b accept() functions in @b Berkeley SOCKET API but,
 * not in @b WIZnet SOCKET API. Because socket() of WIZnet is not only creating a SOCKET but also binding a local port number,
 * and listen() of WIZnet is not only listening to connection request from client but also accepting the connection request. \n
 * When you program "TCP SERVER" with Berkeley SOCKET API, you can use only one listen port.
 * When the listen SOCKET accepts a connection request from a client, it keeps listening.
 * After accepting the connection request, a new SOCKET is created and the new SOCKET is used in communication with the client. \n
 * Following figure shows network flow diagram by Berkeley SOCKET API.
 * @image html Berkeley_SOCKET.jpg "<Berkeley SOCKET API>"
 * But, When you program "TCP SERVER" with WIZnet SOCKET API, you can use as many as 8 listen SOCKET with same port number. \n
 * Because there's no accept() in WIZnet SOCKET APIs, when the listen SOCKET accepts a connection request from a client,
 * it is changed in order to communicate with the client.
 * And the changed SOCKET is not listening any more and is dedicated for communicating with the client. \n
 * If there're many listen SOCKET with same listen port number and a client requests a connection,
 * the SOCKET which has the smallest SOCKET number accepts the request and is changed as communication SOCKET. \n
 * Following figure shows network flow diagram by WIZnet SOCKET API.
 * @image html WIZnet_SOCKET.jpg "<WIZnet SOCKET API>"
 */
#ifndef _SOCKET_H_
#define _SOCKET_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "wizchip_conf.h"

#define SOCKET                uint8_t
#define SOCK_OK               1
#define SOCK_BUSY             0
#define SOCK_FATAL            -1000
#define SOCK_ERROR            0
#define SOCKERR_SOCKNUM       (SOCK_ERROR - 1)
#define SOCKERR_SOCKOPT       (SOCK_ERROR - 2)
#define SOCKERR_SOCKINIT      (SOCK_ERROR - 3)
#define SOCKERR_SOCKCLOSED    (SOCK_ERROR - 4)
#define SOCKERR_SOCKMODE      (SOCK_ERROR - 5)
#define SOCKERR_SOCKFLAG      (SOCK_ERROR - 6)
#define SOCKERR_SOCKSTATUS    (SOCK_ERROR - 7)
#define SOCKERR_ARG           (SOCK_ERROR - 10)
#define SOCKERR_PORTZERO      (SOCK_ERROR - 11)
#define SOCKERR_IPINVALID     (SOCK_ERROR - 12)
#define SOCKERR_TIMEOUT       (SOCK_ERROR - 13)
#define SOCKERR_DATALEN       (SOCK_ERROR - 14)
#define SOCKERR_BUFFER        (SOCK_ERROR - 15)
#define SOCKFATAL_PACKLEN     (SOCK_FATAL - 1)

#define SF_ETHER_OWN           (Sn_MR_MFEN)
#define SF_IGMP_VER2           (Sn_MR_MC)
#define SF_TCP_NODELAY         (Sn_MR_ND)
#define SF_MULTI_ENABLE        (Sn_MR_MULTI)

#define SF_IO_NONBLOCK           0x01
#define PACK_FIRST               0x80
#define PACK_REMAINED            0x01
#define PACK_COMPLETED           0x00

int8_t  socket(uint8_t sn, uint8_t protocol, uint16_t port, uint8_t flag);

int8_t  close(uint8_t sn);

int8_t  listen(uint8_t sn);

int8_t  connect(uint8_t sn, uint8_t * addr, uint16_t port);

int8_t  disconnect(uint8_t sn);

int32_t send(uint8_t sn, uint8_t * buf, uint16_t len);

int32_t recv(uint8_t sn, uint8_t * buf, uint16_t len);

int32_t sendto(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port);

int32_t recvfrom(uint8_t sn, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t *port);

/////////////////////////////
// SOCKET CONTROL & OPTION //
/////////////////////////////
#define SOCK_IO_BLOCK         0
#define SOCK_IO_NONBLOCK      1

typedef enum
{
   SIK_CONNECTED     = (1 << 0),
   SIK_DISCONNECTED  = (1 << 1),
   SIK_RECEIVED      = (1 << 2),
   SIK_TIMEOUT       = (1 << 3),
   SIK_SENT          = (1 << 4),
   SIK_ALL           = 0x1F
}sockint_kind;

typedef enum
{
   CS_SET_IOMODE,
   CS_GET_IOMODE,
   CS_GET_MAXTXBUF,
   CS_GET_MAXRXBUF,
   CS_CLR_INTERRUPT,
   CS_GET_INTERRUPT,
}ctlsock_type;

typedef enum
{
   SO_FLAG,
   SO_TTL,
   SO_TOS,
   SO_MSS,
   SO_DESTIP,
   SO_DESTPORT,
   SO_SENDBUF,
   SO_RECVBUF,
   SO_STATUS,
   SO_REMAINSIZE,
   SO_PACKINFO
}sockopt_type;

int8_t  ctlsocket(uint8_t sn, ctlsock_type cstype, void* arg);

int8_t  setsockopt(uint8_t sn, sockopt_type sotype, void* arg);

int8_t  getsockopt(uint8_t sn, sockopt_type sotype, void* arg);

#ifdef __cplusplus
 }
#endif

#endif // _SOCKET_H_
