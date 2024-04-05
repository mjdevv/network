
### 사용 라이브러리 정보

1. <unistd.h> : <https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html>

* provides access to the POSIX operating system API
* POSIX(Portable Operating System Interface) defines the application programming interface that applciation programmers can interact with the Operating system.

2. <sys/socket.h> : <https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html>

sockaddr_in

3. <netinet/in.h> : <https://pubs.opengroup.org/onlinepubs/009695399/basedefs/netinet/in.h.html>

sockaddr_in 구조체는 Unix-like operating system의 소켓 프로그래밍에서 자주 사용되는 구조체다(이름처럼...)

```c
    struct sockaddr_in {
        short            sin_family;   // Address family, AF_INET (IPv4 주소)
        unsigned short   sin_port;     // Port number 
        struct in_addr   sin_addr;     // IPv4 address
        char             sin_zero[8];  // Not used, must be zero
};```

4. <arpa/inet.h> :

---

### Socket은 파일이다?

리눅스에서는 모든 peripheral들을 파일로 바라 보고 접근함. 파일은 unit of storage(contianer of data)로 이해하면 됨. 아래의 peripheral 자원들은 리눅스 OS 관점에서 파일로 바라보고 다룰 수 있다.

1. Regular files
2. Directories
3. Links
4. Devices (block and character devices)
5. Sockets
6. Pipes

소켓 또한 파일로 바라보고 접근 가능한데 applicaiton 프로그래머 관점에서 봤을 때, 로컬 디스크에서 데이터를 가져 오는 것처럼 i/o 작업을 통일 된 형태로 할 수 있기 때문에 그런 게 아닐까 싶음.

"Everything is a file". 데이터 입출력을 파일이라는 하나의 추상화 된 객체로 다룰 수 있다.

---

### File Descriptor

* file descriptor는 OS에서 관리하는 파일의 identifier. non-negative integer 값을 가진다.
* 리눅스 시스템에서는 소켓 또한 파일로 관리 되기 때문에 소켓을 열면 파일 디스크립터가 딸려 나온다.

---
