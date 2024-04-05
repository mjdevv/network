#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> // pthread library : 스레드 생성, 조인, 종료 등을 위한 함수들이 정의되어 있다.

#define PORT 12345
#define BUFFER_SIZE 256

/*
    tcp server using thread :
        * 멀티 스레드를 사용해서 클라이언트와 통신하는 서버
        * 서버는 클라이언트와 통신하기 위해 새로운 스레드를 생성한다.
*/

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// void *: generic pointer 형식은 스레드 
void *handle_client(void *sock) {
    int sockfd = *((int *)sock); // socket file descriptor에 dereferncing 접근 
    free(sock); // free the dynamically allocated memory for the socket descriptor

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    ssize_t n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) error("ERROR reading from socket");

    printf("Here is the message: %s\n", buffer);

    char message[] = "Message Received";
    n = write(sockfd, message, sizeof(message));
    if (n < 0) error("ERROR writing to socket");

    close(sockfd);
    return NULL;
}

void main()
{
    // sockfd: file descriptor for the socket on which the server will listen for connections
    // newsockfd: file descriptor for the socket that the server will use to communicate with a client
    int sockfd, newsockfd;

    // socklen_t: data type used for socket length
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr; // sockaddr_in: structure containing an internet address

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
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // listen for connections on a socket : maximum 5 connection이 가능하다.
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1)
    {
        // accept a connection on a socket : blocking call 
        // server will block here until a client requests a connection
        printf("Waiting for client connection...\n");
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); // returns a new file descriptor 

        // 클라이언트의 요청이 들어오면 tcp connection을 생성한다.
        if (newsockfd < 0)
            error("ERROR on accept");


        printf("Client connected\n");
        
        // pointer to the socket file descriptor
        int *new_sock = malloc(sizeof(int));

        if (new_sock == NULL) error("ERROR allocating memory");
        *new_sock = newsockfd;

        // 스레드를 생성한다. 
        pthread_t t_id;

        // pthread_create : create a new thread
        if (pthread_create(&t_id, NULL, handle_client, (void *)new_sock) < 0) {
            error("ERROR creating thread");
        }
    }

    close(sockfd);
}
