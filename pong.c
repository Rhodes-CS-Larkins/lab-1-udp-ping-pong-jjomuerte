/*
 * pong.c - UDP ping/pong server code
 *          author: Juan Perdomo
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch;
  int nping = 1;                    // default packet count
  char *pongport = strdup(PORTNO);  // default port

  while ((ch = getopt(argc, argv, "h:n:p:")) != -1) {
    switch (ch) {
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    default:
      fprintf(stderr, "usage: pong [-n #pings] [-p port]\n");
    }
  }

  // pong implementation goes here.
  int sockfd, status;
  struct addrinfo hints, *res, *p;
  struct sockaddr client_addr;
  socklen_t addr_len;
  unsigned char buf;
  char s[INET_ADDRSTRLEN];
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  if((status = getaddrinfo(NULL, pongport, &hints, &res)) != 0){
    printf("error: gai.\n");
    return 1;
  } 
  
  for(p = res; p != NULL; p = p->ai_next){
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      printf("error: socket.\n");
      continue;
    }

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
      close(sockfd);
      printf("error: bind.\n");
      continue;
    }
    break;
  }
  
  if(p == NULL){
    printf("error: bind failed.\n");
    return 1;
  }

  freeaddrinfo(res);

  addr_len = sizeof(client_addr);

  for(int i = 0; i < nping; i++){
    if(recvfrom(sockfd, &buf, sizeof(unsigned char), 0, 
          &client_addr, &addr_len) == -1){
      printf("error: haven't received anything");
      sleep(1);
      i--;
      continue;
    }
    printf("pong[%d]: received packet from %s.\n",
        i, inet_ntop(client_addr.sa_family, 
          &client_addr, s,addr_len));
    
    buf += 1;
    
    while(sendto(sockfd, &buf, sizeof(unsigned char), 0, &client_addr,
            addr_len) == -1){
      printf("error: sending message.\n");
    }

  }
  close(sockfd);
  printf("nping: %d pongport: %s\n", nping, pongport);

  return 0;
}

