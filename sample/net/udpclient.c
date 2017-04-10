#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <progbase/net.h>

#define BUFFER_LEN 1024

int main(void) {
	//
    // initialize all data
    UdpClient * client = UdpClient_init(&(UdpClient){});
    IpAddress * serverAddress = IpAddress_init(
        &(IpAddress){},  // value on stack
        "127.0.0.1",  // server host IP-address (localhost)
        9999);  // server port
    
    NetMessage * message = NetMessage_init(
        &(NetMessage){},  // value on stack
        (char[BUFFER_LEN]){},  // array on stack 
        BUFFER_LEN); 
    //
    // setup message object
    NetMessage_setDataString(message, "Hello, UDP!");
    //
    // send string to server
    printf("Send string `%s` to server %s:%d\n",
        NetMessage_data(message), 
        IpAddress_address(serverAddress),
        IpAddress_port(serverAddress));
    if(!UdpClient_sendTo(client, message, serverAddress)) {
		perror("send");
		return 1;
	}
    //
    // receive response from server
    if(!UdpClient_receiveFrom(client, message, serverAddress)) {
		perror("recv");
		return 1;
	}
    printf("Response received from server (%d bytes): %s\r\n", 
        NetMessage_dataLength(message),
        NetMessage_data(message));
    //
    // close client object
    UdpClient_close(client);
	return 0;
}
