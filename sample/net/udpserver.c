#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <progbase/net.h>

#define BUFFER_LEN 1024

int main(void) {
	//
    // create UDP server
    UdpClient * server = UdpClient_init(&(UdpClient){});
    IpAddress * address = IpAddress_initAny(&(IpAddress){}, 9999);
    if (!UdpClient_bind(server, address)) {
        perror("Can't start server");
        return 1;
    }
    printf("Udp server started on port %d\n", 
        IpAddress_port(UdpClient_address(server)));
    
    NetMessage * message = NetMessage_init(
        &(NetMessage){},  // value on stack
        (char[BUFFER_LEN]){},  // array on stack 
        BUFFER_LEN);

    IpAddress clientAddress;
    while (1) {
        puts("Waiting for data...");
        //
        // blocking call to receive data
        // if someone send it
        if(!UdpClient_receiveFrom(server, message, &clientAddress)) {
			perror("recv");
			return 1;
		}

        printf("Received message from %s:%d (%d bytes): `%s`\n",
            IpAddress_address(&clientAddress),  // client IP-address
            IpAddress_port(&clientAddress),  // client port
            NetMessage_dataLength(message),
            NetMessage_data(message));

        //
        // send echo response
        if (!UdpClient_sendTo(server, message, &clientAddress)) {
			perror("send");
			return 1;
		}
    }
    //
    // close server
    UdpClient_close(server);
	return 0;
}
