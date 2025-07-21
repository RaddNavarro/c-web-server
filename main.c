#include "Routes.h"
#include "Server.h"
#include <arpa/inet.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// the arrangment of these functions is basically the flow of the server
void launch(Server *server);
void handle_requests(int client_socket);
void parse_request(char *req_buffer, int client_socket);
void find_route(char *Method, char *URI, int client_socket);
void send_html(int client_socket, const char *file_path);

Route routes[5];

int main() {
  int idx = 0;
  addRoute(routes, "GET", "/", "index.html", &idx);
  addRoute(routes, "GET", "/about", "about.html", &idx);

  Server server =
      server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, PORT, 10, launch);
  printf("Listening on port: %d\n", PORT);
  server.launch(&server);
  close(server.socket);
}

void launch(Server *server) {

  int addrlen = sizeof(server->address);
  int client_socket;

  while (1) {

    client_socket = accept(server->socket, (struct sockaddr *)&server->address,
                           (socklen_t *)&addrlen);
    if (client_socket < 0) {
      perror("Accepting connection failed\n");
      continue;
    }

    handle_requests(client_socket);

    printf("Client connected\n");
    close(client_socket);
  }
}

void send_html(int client_socket, const char *file_path) {

  FILE *html_file = fopen(file_path, "r");
  if (html_file == NULL) {
    perror("File not found\n");
    exit(1);
  }

  char buffer[BUFFER_SIZE];
  size_t bytes_read;

  char *http_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  send(client_socket, http_header, strlen(http_header), 0);

  while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, html_file)) > 0) {
    send(client_socket, buffer, bytes_read, 0);
  }

  fclose(html_file);
}

void handle_requests(int client_socket) {

  char req_buffer[1024];

  int bytes_recv = recv(client_socket, req_buffer, sizeof(req_buffer) - 1, 0);

  if (bytes_recv < 0) {
    perror("Error receiving requests");
    exit(1);
  }

  parse_request(req_buffer, client_socket);
}

void parse_request(char *req_buffer, int client_socket) {
  char *original;
  char *firstLine, *theRest;
  char *Method, *URI;
  int i = 0;

  printf("%s", req_buffer);
  original = strdup(req_buffer);
  theRest = original;
  firstLine = strtok_r(theRest, "\n", &theRest);

  // stupid way of getting the http method and the uri request
  // pls fix this with something else omg this is so ugly
  for (i = 0; i < 2; i++) {
    switch (i) {
    case 0:
      Method = strtok_r(firstLine, " ", &firstLine);
      break;
    case 1:
      URI = strtok_r(firstLine, " ", &firstLine);
      break;
    }
  }

  find_route(Method, URI, client_socket);
}

void find_route(char *Method, char *URI, int client_socket) {
  int i = 0;
  int routesLen = sizeof(routes) / sizeof(routes[0]);

  while (i < routesLen) {
    if ((strcmp(routes[i].Method, Method)) == 0 &&
        (strcmp(routes[i].URI, URI) == 0)) {
      send_html(client_socket, routes[i].filename);
      break;
    }
    i++;
  }

  if (i == routesLen) {
    send_html(client_socket, "404.html");
  }
}
