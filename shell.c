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


#include <conio.h>          // "Console Input Output Header File” - manages input/output on console based application.
#include <ctype.h>          // tolower()
#include <dirent.h>         // DIR, d, opendir(), readdir(), closedir()
#include <stdio.h>          // Standard I/O: printf(), fprintf(), stderr, perror(), getchar()
#include <stdlib.h>         // malloc(), realloc(), free(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>         // strcmp(), strtok()
#include <sys/stat.h>       // S_ISREG, mkdir()
#include <unistd.h>         // chdir()
#include <windows.h>        // Macros
#include <time.h>           // Get time format

/* windows.h Macros */
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501

/* Read Line Input */
#define RL_BUFSIZE 1024

/* Split Line Input */
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
#define TOK_DELIM_TIME_DATE ":"

#define BUFFER_SIZE MAX_PATH                    // The maximum path of 32,767 characters is approximate as per MSDN documentation.

TCHAR CurDir_Buffer[BUFFER_SIZE + 1];           // CurDir_Buffer is the container for the current directory. the +1 is for the NULL terminating character

int restart = 0;                                // variable that controls the relaunch of the shell


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


/* buitlin functions in their string form. */
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


/* list of addresses of the builtin functions */
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


/* number of builtin functions */
int num_builtins() {
    return sizeof(builtin_cmd) / sizeof(char *);
}


/* Checks whether a file is a directory or just a file */
int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}


/* Checking for rmdir */
int is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}


/* Converts tokens to lowercase to remove case sensitivity */
char *toLowerCase(char *str) {
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
    return str;
}

/* BUILT-IN IMPLEMENTATIONS */


/* CD - Displays the current directory */
int shell_cd(char **args) {
    GetCurrentDirectory(BUFFER_SIZE, CurDir_Buffer);

    if(args[1] == NULL) {
        printf("%s\n", CurDir_Buffer);
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
            printf("%s\n", CurDir_Buffer);
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
    FILE *in, *out;
    int ch;

    if(args[1] == NULL) {
        fprintf(stderr, "No file selected! \n");
    }
    else if (args[2] == NULL) {
        fprintf(stderr, "Invalid location! \n");
    }
    else {
        /* open the source file in read mode */
        in = fopen(args[1], "r");

        /* jump to the destination path */
        chdir(args[2]);

        /* open the destination file in write mode */
        out = fopen(args[1], "w+");

        /* copying the contents of the source file into the target file*/
        while(1) {
            ch = fgetc(in);
            if(feof(in)) {
                break;
            }
            fprintf(out, "%c", ch);
        }

        /* closing the opened files */
        fclose(in);
        fclose(out);

        /* back to the directory where we left off */
        chdir("..");

        printf("Successfully copied the file! \n");
    }

    return 1;
}


/* DATE - displays the date */
int shell_date(char **args) {
    char *month_text[] = {"","January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    char *day_text[] = {"","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    SYSTEMTIME time;

    if(args[1] == NULL) {
        /* show time if no parameters */
        GetSystemTime(&time);           
        int year = time.wYear;
        int month = time.wMonth;
        int day = time.wDay;
        int week;

        /* calculate what day it is using Key value method */
        week =  ( (day+=(month <3? year -- :(year -2))), (23* month/9+ day +4 + year /4- year /100+ year/400) ) % 7;

        printf("The current date is: %s, %s-%d-%d \n",day_text[week],month_text[time.wMonth],time.wDay,time.wYear);    
    }

    /* set date in month-day-year format */
    char *new_date[10];
    printf("Enter the new date: (mm-dd-yyyy): ");
    scanf("%s", new_date);
    SetLocalTime(new_date);

    return 1;
}


/* DELETE - deletes one or more files */
int shell_del(char **args) {
    if(args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"del\"\n");
    }
    else {
        /* File checker */
        if(is_regular_file(args[1])) {
            remove(args[1]);
        } else {
            printf("Unsuccessful. Either you are trying to delete a folder or the file does not exist.\n");
        }
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
    char type[9];

    d = opendir(".");       // opendir() returns a pointer of DIR type.

    /* opendir returns NULL if couldn't open directory */
    if (d == NULL) {
        printf("Could not open current directory");
    }
    printf(" DATE MODIFIED  \t\t\t  FILES/FOLDERS\n");
    while ((dir = readdir(d)) != NULL) {
        stat(dir->d_name, &attr);
        timeStamp = localtime (&attr.st_mtime);                         // Getting time modification attributes of the directory/file
        strftime(time, sizeof(time), "%m/%d/%Y %I:%M %p", timeStamp);
        printf("%20s ----- ", time);

        /* File checker */
        if(is_directory(dir->d_name)) {                                
            printf("<FOLDER> ----- ");
        } else {
            printf("<FILE> ------- ");
        }
        printf("%s\n", dir->d_name);
    }
    closedir(d);
    return 1;
}


/* HELP - Print  the list of available commands */
int shell_help(char **args) {
    printf(" COMMANDS: \n");
    printf(" CD\t\tDisplays the name of or changes the current directory.\n");
    printf(" CHDIR\t\tChanges the current directory.\n");
    printf(" CLS\t\tClears the screen.\n");
    printf(" CMD\t\tStarts a new instance of the command interpreter.\n");
    printf(" COPY\t\tCopies one or more files to another location.\n");
    printf(" DATE\t\tDisplays or sets the date.\n");
    printf(" DEL\t\tDeletes one or more files.\n");
    printf(" DIR\t\tDisplays a list of files and subdirectories in a directory.\n");
    printf(" HELP\t\tPrint the list of available commands.\n");
    printf(" MKDIR\t\tCreates a directory.\n");
    printf(" MOVE\t\tMoves one or more files from one directory to another directory.\n");
    printf(" RENAME\t\tRenames a file or files.\n");
    printf(" RMDIR\t\tRemoves a directory.\n");
    printf(" TIME\t\tDisplays or sets the system time.\n");
    printf(" TYPE\t\tDisplays the contents of a text file.\n");
    printf(" EXIT\t\tTerminates the program.\n\n");
    printf(" These commands are based on Windows Command Prompt. (c)2018 Microsoft Corporation.\n");
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

    /* Check if directory is created or not */
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
    FILE *in, *out;
    int ch;

    if(args[1] == NULL) {
        fprintf(stderr, "No file selected! \n");
    }
    else if (args[2] == NULL) {
        fprintf(stderr, "Invalid location! \n");
    }
    else {
        /* open the source file in read mode */
        in = fopen(args[1], "r");

        /* jump to the destination path */
        chdir(args[2]);

        /* open the destination file in write mode */
        out = fopen(args[1], "w+");


        /* copying the contents of the source file into the target file*/
        while(1) {
            ch = fgetc(in);
            if(feof(in)) {
                break;
            }
            fprintf(out, "%c", ch);
        }

        /* closing the opened files */
        fclose(in);
        fclose(out);

        /* back to the directory where we left off */
        chdir("..");

        /* removing the source file */
        remove(args[1]);
        printf("Successfully moved the file! \n");
    }
    return 1;
}


/* RENAME - rename a file or files */
int shell_rename(char **args) {

    // checks whether args[1] or args[2] is empty
    if(args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Expected argument to \"rename\"\n");
    }
    else {
        /* args[1] - existing file name, args[2] - new file name
           if the rename function's return value is 0, then the file changed its name successfully.
           else, an error is returned.
        */
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

    if(args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"rmdir\"\n");
    }
    else {
        /* File checker */
        if(is_directory(args[1])) {                          
            rmdir(args[1]);
        } else {
            printf("Unsuccessful. Either you are trying to delete a file or the folder does not exist.\n");
        }
    }
    return 1;
}


/* TIME - displays or sets system time */
int shell_time(char **args) {
    SYSTEMTIME time;

    if(args[1] == NULL){
        /* show current time in hours, minutes, and seconds */
        GetLocalTime(&time);
        printf("The current time is: %d:%d:%d\n",time.wHour, time.wMinute, time.wSecond);
    }
    
    /* set time in hours:minutes:seconds format*/
    char *new_time[10];
    printf("Enter the new time: (hh:mm:ss) ");
    scanf("%s", new_time);
    SetLocalTime(new_time);

    return 1;
}


/* TYPE - displays the contents of a text file */
int shell_type(char **args) {
    FILE *file;                                              // Initialize an object of the type FILE, which contains all the information necessary to control the stream
    char ch;

    file = fopen(args[1], "r");                              // r - read file only   
    if (file == NULL) {
        printf("File does not exist.\n");
        return 1;
    }

    /* Read contents if the text file exists */
    ch = fgetc(file);                                        // Used to obtain input from a file single character at a time
    while (ch != EOF) {
        printf ("%c", ch);
        ch = fgetc(file);
    }

    fclose(file);
    return 1;
}


/*
 *  END OF BUILT-IN IMPLEMENTATIONS. BELOW IS FUNCTIONS TO HANDLE USER INPUT AND TOKENIZING ARGUMENTS
 */


/* A signal for the command loop to terminate */
int shell_exit(char **args) {
    return 0;
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
        c = getchar();                                       // Read a character

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

        if (position >= bufsize) {                           // If we have exceeded the buffer, reallocate.
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
    char *line;                                               // Array of characters from user input
    char **args;                                              // Formatted array of characters ready for execution
    int status;

    do {
        GetCurrentDirectory(BUFFER_SIZE, CurDir_Buffer);      // Get the current directory where the program is running
        printf(("\n%s>"), CurDir_Buffer);                     // Print current directory
        line = read_line();                                   // Read the command from the standard input
        args = split_line(line);                              // Parsing/separation/tokenization of the command string  into a program and arguments. //
        if(args[0] == NULL) {
            status = 1;                                       // Resolve enter-terminate program issue
        } else {
            status = shell_execute(args);                     // Run the parsed command
        }

        free(line);                                           // Deallocates the memory previously allocated
        free(args);
    } while(status != 0);                                     // Loop until the user doesn't "exit" the program
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
