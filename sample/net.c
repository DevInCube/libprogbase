#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>

#include <progbase/net.h>

#define BUFFER_LEN 1024

int udpServerSample() {
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
        UdpClient_receiveFrom(server, message, &clientAddress);

        printf("Received message from %s:%d (%d bytes): `%s`\n",
            IpAddress_address(&clientAddress),  // client IP-address
            IpAddress_port(&clientAddress),  // client port
            NetMessage_dataLength(message),
            NetMessage_data(message));

        //
        // send echo response
        UdpClient_sendTo(server, message, &clientAddress);
    }
    //
    // close server
    UdpClient_close(server);
}

int udpClientSample() {
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
    UdpClient_sendTo(client, message, serverAddress);
    //
    // receive response from server
    UdpClient_receiveFrom(client, message, serverAddress);
    printf("Response received from server (%d bytes): %s\r\n", 
        NetMessage_dataLength(message),
        NetMessage_data(message));
    //
    // close client object
    UdpClient_close(client);
    return 0;
}

int tcpServerSample() {
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
    printf("TCP Server is listening on port %d\n", 
        IpAddress_port(TcpListener_address(server)));
    
    NetMessage * message = NetMessage_init(
        &(NetMessage){},  // value on stack
        (char[BUFFER_LEN]){},  // array on stack 
        BUFFER_LEN);
    // to store information about current client
    TcpClient client;
    while (1) {
        // wait for someone to connect to server
        TcpListener_accept(server, &client);
        // wait for data from client
        TcpClient_receive(&client, message);
        IpAddress * clientAddress = TcpClient_address(&client);
        printf("Received message from %s:%d (%d bytes): `%s`\n",
            IpAddress_address(clientAddress),  // client IP-address
            IpAddress_port(clientAddress),  // client port
            NetMessage_dataLength(message),
            NetMessage_data(message));
        // echo send data back
        TcpClient_send(&client, message);
        // close tcp connection
        TcpClient_close(&client);
    }
    // close listener
    TcpListener_close(server);
    return 0;
}

int tcpClientSample() {
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
    //
    // setup message object
    NetMessage_setDataString(message, "Hello, TCP Server!");
    //
    // send string to server
    printf("Send `%s` to server %s:%d\n",
        message->buffer, 
        IpAddress_address(serverAddress),
        IpAddress_port(serverAddress));
    TcpClient_send(client, message);
    //
    // receive response from server
    TcpClient_receive(client, message);
    printf("Response received from server (%d bytes): %s\r\n", 
        message->dataLength,
        message->buffer);
    TcpClient_close(client);
    return 0;
}

int main(void) {
    //udpServerSample();
    //udpClientSample();
    //tcpServerSample();
    tcpClientSample();
    return 0;
}
