#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 

#include <progbase/net.h>

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

bool Ip_resolveHostname(char * ipv4, const char * hostname) {
	struct hostent *he = gethostbyname(hostname);         
    if (NULL == he) return false;
 
    struct in_addr ** addr_list = (struct in_addr **) he->h_addr_list;
    for(int i = 0; addr_list[i] != NULL; i++) 
    {
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
	message->buffer[bytes] = '\0';  // terminate c-string
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
    self->bufferLength = bufLen;
	return self;
}

NetMessage * NetMessage_setData(NetMessage * self, const char * data, size_t dataLen) {
	if (self->bufferLength < dataLen) {
		assert(0 && "Input data is longer than message buffer");
	}
	memcpy(self->buffer, data, dataLen);
	self->dataLength = dataLen;
	return self;
}

NetMessage * NetMessage_setDataString(NetMessage * self, const char * str) {
	int dataLength = strlen(str) + 1;  // + null-termination
	if (dataLength >= self->bufferLength) {
		assert(0 && "Buffer overflow error");
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
		return self;
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

TcpClient * TcpListener_accept(TcpListener * self, TcpClient * client) {
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	client->socket = accept(
		self->socket,
		(struct sockaddr *)&addr, 
		&addrlen);
	if (client->socket < 0) {
		assert(0 && "Accept error");
	}
	client->address.addr = addr;
	return client;
}

IpAddress * TcpListener_address(TcpListener * self) {
	return &self->address;
}

// Tcp client

TcpClient * TcpClient_init(TcpClient * self) {
	self->socket = -1;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
		return NULL;
    }
	self->socket = sock;
	return self;
}

bool TcpClient_connect(TcpClient * self, IpAddress * serverAddress) {
	int addrlen = sizeof(serverAddress->addr);
	return connect(self->socket, (struct sockaddr*)&serverAddress->addr, addrlen) >= 0;
}

void TcpClient_close(TcpClient * self) {
	close(self->socket);
}

bool TcpClient_receive(TcpClient * self, NetMessage * message) {
	int bytes = read(self->socket, message->buffer, message->bufferLength);
	message->buffer[bytes] = '\0';  // terminate c-string
	message->dataLength = bytes;
	message->sentDataLength = 0;
	return bytes >= 0;
}

bool TcpClient_send(TcpClient * self, NetMessage * message) {
	int bytes = (int)send(
		self->socket, 
		message->buffer, 
		message->dataLength,
		0);
	message->sentDataLength = bytes;
	return bytes >= 0;
}

IpAddress * TcpClient_address(TcpClient * self) {
	return &self->address;
}
