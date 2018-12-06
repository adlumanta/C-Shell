/* CMSC 125 Operating Systems
 * A.Y. 2018 - 2019 Semester 1
 * University of the Philippines Cebu
 *
 * CAPSTONE PROJECT: SHELL
 * Description: Implementing a kernel interface using C Programming language.
 *
 * Created by:
 * Okiya, Franklin
 * Lumanta, Angelo Rey
 */

#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <unistd.h>
#include <windows.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501
#define RL_BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
#define BUFFER_SIZE MAX_PATH              // The maximum path of 32,767 characters is approximate as per MSDN documentation.

TCHAR CurDir_Buffer[BUFFER_SIZE + 1];     // CurDir_Buffer is the container for the current directory. the +1 is for the NULL terminating character

int restart = 0;                          // variable that controls the relaunch of the shell

/* BUILT-IN IMPLEMENTATIONS */
int shell_cd(char **args);
int shell_chdir(char **args);
int shell_cls();
int shell_cmd();
int shell_copy(char **args);
int shell_date(char **args);
int shell_del(char **args);
int shell_dir(char **args);
int shell_exit(char **args);
int shell_help(char **args);
int shell_mkdir(char **args);
int shell_move(char **args);
int shell_rename(char **args);
int shell_rmdir(char **args);
int shell_time(char **args);
int shell_type(char **args);


/* List of builtin commands followed by their correspondng functions */
char *builtin_cmd[] = {
  "cd",
  "chdir",
  "cls",
  "cmd",
  "copy",
  "date",
  "del",
  "dir",
  "exit",
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
  &shell_exit,
  &shell_help,
  &shell_mkdir,
  &shell_move,
  &shell_rename,
  &shell_rmdir,
  &shell_time,
  &shell_type
};

int num_builtins() {
  return sizeof(builtin_cmd) / sizeof(char *);
}

/* BUILT-IN IMPLEMENTATIONS */

/* CD - Displays the current directory */
int shell_cd(char **args) {
  GetCurrentDirectory(BUFFER_SIZE, CurDir_Buffer);
  if(args[1] == NULL) {
    fprintf(stderr, "expected argument to \"cd\"\n");
  } else {
    if(chdir(args[1]) != 0) {
      perror("Argument not found! \n");
    }
  }
  return 1;
}


/* CHDIR - Changes the current directory */
int shell_chdir(char **args) {
  GetCurrentDirectory(BUFFER_SIZE, CurDir_Buffer);
  if(args[1] == NULL) {
    fprintf(stderr, "expected argument to \"cd\"\n");
  } else {
    if(chdir(args[1]) != 0) {
      perror("Argument not found! \n");
    }
  }
  return 1;
}


/* CLS - Clear console */
int shell_cls() {
  return 1;
}


/* CMD - Starts a new instance of the command interpreter/shell */
int shell_cmd() {
  restart = 1;// flagged to start a new instance of the shell.

  return 0; // gets out of the current loop
}



/* COPY - copies one or more files to another location */
int shell_copy(char **args) {
  return 1;
}


/* DATE - displays the date */
int shell_date(char **args) {
  return 1;
}


/* DELETE - deletes one or more files */
int shell_del(char **args) {
  return 1;
}


/* DIR - Displays a list of files and subdirectories in a directory */
int shell_dir(char **args) {
  return 1;
}


/* HELP - Print the list of available commands */
int shell_help(char **args) {
  int i;
  printf("Type the program names and arguments, then hit enter.\n");
  printf("The following are built in:\n");

  for(i = 0; i < num_builtins(); i++) {
    printf("  %s\n", builtin_cmd[i]);
  }
  return 1;
}


/* MKDIR - creates a new directory/folder */
int shell_mkdir(char **args) {
  return 1;
}


/* MOVE - moves one or more files from one directory to another directory */
int shell_move(char **args) {
  return 1;
}


/* RENAME - rename a file or files */
int shell_rename(char **args) {
  return 1;
}


/* RMDIR - removes a directory */
int shell_rmdir(char **args) {
  return 1;
}


/* TIME - displays or sets asystem time */
int shell_time(char **args) {
  return 1;
}


/* TYPE - displays the contents of a text file */
int shell_type(char **args) {
  return 1;
}


/* A signal for the command loop to terminate */
int shell_exit(char **args) {
  return 0;
}


/* launch either a builtin or a proces */
int shell_execute(char **args) {
  int i;

  if(args[0] == NULL) {
    // empty command input
    return 1;
  }

  for(i = 0; i < num_builtins(); i++) {
      if(strcmp(args[0], "cd..") == 0) {
        args[0] = "cd";
        args[1] = "..";
      }
    if(strcmp(args[0], builtin_cmd[i]) == 0) {
       return(*builtin_func[i])(args);
    }
  }
  printf("Error: \"%s\"is not a builtin command.", args[0]);
  return 1;
}


/* Reads a line of input from stdin */
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

    // if we hit EOF, replace it with a null character and return
    // EOF us an integer, not a character
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


/* Separate the command string into program and arguments */
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

/* A loop getting an input and executing it */
void loop(void) {
  char *line;
  char **args;
  int status;

  do {
    // get the current directory where the program is running
    GetCurrentDirectory(BUFFER_SIZE, CurDir_Buffer);
    printf(("\n%s>"), CurDir_Buffer);

    // read the command from the standard input
    line = read_line();

    /* parsing/separation/tokenization of the command string
     * into a program and arguments.
     */
    args = split_line(line);

    // run the parsed command
    status = shell_execute(args);

    free(line);
    free(args);
  } while(status != 0);
}

int main() {
  do {
    restart = 0;
    loop();
    if(restart == 0) {
      return EXIT_SUCCESS;
    }
  }while(restart == 1);

  return 0;
}
