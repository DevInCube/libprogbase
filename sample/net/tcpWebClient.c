#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <progbase.h>
#include <progbase/net.h>
#include <progbase/console.h>

#define BUFFER_LEN 10000

int main(void) {
	TcpClient * client = TcpClient_init(&(TcpClient){});
    const char * serverHostname = "progbase.herokuapp.com";
    char ipv4[20] = "";
    if (!Ip_resolveHostname(ipv4, serverHostname)) {
        perror("resolve address");
        return 1;
    }
    IpAddress * serverAddress = IpAddress_init(
        &(IpAddress){}, 
        ipv4, 
        80);  // default port for web servers
    NetMessage * message = NetMessage_init(
        &(NetMessage){},  // value on stack
        (char[BUFFER_LEN]){},  // array on stack 
        BUFFER_LEN); 
    
    if (!TcpClient_connect(client, serverAddress)) {
        perror("tcp connect");
        return 1;
    }
    //
    // setup message object
    char httpWebRequest[100] = "";
    sprintf(httpWebRequest, 
        "GET / HTTP/1.0\r\n"
        "Host: %s\r\n"
        "\r\n", serverHostname);
    NetMessage_setDataString(message, httpWebRequest);
    //
    // send string to server
    printf(">> Send message to server %s:%d (%s):\r\n%s\r\n",
        IpAddress_address(serverAddress),
        IpAddress_port(serverAddress),
        serverHostname,
        message->buffer);
    if(!TcpClient_send(client, message)) {
		perror("send");
		return 1;
	}
    //
    // receive response from server
    // use loop to receive big data buffers
    // the end of message if determined by 0 data length
    while (1) {
        if(!TcpClient_receive(client, message)) {
            perror("recv");
            return 1;
        }
        if (NetMessage_dataLength(message) == 0) {
            // no more data to receive from server
            break;
        }
        printf(">> Response received from server (%d bytes):\r\n%s\r\n", 
            message->dataLength,
            message->buffer);
    }
    
    TcpClient_close(client);
	return 0;
}
