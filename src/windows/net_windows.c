#include <progbase\net.h>
#include <stdlib.h>

// dummy implementation

IpAddress * IpAddress_init(IpAddress * self, const char * ipv4, int port) {
	return NULL;
}

IpAddress * IpAddress_initAny(IpAddress * self, int port) {
	return NULL;
}

const char * IpAddress_address(IpAddress * self) {
	return NULL;
}

int IpAddress_port(IpAddress * self) {
	return -255;
}

void IpAddress_setAddress(IpAddress * self, const char * ipv4) {
}

void IpAddress_setPort(IpAddress * self, int port){
}


NetMessage * NetMessage_init(NetMessage * self, char * buf, size_t bufLen) {
	return NULL;
}

NetMessage * NetMessage_setData(NetMessage * self, const char * str, size_t bufLen) {
	return NULL;
}

NetMessage * NetMessage_setDataString(NetMessage * self, const char * str) {
	return NULL;
}

const char * NetMessage_data(NetMessage * self) {
	return NULL;
}

int NetMessage_dataLength(NetMessage * self) {
	return -1;
}


UdpClient * UdpClient_init(UdpClient * self) {
	return NULL;
}

UdpClient * UdpClient_initBind(UdpClient * self, int port) {
	return NULL;
}

bool UdpClient_bind(UdpClient * self, IpAddress * address) {
	return false;
}

void UdpClient_close(UdpClient * self) {
}

bool UdpClient_receiveFrom(UdpClient * self, NetMessage * message, IpAddress * address) {
	return false;
}

bool UdpClient_sendTo(UdpClient * self, NetMessage * message, IpAddress * address) {
	return false;
}

IpAddress * UdpClient_address(UdpClient * self) {
	return NULL;
}


Ssl * Ssl_init(Ssl * self) {
	return NULL;
}
void Ssl_cleanup(Ssl * self) {
}

TcpListener * TcpListener_init(TcpListener * self) {
	return NULL;
}

bool TcpListener_bind(TcpListener * self, IpAddress * address) {
	return false;
}

bool TcpListener_start(TcpListener * self) {
	return false;
}

void TcpListener_close(TcpListener * self) {
}

TcpClient * TcpListener_accept(TcpListener * self, TcpClient * client) {
	return NULL;
}

IpAddress * TcpListener_address(TcpListener * self) {
	return NULL;
}
