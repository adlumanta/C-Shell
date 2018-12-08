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

#include <sys/types.h>
#include <sys/stat.h>
#include <conio.h>        //  "Console Input Output Header File” - manages input/output on console based application.
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <unistd.h>
#include <windows.h>
#include <time.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501

// Read Line Input
#define RL_BUFSIZE 1024

// Split Line Input
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

#define BUFFER_SIZE MAX_PATH                    // The maximum path of 32,767 characters is approximate as per MSDN documentation.

TCHAR CurDir_Buffer[BUFFER_SIZE + 1];           // CurDir_Buffer is the container for the current directory. the +1 is for the NULL terminating character

int restart = 0;                                // variable that controls the relaunch of the shell


// Checks whether a file is a directory or just a file
int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/* BUILTIN FUNCTIONS INITIALIZATION */
int shell_cd(char **args);
int shell_chdir(char **args);
int shell_cls(char **args);
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
        printf("\n%s\n", CurDir_Buffer);
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
        fprintf(stderr, "Expected argument to \"chdir\"\n");
    } else {
        if(chdir(args[1]) != 0) {
            perror("Argument not found! \n");
        } else {
            GetCurrentDirectory(BUFFER_SIZE, CurDir_Buffer);
            printf("\n%s\n", CurDir_Buffer);
        }
    }
    return 1;
}



/* CLS - Clear console */
int shell_cls(char **args) {
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
    if (hStdOut == INVALID_HANDLE_VALUE) {
        return 1;
    }
    /* Get the number of cells in the current buffer */
    if(!GetConsoleScreenBufferInfo( hStdOut, &csbi )) {
        return 1;
    }

    /* compute the total number of cells in the screen */
    cellCount = csbi.dwSize.X *csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if(!FillConsoleOutputCharacter(hStdOut, (TCHAR) ' ', cellCount, homeCoords, &count)) {
        return 1;
    }

    /* Move the cursor home */
    SetConsoleCursorPosition( hStdOut, homeCoords );
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

    // show time if there are no parameters
    if(args[1] == NULL) {

    }


    return 1;
}


/* DELETE - deletes one or more files */
int shell_del(char **args) {
    if(args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"del\"\n");
    }
    else {
        if(is_regular_file(args[1]))    // File checker
        remove(args[1]);
    }
    return 1;
}

/* DIR - Displays a list of files and subdirectories in a directory */
int shell_dir(char **args) {
    DIR *d;
    struct dirent *dir;     // Pointer for directory entry
    struct stat attr;       // Data structure containing detailed file information
    struct tm *timeStamp;   // Structure containing a calendar date and time broken down into its components

    char time[20];          // Timestamp

    d = opendir(".");       // opendir() returns a pointer of DIR type.

    // opendir returns NULL if couldn't open directory
    if (d == NULL) {
        printf("Could not open current directory");
    }
    printf(" DATE MODIFIED       \t\tFILES/FOLDERS\n");
    while ((dir = readdir(d)) != NULL) {
        stat(dir->d_name, &attr);
        timeStamp = localtime (&attr.st_mtime);                         //Getting time modification attributes of the directory/file
        strftime(time, sizeof(time), "%m/%d/%Y %I:%M %p", timeStamp);
        printf("%20s ---------- %s\n", time, dir->d_name);

    }
    closedir(d);
    return 1;
}


/* HELP - Print  the list of available commands */
int shell_help(char **args) {
    int i;
    printf("List of commands: \n");

    for(i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_cmd[i]);
    }
    return 1;
}


/* MKDIR - creates a new directory/folder */
int shell_mkdir(char **args) {
    int check;

    if(args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"mkdir\"\n");
    }

    char* dirname = args[1];
    check = mkdir(dirname);

    // Check if directory is created or not
    if(!check) {
        printf("Successfully created the directory. \n");
    }
    else {
        perror("Unable to create the directory. \n");
    }

    return 1;
}


/* MOVE - moves one or more files from one directory to another directory */
int shell_move(char **args) {
    return 1;
}


/* RENAME - rename a file or files */
int shell_rename(char **args) {

    // checks whether args[1] or args[2] is emptyd
    if(args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Expected argument to \"rename\"\n");
    }
    else {
        // args[1] - existing file name, args[2] - new file name
        // if the rename function's return value is 0, then the file changed its name successfully.
        // else, an error is returned.
        if(rename(args[1], args[2]) == 0) {
            printf("Successfully renamed the file. \n");
        }
        else {
            perror("Unable to rename the file! \n");
        }
    }

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
    FILE *file;     // Initialize an object of the type FILE, which contains all the information necessary to control the stream
    char ch;

    file = fopen(args[1], "r");     // r - read file only
    if (file == NULL) {
        printf("File does not exist.\n");
        return 1;
    }

    /* Read contents if the text file exists */
    ch = fgetc(file);   // Used to obtain input from a file single character at a time
    while (ch != EOF) {
        printf ("%c", ch);
        ch = fgetc(file);
    }

    fclose(file);
    return 1;
}

/* A signal for the command loop to terminate */
int shell_exit(char **args) {
    return 0;
}

/* Converts tokens to lowercase to remove case sensitivity */
char *toLowerCase(char *str) {
	for(int i = 0; str[i]; i++){
	    str[i] = tolower(str[i]);
	}
	return str;
}

/* launch either a builtin or a proces */
int shell_execute(char **args) {
    toLowerCase(args[0]);
    int i;

    if(args[0] == NULL) {
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

    printf("Error: \"%s\" is not a built-in command. \n", args[0]);
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
        c = getchar();  // Read a character

        /* If we hit EOF, replace it with a null character and return
         * EOF as an integer, not a character
         */
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
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

/* THE COMMAND LOOP */
void loop(void) {
    char *line;     // Array of characters from user input
    char **args;    // Formatted array of characters ready for execution
    int status;

    do {
        GetCurrentDirectory(BUFFER_SIZE, CurDir_Buffer);      // Get the current directory where the program is running
        printf(("\n%s>"), CurDir_Buffer);                     // Print current directory
        line = read_line();                                   // Read the command from the standard input

        /*
         * Parsing/separation/tokenization of the command string
         * into a program and arguments.
         */
        args = split_line(line);

        status = shell_execute(args);       // Run the parsed command

        free(line);                         // Deallocates the memory previously allocated
        free(args);
    } while(status != 0);                   // Loop until the user doesn't "exit" the program
}


/*  MAIN FUNCTION */
int main() {
    printf("SHELL (Version 1.0)\n");
    printf("CMSC 125 Operating Systems Capstone Project by Okiya and Lumanta.\n");
    do {
        restart = 0;
        loop();
        if(restart == 0) {
            return EXIT_SUCCESS;
        }
     } while(restart == 1);

  return 0;
}
