#include "Server.h"
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

Server server_constructor(int domain, int service, int protocol,
                          u_long interface, int port, int backlog,
                          void (*launch)(Server *server)) {
  Server server;

  // domain - is basically which address family it will listen to like UNIX,
  // BLUETOOTH, and the one we'll use if the IPv4. service - is basically
  // choosing if ur gonna use a TCP connection protocol or a UDP connection
  //
  // protocol - there are many protocols that exists so if its set to 0
  // then it will let the service provider decide which to use
  //
  // interface - is basically the ip address in the socket sin_addr is a union
  // which has other properties liek the s_addr which is a 4-byte number that
  // represents one digit in an ip address
  //
  // port - is just the port we want to listen to
  // backlog - is basically how much users we are willing to give a connection
  // to or accept
  //
  server.domain = domain;
  server.service = service;
  server.protocol = protocol;
  server.interface = interface;
  server.port = port;
  server.backlog = backlog;

  server.address.sin_family = domain;
  server.address.sin_port = htons(port);
  server.address.sin_addr.s_addr = htonl(interface);

  // This will create a socket connection, a socket is basically what allows the
  // operating system to communicate to the network in a particular domain, type
  // which i put as service, and protocol
  server.socket = socket(domain, service, protocol);

  if (server.socket == 0) {
    perror("Failed to connect to socket\n");
    exit(1);
  }

  // so i put this here because apparently theres such a thing where after
  // closing the server there will be a delay, thats why when i run the server
  // again immediately it would give an error that the address is already in
  // use, so this function takes care of that where it will reuse the address
  // again, reuseaddr_opt is used for boolean checking 1 means enabling this
  // option and 0 means disabling it
  int reuseaddr_opt = 1;
  if (setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt,
                 sizeof(reuseaddr_opt)) < 0) {
    perror("failed to setup reusable socket\n");
    exit(1);
  }

  // so in here we are trying to bind our socket to the network
  // after the process of binding it would return either a 0 or a negative
  // number 0 means it was successful, negative num means it failed
  if ((bind(server.socket, (struct sockaddr *)&server.address,
            sizeof(server.address))) < 0) {
    perror("Failed to bind socket\n");

    exit(1);
  }

  // this listen function is just listening to a specified port thats it
  // same thing as the binding, it will also return a negative num if it failed
  // to listen to a port
  if ((listen(server.socket, server.backlog)) < 0) {
    perror("Failed to start listening\n");

    exit(1);
  }

  server.launch = launch;

  return server;
}
