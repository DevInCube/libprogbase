#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#ifndef SSL_MISSING
	#include <openssl/ssl.h>
#endif

#include <progbase/net.h>

#define FATAL(MSG) { assert(0 && MSG); fprintf(stderr, "%s", MSG); }

static struct sockaddr_in __ip_init(in_addr_t address, int port) {
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;            /* Internet Domain    */
	addr.sin_port        = htons(port);        /* Server Port        */
	addr.sin_addr.s_addr = address;  		   /* Server's Address   */
	return addr;
}

IpAddress * IpAddress_init(IpAddress * self, const char * ipv4, int port) {
	self->addr = __ip_init(inet_addr(ipv4), port);
	return self;
}

IpAddress * IpAddress_initAny(IpAddress * self, int port) {
	self->addr = __ip_init(INADDR_ANY, port);
	return self;
}

// adapted from https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
bool Ip_resolveHostname(char * ipv4, const char * hostname) {
	struct hostent * he = gethostbyname(hostname);
    if (NULL == he) { return false; }

    struct in_addr ** addr_list = (struct in_addr **) he->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
		strcpy(ipv4, inet_ntoa(*addr_list[i]));
        return true;
    }
    return false;
}

const char * IpAddress_address(IpAddress * self) {
	return inet_ntoa(self->addr.sin_addr);
}

void IpAddress_setAddress(IpAddress * self, const char * ipv4) {
	self->addr.sin_addr.s_addr = inet_addr(ipv4);
}

int IpAddress_port(IpAddress * self) {
	return ntohs(self->addr.sin_port);
}

void IpAddress_setPort(IpAddress * self, int port) {
	self->addr.sin_port = htons(port);
}

// UDP

UdpClient * UdpClient_init(UdpClient * self) {
	self->socket = -1;
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
		return self;
    }
	self->socket = sock;
	return self;
}

bool UdpClient_bind(UdpClient * self, IpAddress * address) {
	self->address = *address;
    return bind(
		self->socket,
		(struct sockaddr*)&(self->address.addr),
		sizeof(self->address.addr)) >= 0;
}

bool UdpClient_receiveFrom(UdpClient * self, NetMessage * message, IpAddress * address) {
	struct sockaddr_in addr;  // to get sender address
	socklen_t addrlen = sizeof(addr);
	int bytes = recvfrom(
		self->socket,
		message->buffer,
		message->bufferLength,  // maximum bytes to receive in buffer
		0,  // control flags
		(struct sockaddr*)&addr,
		&addrlen);
	if (bytes <= message->bufferLength) {
		message->buffer[bytes] = '\0';  // terminate c-string
	}
	if (address != NULL) {
		address->addr = addr;
	}
	message->dataLength = bytes;
	message->sentDataLength = 0;
	return bytes >= 0;
}

bool UdpClient_sendTo(UdpClient * self, NetMessage * message, IpAddress * address) {
	struct sockaddr_in addr = address->addr;
	socklen_t addrlen = sizeof(addr);
	int bytes = sendto(
		self->socket,
		message->buffer,
		message->dataLength,
		0,  // control flags
		(struct sockaddr*)&addr,
		addrlen);
	message->sentDataLength = bytes;
	return bytes >= 0;
}

void UdpClient_close(UdpClient * self) {
	close(self->socket);
}

IpAddress * UdpClient_address(UdpClient * self) {
	return &self->address;
}

NetMessage * NetMessage_init(NetMessage * self, char * buf, size_t bufLen) {
	memset(self, 0, sizeof(*self));
    self->buffer = buf;
    self->bufferLength = bufLen - 1;  // to store null-symbol
	return self;
}

NetMessage * NetMessage_setData(NetMessage * self, const char * data, size_t dataLen) {
	if (self->bufferLength < dataLen) {
		FATAL("Input data is longer than message buffer");
		return NULL;
	}
	memcpy(self->buffer, data, dataLen);
	self->dataLength = dataLen;
	return self;
}

NetMessage * NetMessage_setDataString(NetMessage * self, const char * str) {
	int dataLength = strlen(str) + 1;  // + null-termination
	if (dataLength >= self->bufferLength) {
		FATAL("Buffer overflow error");
		return NULL;
	}
	strcpy(self->buffer, str);
	self->dataLength = dataLength;
	return self;
}

const char * NetMessage_data(NetMessage * self) {
	return self->buffer;
}

int NetMessage_dataLength(NetMessage * self) {
	return self->dataLength;
}

// TCP

static bool TcpListener_listen(TcpListener * self, int queueMaxSize);

TcpListener * TcpListener_init(TcpListener * self) {
	self->socket = -1;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
		FATAL("Can't create socket");
		return NULL;
    }
	self->socket = sock;
	return self;
}

bool TcpListener_start(TcpListener * self) {
	return TcpListener_listen(self, 10);
}

bool TcpListener_bind(TcpListener * self, IpAddress * address) {
	self->address = *address;
    return bind(
		self->socket,
		(struct sockaddr*)&(self->address.addr),
		sizeof(self->address.addr)) >= 0;
}

bool TcpListener_listen(TcpListener * self, int queueMaxSize) {
	return listen(self->socket, queueMaxSize) >= 0;
}

void TcpListener_close(TcpListener * self) {
	close(self->socket);
}

static TcpClient * __TcpClient_initSocket(TcpClient * self, int socket);

TcpClient * TcpListener_accept(TcpListener * self, TcpClient * client) {
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int sock = accept(
		self->socket,
		(struct sockaddr *)&addr,
		&addrlen);
	if (sock < 0) {
		FATAL("Accept client error");
		return NULL;
	}
	__TcpClient_initSocket(client, sock);
	client->address.addr = addr;
	return client;
}

IpAddress * TcpListener_address(TcpListener * self) {
	return &self->address;
}

// Tcp client

static TcpClient * __TcpClient_initSocket(TcpClient * self, int socket) {
	IpAddress_init(&self->address, "127.0.0.1", 0);
	self->socket = socket;
	self->ssl = NULL;
	return self;
}

TcpClient * TcpClient_init(TcpClient * self) {
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
		FATAL("Can't create socket");
		return NULL;
    }
	return __TcpClient_initSocket(self, sock);
}

struct __Ssl_priv {
#ifndef SSL_MISSING
	SSL_CTX * ssl_ctx;
    SSL * conn;
#else
	int _void;
#endif
};

bool Ssl_connect(Ssl * self, TcpClient * client);
bool Ssl_send(Ssl * self, NetMessage * message);
bool Ssl_receive(Ssl * self, NetMessage * message);
void Ssl_close(Ssl * self);

bool TcpClient_connect(TcpClient * self, IpAddress * serverAddress) {
	self->ssl = NULL;
	int addrlen = sizeof(serverAddress->addr);
	bool status = connect(
		self->socket,
		(struct sockaddr*)&serverAddress->addr,
		addrlen) >= 0;
	if (status && self->ssl != NULL) {
		return Ssl_connect(self->ssl, self);
	}
	return status;
}

bool TcpClient_connectSecure(TcpClient * self, IpAddress * serverAddress, Ssl * ssl) {
	self->ssl = ssl;
	int addrlen = sizeof(serverAddress->addr);
	bool status = connect(
		self->socket,
		(struct sockaddr*)&serverAddress->addr,
		addrlen) >= 0;
	if (status && self->ssl != NULL) {
		return Ssl_connect(self->ssl, self);
	}
	return status;
}

void TcpClient_close(TcpClient * self) {
	if (self->ssl != NULL) {
		Ssl_close(self->ssl);
	}
	close(self->socket);
}

bool TcpClient_receive(TcpClient * self, NetMessage * message) {
	int bytes = -1;
	if (self->ssl != NULL) {
		return Ssl_receive(self->ssl, message);
	} else {
		bytes = read(self->socket, message->buffer, message->bufferLength);
		if (bytes <= message->bufferLength) {
			message->buffer[bytes] = '\0';  // terminate c-string
		}
		message->dataLength = bytes;
		message->sentDataLength = 0;
	}
	return bytes >= 0;
}

bool TcpClient_send(TcpClient * self, NetMessage * message) {
	int bytes = -1;
	if (self->ssl != NULL) {
		return Ssl_send(self->ssl, message);
	} else {
		bytes = (int)send(
			self->socket,
			message->buffer,
			message->dataLength,
			0);
		message->sentDataLength = bytes;
	}
	return bytes >= 0;
}

IpAddress * TcpClient_address(TcpClient * self) {
	return &self->address;
}

// SSL Protocol

Ssl * Ssl_init(Ssl * self) {
#ifndef SSL_MISSING
	self->__priv = malloc(sizeof(__Ssl_priv));
	if (self->__priv == NULL) {
		FATAL("Ssl init priv error");
		return NULL;
	}
    self->__priv->conn = NULL;
    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    self->__priv->ssl_ctx = ssl_ctx;
#endif
    return self;
}

bool Ssl_connect(Ssl * self, TcpClient * client) {
#ifndef SSL_MISSING
    //
    // create an SSL connection and attach it to the socket
    self->__priv->conn = SSL_new(self->__priv->ssl_ctx);
	if (NULL == self->__priv->conn) {
		return false;
	}
	//
	// bind ssl for client socket
    if(SSL_set_fd(self->__priv->conn, client->socket) < 0) {
		return false;
    }
    //
    // perform the SSL/TLS handshake with the server - when on the
    // server side, this would use SSL_accept()
    return SSL_connect(self->__priv->conn);
#else
	return false;
#endif
}

bool Ssl_send(Ssl * self, NetMessage * message) {
#ifndef SSL_MISSING
    int bytes = SSL_write(self->__priv->conn, (void *)message->buffer, message->dataLength);
    message->sentDataLength = bytes;
    return bytes >= 0;
#else
	errno = EPERM;
	return false;
#endif
}

bool Ssl_receive(Ssl * self, NetMessage * message) {
#ifndef SSL_MISSING
    int bytes = SSL_read(self->__priv->conn, message->buffer, message->bufferLength);
    if (bytes < message->bufferLength) {
        message->buffer[bytes] = '\0';  // null-terminate string
    }
    message->dataLength = bytes;
    return bytes >= 0;
#else
	errno = EPERM;
	return false;
#endif
}

void Ssl_close(Ssl * self) {
#ifndef SSL_MISSING
    SSL_shutdown(self->__priv->conn);
    SSL_free(self->__priv->conn);
#endif
}

void Ssl_cleanup(Ssl * self) {
#ifndef SSL_MISSING
    SSL_CTX_free(self->__priv->ssl_ctx);
	free(self->__priv);
#endif
}

