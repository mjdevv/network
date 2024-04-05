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

int main()
{
    // sockfd: file descriptor for the socket on which the server will listen for connections
    // newsockfd: file descriptor for the socket that the server will use to communicate with a client
    int sockfd, newsockfd;

    // socklen_t: data type used for socket length
    socklen_t clilen;

    struct sockaddr_in serv_addr, cli_addr; // sockaddr_in: structure containing an internet address
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
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // bind the socket to an address
    // sockfd : socket file descriptor
    int sock_bind = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))

        if (sock_bind < 0)
            error("ERROR on binding");

    // listen for connections on a socket
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        // Create a new process for each connection
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)
        {
            // This is the child process
            close(sockfd);
            handle_client(newsockfd);
            exit(0);
        }
        else
        {
            // This is the parent process
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}
