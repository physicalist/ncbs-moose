#include "Main.h"
#include "ParamStorage.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

string Main::launchJob(ParamStorage &p){
  // set up a socket 
  int listenFd;
  int connFd;
  sockaddr_in servAddr;

  char recvBuf[1025];

  listenFd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&servAddr, '0', sizeof(servAddr));
  memset(recvBuf, '0', sizeof(recvBuf)); 

  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(p.defaultListenPort);

  bind(listenFd, (sockaddr *)&servAddr, sizeof(servAddr));

  // XXX launch job

  // listen on socket
  listen(listenFd, 1);

  // accept connection
  connFd = accept(listenFd, (sockaddr *) NULL, NULL); 

  // XXX read CCS serverName from socket
  string serverName;

  close(connFd);

  // return serverName 
  return serverName;
}
