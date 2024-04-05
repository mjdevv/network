#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 12345
#define BUFFER_SIZE 256

// 에러 메세지
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{

    // socket file descriptor : client side socket
    int sockfd, n;

    // sockaddr_in : structure containing an internet address
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // 일단 256 바이트로 설정
    char buffer[256];

    // get socket file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    // gethostbyname : returns a structure including host information
    server = gethostbyname("localhost"); // loopback으로 하드코딩

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET; // IPv4

    // bcopy : copy memory area
    bcopy(
        (char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    printf("Please enter the message: ");

    // bzero : fill buffer with 0. clear all data in buffer to 0
    bzero(buffer, 256);

    // fgets : read a line from stdin and store it in buffer
    fgets(buffer, 255, stdin); // 255개 문자를 읽어서 buffer에 저장

    // write message to server
    n = write(sockfd, buffer, strlen(buffer)); // buffer에 있는 문자열을 server에 전송

    if (n < 0)
        error("ERROR writing to socket");

    // clear buffer
    bzero(buffer, 256); // buffer에 있는 데이터를 모두 0으로 초기화. 버퍼를 재활용 한다.

    n = read(sockfd, buffer, 255); // server로부터 데이터를 읽어 buffer에 저장

    if (n < 0)
        error("ERROR reading from socket");

    printf("%s\n", buffer);
    close(sockfd);
    return 0;
}
