#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 42069
#define BUFFER_SIZE 1024

typedef struct Server {
  int domain;
  int service;
  int protocol;
  u_long interface;
  int port;
  int backlog;

  struct sockaddr_in address;
  void (*launch)(struct Server *server);

  int socket;
} Server;

Server server_constructor(int domain, int service, int protocol,
                          u_long interface, int port, int backlog,
                          void (*launch)(Server *server));

#endif // !SERVER_H
