#include "Main.h"
#include "ParamStorage.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

template<typename T>
string Main::toString(const T &t){
  ostringstream oss;
  oss << t;
  return oss.str();
}

void Main::acceptJobReply(ParamStorage &p){
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
  servAddr.sin_port = htons(p.listenPort);

  bind(listenFd, (sockaddr *)&servAddr, sizeof(servAddr));

  // listen on socket
  listen(listenFd, 1);
  CkPrintf("[test] Listening for connection\n");

  // accept connection
  connFd = accept(listenFd, (sockaddr *) NULL, NULL); 

  CkPrintf("[test] Accepted connection from compute nodes!\n");

  close(connFd);
  close(listenFd);
}

string Main::findHostAddr(){
  struct addrinfo hints, *info, *p;
  int gai_result;

  char hostname[1024];
  hostname[1023] = '\0';
  gethostname(hostname, 1023);

  return string(hostname);
}
