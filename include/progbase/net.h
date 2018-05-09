/** @file
    @brief Networking module
*/

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __linux__
	#include <netinet/in.h>
	struct IpAddress {
		struct sockaddr_in addr;
	};
#else // for windows
	struct IpAddress {
		int _;
	}
#endif

/* IP Protocol */

bool Ip_resolveHostname(char * ipv4, const char * hostname);

typedef struct IpAddress IpAddress;

IpAddress * IpAddress_init(IpAddress * self, const char * ipv4, int port);
IpAddress * IpAddress_initAny(IpAddress * self, int port);
const char * IpAddress_address(IpAddress * self);
int IpAddress_port(IpAddress * self);
void IpAddress_setAddress(IpAddress * self, const char * ipv4);
void IpAddress_setPort(IpAddress * self, int port);

/* UDP Protocol */
typedef struct NetMessage NetMessage;
struct NetMessage {
	char * buffer;  /**< buffer to store message data */
	size_t bufferLength;  /**< maximum length of data buffer */
	int dataLength;  /**< length of actual data in buffer */
	int sentDataLength;  /** length of data actually sent to receiver */
};

NetMessage * NetMessage_init(NetMessage * self, char * buf, size_t bufLen);
NetMessage * NetMessage_setData(NetMessage * self, const char * str, size_t bufLen);
NetMessage * NetMessage_setDataString(NetMessage * self, const char * str);
const char * NetMessage_data(NetMessage * self);
int NetMessage_dataLength(NetMessage * self);

typedef struct UdpClient UdpClient;
struct UdpClient {
	IpAddress address;  /**< server IP-address */
	int socket;  /**< private server socket handle */
};

UdpClient * UdpClient_init(UdpClient * self);
UdpClient * UdpClient_initBind(UdpClient * self, int port);
bool UdpClient_bind(UdpClient * self, IpAddress * address);
void UdpClient_close(UdpClient * self);
bool UdpClient_receiveFrom(UdpClient * self, NetMessage * message, IpAddress * address);
bool UdpClient_sendTo(UdpClient * self, NetMessage * message, IpAddress * address);
IpAddress * UdpClient_address(UdpClient * self);

/* TCP Protocol */

typedef struct TcpListener TcpListener;
typedef struct TcpClient TcpClient;
typedef struct Ssl Ssl;

typedef struct __Ssl_priv __Ssl_priv;
struct Ssl {
    __Ssl_priv * __priv;
};

Ssl * Ssl_init(Ssl * self);
void Ssl_cleanup(Ssl * self);

struct TcpListener {
	IpAddress address;  /**< server IP-address */
	int socket;  /**< private server socket handle */
};

TcpListener * TcpListener_init(TcpListener * self);
bool TcpListener_bind(TcpListener * self, IpAddress * address);
bool TcpListener_start(TcpListener * self);
void TcpListener_close(TcpListener * self);
TcpClient * TcpListener_accept(TcpListener * self, TcpClient * client);
IpAddress * TcpListener_address(TcpListener * self);

struct TcpClient {
	IpAddress address;  /**< server IP-address */
	int socket;
	Ssl * ssl;
};

TcpClient * TcpClient_init(TcpClient * self);
bool TcpClient_connect(TcpClient * self, IpAddress * serverAddress);
bool TcpClient_connectSecure(TcpClient * self, IpAddress * serverAddress, Ssl * ssl);
void TcpClient_close(TcpClient * self);
bool TcpClient_receive(TcpClient * self, NetMessage * message);
bool TcpClient_send(TcpClient * self, NetMessage * message);
IpAddress * TcpClient_address(TcpClient * self);

#ifdef __cplusplus
}
#endif
