#include <iostream>
#include <sstream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#include "Main.h"

void Main::sendSocketReply(){
  trySendReply();
}

void Main::RetryHeadNodeConnect(void *arg, double){
  Main *main = (Main *)arg;
  main->trySendReply();
}

void Main::trySendReply(){
  int rv;

  addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;

  addrinfo *servinfo;
  ostringstream oss;
  oss << replyPort_; 

  CkPrintf("Main::trySendReply %s:%d\n", replyAddress_.c_str(), replyPort_);

  if ((rv = getaddrinfo(replyAddress_.c_str(), oss.str().c_str(), &hints, &servinfo)) != 0) {
    CkPrintf("getaddrinfo: %s\n", gai_strerror(rv));
    CkExit();
  }

  int sockfd; 
  int nHostAddrs = 0;
  bool success = false;

  for(addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
    CkPrintf("[main] hostname: %s\n", p->ai_canonname);
    nHostAddrs++;

    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("connect");
      continue;
    }

    success = true;
    break;
  }
  
  freeaddrinfo(servinfo);

  retryAttempts_--;

  if(success){
    CkPrintf("[main] sendSocketReply: connection to head node successful!\n");
    close(sockfd);
  }
  else if(retryAttempts_ > 0){
    CkPrintf("[main] sendSocketReply: couldn't connect to head node, retrying in %d seconds\n", retryPeriod_);
    CcdCallFnAfter(Main::RetryHeadNodeConnect, this, retryPeriod_ * 1000);
  }
  else{
    CkPrintf("[main] sendSocketReply: unable to connect to head node\n");
    CkExit();
  }
}
