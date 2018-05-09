#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <progbase/net.h>

#define BUFFER_LEN 1024

int main(void) {
	TcpPbListener * server = TcpPbListener_init(&(TcpPbListener){});
    IpAddress * address = IpAddress_initAny(&(IpAddress){}, 3000);
    if(!TcpPbListener_bind(server, address)) {
        perror("tcp bind");
        return 1;
    }
    if (!TcpPbListener_start(server)) {
        perror("tcp server start");
        return 1;
    }
    printf("TCP Server is listening on port %d\n", 
        IpAddress_port(TcpPbListener_address(server)));
    
    NetMessage * message = NetMessage_init(
        &(NetMessage){},  // value on stack
        (char[BUFFER_LEN]){},  // array on stack 
        BUFFER_LEN);
    // to store information about current client
    TcpClient client;
    while (1) {
        // wait for someone to connect to server
        TcpPbListener_accept(server, &client);
        // wait for data from client
        if(!TcpClient_receive(&client, message)) {
			perror("recv");
			return 1;
		}
        IpAddress * clientAddress = TcpClient_address(&client);
        printf("Received message from %s:%d (%d bytes): `%s`\n",
            IpAddress_address(clientAddress),  // client IP-address
            IpAddress_port(clientAddress),  // client port
            NetMessage_dataLength(message),
            NetMessage_data(message));
        // echo send data back
        if(!TcpClient_send(&client, message)) {
			perror("send");
			return 1;
		}
        // close tcp connection
        TcpClient_close(&client);
    }
    // close listener
    TcpPbListener_close(server);
	return 0;
}
