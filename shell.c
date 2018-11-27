#include <stdio.h>
#include <stdlib.h>
#define MAX_WORD 10
#define MAX_CHAR 100


void readline(char line[]) {

}


int read_parse_line(char* args[], char line[]) {
  read_line(line);
  process_line(args, line);

}

int main() {
  char* args[MAX_WORD];
  char line[MAX_CHAR];

  read_parse_line(args, line);
  return 0;
}
