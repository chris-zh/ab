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
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  if(clientSocket < 0){
    perror("client socket error");
    return 1;
  }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(SERVER_PORT);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int connectfd = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if(connectfd < 0){
    perror("client connect error");
    return 1;
  }

  char sendbuff[200];
  char recvbuff[200];
  int dataSize;

  printf("connect to destination host...\n");

  while(1){
    printf("input your word:>");
    scanf("%s", sendbuff);
    printf("\n");
    send(clientSocket, sendbuff, strlen(sendbuff), 0);
    if(strcmp(sendbuff, "quit") == 0){
      break;
    }else if(strcmp(sendbuff, "quit\r\n") == 0){
      break;
    }
    dataSize = recv(clientSocket, recvbuff, 200, 0);
    recvbuff[dataSize] = '\0';
    printf("received data of my word is :%s\n", recvbuff);
  }
  close(clientSocket);
  return 0;
}
