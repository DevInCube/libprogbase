#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <progbase.h>
#include <progbase/net.h>
#include <pbconsole.h>

#define BUFFER_LEN 1024

int main(void) {
	TcpClient * client = TcpClient_init(&(TcpClient){});
    IpAddress * serverAddress = IpAddress_init(&(IpAddress){}, "127.0.0.1", 3000);
    NetMessage * message = NetMessage_init(
        &(NetMessage){},  // value on stack
        (char[BUFFER_LEN]){},  // array on stack 
        BUFFER_LEN); 
    
    if (!TcpClient_connect(client, serverAddress)) {
        perror("tcp connect");
        return 1;
    }
	sleepMillis(3000);
    //
    // setup message object
    NetMessage_setDataString(message, "Hello, TCP Server!");
    //
    // send string to server
    printf("Send `%s` to server %s:%d\n",
        message->buffer, 
        IpAddress_address(serverAddress),
        IpAddress_port(serverAddress));
    if(!TcpClient_send(client, message)) {
		perror("send");
		return 1;
	}
    //
    // receive response from server
    if(!TcpClient_receive(client, message)) {
		perror("recv");
		return 1;
	}
    printf("Response received from server (%d bytes): %s\r\n", 
        message->dataLength,
        message->buffer);
    TcpClient_close(client);
	return 0;
}
