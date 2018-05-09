#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <progbase/net.h>

#define BUFFER_LEN 1024

int main(void) {
	TcpListener * server = TcpListener_init(&(TcpListener){});
    IpAddress * address = IpAddress_initAny(&(IpAddress){}, 3000);
    if(!TcpListener_bind(server, address)) {
        perror("tcp bind");
        return 1;
    }
    if (!TcpListener_start(server)) {
        perror("tcp server start");
        return 1;
    }
    printf(">> TCP Server is listening on port %d\n", 
        IpAddress_port(TcpListener_address(server)));
    
    NetMessage * message = NetMessage_init(
        &(NetMessage){},  // value on stack
        (char[BUFFER_LEN]){},  // array on stack 
        BUFFER_LEN);
    // to store information about current client
    TcpClient client;
    while (1) {
        printf(">> Waiting for a client connections...\n");
        fflush(stdout);
        // wait for someone to connect to server
        if (!TcpListener_accept(server, &client)) {
            perror("accept");
			return 1;
        }
        printf(">> Client connection accepted\n"); 
        fflush(stdout);
        // wait for data from client
        if(!TcpClient_receive(&client, message)) { 
			perror("recv");
			return 1;
		}
        IpAddress * clientAddress = TcpClient_address(&client);
        printf(">> Received message from %s:%d (%d bytes):\n%s\n",
            IpAddress_address(clientAddress),  // client IP-address
            IpAddress_port(clientAddress),  // client port
            NetMessage_dataLength(message),
            NetMessage_data(message));
        fflush(stdout);
        printf(">> Sending message back to the client\n");
        // echo send data back
        if(!TcpClient_send(&client, message)) {
			perror("send");
			return 1;
		}
        printf(">> Closing client connection\n");
        // close tcp connection
        TcpClient_close(&client);
    }
    printf(">> Closing server\n");
    // close listener
    TcpListener_close(server);
	return 0;
}
