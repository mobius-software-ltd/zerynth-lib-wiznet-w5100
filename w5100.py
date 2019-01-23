"""
.. module:: w5100
*********************
W5100 Ethernet Module
*********************
This module implements the Zerynth driver for the Wiznet W5100 Ethernet (`Resources and Documentation <https://www.wiznet.io/product-item/w5100/>`_).
This module supports SSL/TLS
"""

"""
@native_c("_w5100_init",["csrc/w5100_ifc.c",
                         "csrc/lib/Ethernet/Ethernet.c",
                         "csrc/lib/Ethernet/EthernetUdp.c",
                         "csrc/lib/Ethernet/EthernetClient.c",
                         "csrc/lib/Ethernet/Dhcp.c",
                         "csrc/lib/Ethernet/Dns.c",
                         "csrc/lib/Ethernet/socket.c",
                         "csrc/lib/Ethernet/utility/w5100.c",
                         "csrc/lib/cores/Client.c",
                         "csrc/lib/cores/IPAddress.c",
                         "csrc/lib/cores/Stream.c",
                         "csrc/lib/cores/Udp.c",
                         "csrc/lib/libraries/variant.c",
                         "csrc/lib/libraries/SPI.c"]
                         ,[],[
                         "-I.../csrc/lib/Ethernet/",
                         "-I.../csrc/lib/cores/",
                         "-I.../csrc/lib/libraries/"
                         ])

@native_c("_w5100_init",["csrc/w5100_ifc.c",
                         "csrc/lib/Ethernet/*",
                         "csrc/lib/cores/*",
                         "csrc/lib/libraries/*"]
                         ,[],[
                         "-I.../csrc/lib/Ethernet/",
                         "-I.../csrc/lib/cores/",
                         "-I.../csrc/lib/libraries/",
                         "-I"
                         ])
"""
@native_c("_w5100_init",["csrc/w5100_ifc.c"],["DHCP","MAC={0x00,0x08,0xdc,0x78,0x91,0x71}"],[])
def _hwinit():
    pass

def auto_init():
    init()

def init():
    _hwinit()
    __builtins__.__default_net["eth"] = __module__
    __builtins__.__default_net["sock"][0] = __module__ #AF_INET

@native_c("w5100_eth_link",[],[])
def link():
    pass

@native_c("w5100_eth_is_linked",[],[])
def is_linked():
    pass

@native_c("w5100_eth_unlink",["csrc/*"])
def unlink():
    pass

@native_c("w5100_net_link_info",[])
def link_info():
    pass
"""
@native_c("w5100_net_set_link_info",[])
def set_link_info(ip,mask,gw,dns):
    pass

@native_c("w5100_net_resolve",["csrc/*"])
def gethostbyname(hostname):
    pass

@native_c("w5100_net_socket",["csrc/*"])
def socket(protocol,port,flag):
    pass

@native_c("w5100_net_setsockopt",["csrc/*"])
def setsockopt(sock,sotype,optname,value):
    pass

@native_c("w5100_net_close",["csrc/*"])
def close(sock):
    pass

@native_c("w5100_net_sendto",["csrc/*"])
def sendto(sock,buf,addr,port):
    pass

@native_c("w5100_net_send",["csrc/*"])
def send(sock,buf):
    pass

@native_c("w5100_net_send_all",["csrc/*"])
def sendall(sock,buf):
    pass

@native_c("w5100_net_recv_into",["csrc/*"])
def recv_into(sock,buf,bufsize,ofs=0):
    pass

@native_c("w5100_net_recvfrom_into",["csrc/*"])
def recvfrom_into(sock,buf,bufsize,addr,port,ofs=0):
    pass

@native_c("w5100_net_listen",["csrc/*"])
def listen(sock):
    pass

@native_c("w5100_net_connect",["csrc/*"])
def connect(sock, addr, port):
    pass

@native_c("w5100_net_bind",["csrc/*"])
def bind(sock,addr):
    pass

@native_c("w5100_net_accept",["csrc/*"])
def accept(sock):
    pass
"""
