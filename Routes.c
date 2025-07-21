#include "Routes.h"
#include <string.h>

void addRoute(Route *routes, char *Method, char *URI, char *filename,
              int *idx) {
  strcpy(routes[*idx].Method, Method);
  strcpy(routes[*idx].URI, URI);
  strcpy(routes[*idx].filename, filename);
  (*idx)++;
}
