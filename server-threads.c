/**
 * HTTP Server with multi-threading implementation
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 6000

typedef struct {
  int sock_fd;
} thread_config_t;

char *res =
    "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 5\n\nHello";
size_t res_len;

// function for serving thread
void *serve(void *arg) {
  thread_config_t *config = (thread_config_t *)arg;
  int sock_fd = config->sock_fd;
  free(config);

  printf("Thread is served fd: %d.\n", sock_fd);

  char buff[1024];
  memset(buff, 0, sizeof buff);

  ssize_t numbytes = read(sock_fd, buff, sizeof buff);
  if (numbytes < 0) {
    printf("Error in read");
  }

  write(sock_fd, res, res_len);
  close(sock_fd);

  return 0;
}

int main(int argc, char *argv[]) {
  // protocol 0 is internet protocol (see /etc/protocols)
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  int new_fd;
  if (sock_fd < 0) {
    printf("Error opening socket\n");
    return 1;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);
  memset(addr.sin_zero, 0, sizeof addr.sin_zero);

  struct sockaddr_storage client_addr;
  unsigned int client_len = sizeof client_addr;

  char client_ip_str[INET6_ADDRSTRLEN];

  // binding address
  if (bind(sock_fd, (struct sockaddr *)&addr, sizeof addr) < 0) {
    printf("Error in Bind \n");
    return 1;
  }

  // listen with backlog = 2
  if (listen(sock_fd, 2) < 0) {
    printf("Error in Listen \n");
    return 1;
  }

  while (1) {
    printf("\nWaiting for new connection...\n");
    // block if no incoming connection
    new_fd = accept(sock_fd, (struct sockaddr *)&client_addr,
                    (socklen_t *)&client_len);
    if (new_fd < 0) {
      printf("Error in Accept");
      continue;
    }

    char ipstr[INET6_ADDRSTRLEN];

    // convert from network byte order to host byte order (if any)
    if (client_addr.ss_family == AF_INET) {
      inet_ntop(client_addr.ss_family,
                &(((struct sockaddr_in *)&client_addr)->sin_addr), ipstr,
                sizeof ipstr);
      printf("accept:ipv4: %s\n", ipstr);
    } else {
      inet_ntop(client_addr.ss_family,
                &(((struct sockaddr_in6 *)&client_addr)->sin6_addr), ipstr,
                sizeof ipstr);
      printf("accept:ipv6: %s\n", ipstr);
    }

    res_len = strlen(res);

    pthread_t thread;
    thread_config_t *config = (thread_config_t *)malloc(sizeof *config);
    config->sock_fd = new_fd;
    pthread_create(&thread, NULL, serve, config);
  }
  close(sock_fd);
  return 0;
}