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
  //调用bind,第二个参数要强转成(struct sockaddr *)类型
  int checkBind = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
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
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, (socklen_t*)&clientAddrLen);
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
      }else if(strcmp(buffer, "quit") == 0){
        break;
      }
      printf("%d data received, data is: (%s)\n", dataSize, buffer);
      //把收到的数据发给客户端
      send(clientSocket, buffer, dataSize, 0);
    }
  }
  return 0;
}
