/**
 * Retrieve IP address for the given domain name or host via command line
 */
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  struct addrinfo hints;
  struct addrinfo *result, *cur;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  if (argc != 2) {
    fprintf(stderr, "invalid arguments\n");
    return 1;
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(argv[1], NULL, &hints, &result);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  for (cur = result; cur != NULL; cur = cur->ai_next) {
    void *addr;
    char *ipver;
    if (cur->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)cur->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)cur->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    inet_ntop(cur->ai_family, addr, ipstr, sizeof ipstr);
    printf("%s: %s\n", ipver, ipstr);
  }

  return 0;
}