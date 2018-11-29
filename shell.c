#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RL_BUFSIZE 1024

#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"


// function declaration for builtin shell commands
int shell_cd(char **args);
int shell_chdir(char **args);
int shell_cls();
int shell_cmd();
int shell_copy(char **args);
int shell_date(char **args);
int shell_del(char **args);
int shell_dir(char **args);
int shell_help(char **args);
int shell_mkdir(char **args);
int shell_move(char **args);
int shell_rename(char **args);
int shell_rmdir(char **args);
int shell_time(char **args);
int shell_type(char **args);


// list of builtin commands, followed by their correspondng functions.
char *builtin_cmd[] = {
  "cd",
  "chdir",
  "cls",
  "cmd",
  "copy",
  "date",
  "del",
  "dir",
  "help",
  "mkdir",
  "move",
  "rename",
  "rmdir",
  "time",
  "type"
};

int(*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_chdir,
  &shell_cls,
  &shell_cmd,
  &shell_copy,
  &shell_date,
  &shell_del,
  &shell_dir,
  &shell_help,
  &shell_mkdir,
  &shell_move,
  &shell_rename,
  &shell_rmdir,
  &shell_time,
  &shell_type,
};

int num_builtins() {
  return sizeof(builtin_cmd) / sizeof(char *);
}


char *read_line(void) {
  int bufsize = RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "Allocation error!\n");
    exit(EXIT_FAILURE);
  }

  while(1) {
    // read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // if we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "Allocation error!\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}


// separate the command string into program and arguments
char **split_line(char *line) {
  int bufsize = TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "Allocation error!\n");
    exit(EXIT_FAILURE);
  }
  token = strtok(line, TOK_DELIM);
  while(token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "Allocation error!\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}


void loop(void) {
  char *line;
  char **args;
  int status;

  printf("C-Shell build 1.0 by /n Lumanta & Okiya");

  do {
    printf("> ");
    line = read_line();
    args = split_line(line);
    status = shell_execute(args);

    free(line);
    free(args);
  } while(status);
}



int main() {
  // load configuration files

  // run command loop
  loop();

  // perform any shutdown/cleanup

  return EXIT_SUCCESS;
}
