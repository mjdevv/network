#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 256

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void handle_client(int sock)
{
    int n;
    char buffer[BUFFER_SIZE];

    bzero(buffer, BUFFER_SIZE);              // bzero : fill buffer with 0
    n = read(sock, buffer, BUFFER_SIZE - 1); // read message from client and store it in buffer

    if (n < 0)
        error("ERROR reading from socket");
    printf("Here is the message: %s\n", buffer);

    // Send a message to the client
    char message[] = "Message Received";
    int message_len = sizeof(message);

    // parameter : socket, message, message length
    // returns : number of bytes sent. when error, returns -1
    n = write(sock, message, message_len);

    if (n < 0) // when error
        error("ERROR writing to socket");

    // close the socket. the connection is closed and file descriptor is deallocated
    close(sock);
}

void tcp_server_process()
{

    // sockfd: file descriptor for the socket on which the server will listen for connections
    // newsockfd: file descriptor for the socket that the server will use to communicate with a client
    int sockfd, newsockfd;

    // socklen_t: data type used for socket length
    socklen_t clilen;

    struct sockaddr_in serv_addr, client_addr; // sockaddr_in: structure containing an internet address
    int pid;

    // Create a new socket.
    // socket is a system call that creates a new socket.
    // parameters : domain, type, and protocol.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    // Initialize serv_addr to zero
    bzero((char *)&serv_addr, sizeof(serv_addr));

    // Set the fields of serv_addr
    serv_addr.sin_family = AF_INET;         // AF_INET : address family for IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY : IP address of the host

    // hton : big-endian network byte order conversion
    serv_addr.sin_port = htons(PORT); // htons : converts port number in host byte order to network byte order

    // bind the socket to an address
    // sockfd : socket file descriptor
    // Attempt to bind the socket to the specified IP address and port
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // listen for connections on a socket : parameter : socket file descriptor, maximum number of connections that can be waiting
    listen(sockfd, 5);
    clilen = sizeof(client_addr);

    // simple logging
    printf("Server started at port %d\n", PORT);
    printf("TCP Server Listening... \n", PORT);

    while (1)
    {
        // server accpets a connection request from a client
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        // Create a new process for each connection
        // 멀티 프로세싱으로 구현한다. 자식 프로세스 포크 뜨기
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        // Child Process
        if (pid == 0)
        {
            // 자식 프로세스는 sockfd를 닫는다. 포크를 뜬 시점에서 부모의 context를 그대로 복사해오는데, sockfd를 닫지 않으면 부모와 같은 소켓을 공유하게 된다.
            // 그리고 자식 프로세스는 클라이언트와 통신하기 위해 newsockfd를 사용하므로 sockfd를 닫아도 된다.
            close(sockfd);
            handle_client(newsockfd);
            exit(0);
        }
        // Parent Process
        else
        {
            // 반대로 부모 프로세스는 자식 프로세스가 사용하는 newsockfd를 가지고 있을 필요가 없음.
            close(newsockfd); // 부모 프로세스는 소켓을 닫는다.
        }
    }

    close(sockfd);
}

int main()
{
    tcp_server_process();
    return 0;
}
