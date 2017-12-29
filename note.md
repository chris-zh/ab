# note
- `getsockname()`, `getpeername()`两个函数还没搞清楚
- 服务端和客户端程序都从调用socket开始，返回一个socket描述符。客户端调用connect, 服务端调用bind, listen, accept。一般用close关闭socket，正确关闭socket在另外的章节讨论。
- 服务器一般是并发的，为每一个请求调用fork()派生一个进程去处理。除此之外还有其他的服务器设计的方法。


# Unix Socket编程套路
## 服务端步骤(回射服务器的例子)

  `socket()` -> `bind()` -> `listen()` -> `accept()` -> `read()` -> `write()` -> `read()` -> `close()`

## 客户端步骤(回射客户端的例子)

  `socket()` -> `connect()` -> `write()` -> `read()` -> `close()`

## 常用函数解释
- `int socket(int domain, int type, int protocol)` 三个参数
    - 第一个参数表示使用的地址，一般是ipv4: AF_INET
    - 第二个参数表示socket类型，tcp: SOCK_STREAM
    - 第三个参数设置为0
- `int bind(int sockfd, const struct sockaddr * serverAddr, socklen_t length)` 三个参数
    - 第一个参数是socket
    - 第二个参数是服务端地址，这个参数需要强转成(struct sockaddr \*)类型
    - 第三个参数是服务端地址长度
- `int listen(int sockfd, int backlog)`
    - 第一个参数是socket
    - 第二个参数是backlog, [关于backlog的解释](http://man7.org/linux/man-pages/man2/listen.2.html)
- `int accept(int sockfd, struct sockaddr * sockAddr, socklen_t * sockAddrLen)`
    - 第一个参数是服务端socket描述符
    - 第二个参数是传出参数，accept返回时传出客户端的地址和端口号
    - 第三个参数是传入-传出参数，传入的是服务端提供的clientAddr缓冲区的大小，传出的是客户端地址结构的实际长度
    - 出错返回-1
- `ssize_t recv(int sockfd, const void *buff, size_t nbytes, int flags)`
    - 第一个参数是客户端socket描述符
    - 第二个参数是接收缓冲区
    - 第三个参数是接收数据的长度??存疑
    - 第四个参数一般设置为0
- `ssize_t	send(int sockfd, const void * buff, size_t nbytes, int flags)`
    - 第一个参数是客户端socket描述符
    - 第二个参数是发送缓冲区
    - 第三个参数是发送数据的长度??存疑
    - 第四个参数一般设置为0


## 代码示例
- 服务端程序`server.c`

```c

  #include <sys/stat.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <netdb.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include <unistd.h>

  #define SERVER_PORT 5555

  int main(){
    int serverSocket, clientSocket;//声明服务端和客户端socket

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);//调用socket，获得服务端socket

    //异常检测

    if(serverSocket < 0){
      perror("server socket error");
      exit(0);
    }

    struct sockaddr_in serverAddr;//声明服务端地址结构
    bzero(&serverAddr, sizeof(serverAddr));//服务端地址结构填充
    serverAddr.sin_family = AF_INET;//设置地址类型
    serverAddr.sin_port = htons(SERVER_PORT);//设置地址端口,将端口转换成网络字节序
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY代表0.0.0.0,转换成网络字节序

    struct sockaddr_in clientAddr;//声明客户端地址结构
    int clientAddrLen = sizeof(clientAddr);//客户端地址长度
    char buffer[200];//读取数据用的缓冲区
    int dataSize;//客户端返回的字节数

    //调用bind,第二个参数要强转成(struct sockaddr \*)类型
    int checkBind = bind(serverSocket, (struct sockaddr \*)&serverAddr, sizeof(serverAddr));


    if(checkBind < 0){
      perror("server bind error");
      exit(0);
    }

    int checkListen = listen(serverSocket, 5);//第二个参数是backlog，一般设置到30以下


    if(checkListen < 0){
      perror("server listen error");
      exit(0);
    }

    //开始主循环，接收客户端请求并处理请求
    while(1){
      printf("Listening on port: %d\n", SERVER_PORT);
      //调用accept函数，进程进入阻塞状态
      //accept返回客户端socket的文件描述符
      //clientAddr是传出参数，accept返回时传出客户端的地址和端口号
      //clientAddrLen是传入-传出参数，传入的是服务端提供的clientAddr缓冲区的大小，传出的是客户端地址结构的实际长度
      //出错返回-1
      clientSocket = accept(serverSocket, (struct sockaddr \*)&clientAddr, (socklen_t*)&clientAddrLen);

      if(clientSocket < 0){
        perror("server accept error");
        continue;
      }

      //打印出客户端的ip和端口
      printf("client ip: %s\n", inet_ntoa(clientAddr.sin_addr));
      printf("client port: %d\n", htons(clientAddr.sin_port));

      //开始读取客户端数据...
      while(1){
        dataSize = recv(clientSocket, buffer, 1024, 0);
        if(dataSize < 0){
          perror("server recv error");
          continue;
        }else if(dataSize == 0){
          //read返回0， 可关闭连接
          break;
        }
        printf("dataSize大小：(%d)\n", dataSize);
        buffer[dataSize] = '\0';//设置缓冲区结尾为\0
        //如果客户端发送(quit\r\n)，结束传输数据，跳出循环
        if(strcmp(buffer, "quit\r\n") == 0){
          break;
        }else if(strcmp(buffer, "quit" == 0)){
          break;
        }
        printf("%d data received, data is: (%s)\n", dataSize, buffer);
        //把收到的数据发给客户端
        send(clientSocket, buffer, dataSize, 0);

      }


    }
    return 0;
  }

```
