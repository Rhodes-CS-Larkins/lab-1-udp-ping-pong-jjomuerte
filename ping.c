/*
 * ping.c - UDP ping/pong client code
 *          author: Juan Perdomo
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch;
  int nping = 1;                        // default packet count
  char *ponghost = strdup("localhost"); // default host
  char *pongport = strdup(PORTNO);      // default port
  int arraysize = 100;                  // default packet size

  while ((ch = getopt(argc, argv, "h:n:p:")) != -1) {
    switch (ch) {
    case 'h':
      ponghost = strdup(optarg);
      break;
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    case 's':
      arraysize = atoi(optarg);
      break;
    default:
      fprintf(stderr, "usage: ping [-h host] [-n #pings] [-p port]\n");
    }
  }
  // UDP ping implemenation goes here
  int status, sockfd;
  double starttime, endtime, timeelapsed, avg_time;
  double total_time = 0;
  struct addrinfo hints;
  struct addrinfo* res;
  unsigned char arr[arraysize];

  memset(arr, 200, nping);
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  
  if((status = getaddrinfo(ponghost, pongport, &hints, &res)) != 0){
    printf("error: getaddrinfo\n");
    return 1;
  }

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  for(int i = 0; i < nping; i++){
    starttime = get_wctime();
    sendto(sockfd, &(arr[i]), sizeof(unsigned char), 0, res->ai_addr, res->ai_addrlen);
    while(recvfrom(sockfd, &(arr[i]), sizeof(unsigned char), 0, res->ai_addr, &res->ai_addrlen)
        == -1){
      sleep(1);
      printf("waiting to receive.\n");
    }
    endtime = get_wctime();
    if(201 != (int)arr[i]){
      printf("error: did not receive what was expected from pong, received: %c\n", arr[i]);
      return -1;
    }
    timeelapsed = endtime - starttime;
    printf("ping[%d] : round-trip time: %f ms.\n", i, timeelapsed);
    total_time += timeelapsed;
  }
  printf("I made it out the loop!\n");

  avg_time = total_time/nping;

  printf("total time: %f; average time: %f\n", total_time, avg_time);
  close(sockfd);
  freeaddrinfo(res);
  return 0;
}
