#ifndef ROUTES_H
#define ROUTES_H

#define MAX_CHAR 99

typedef struct Route {
  char Method[MAX_CHAR];
  char URI[MAX_CHAR];
  char filename[MAX_CHAR];
} Route;

void addRoute(Route *routes, char *Method, char *URI, char *filename, int *idx);
#endif
