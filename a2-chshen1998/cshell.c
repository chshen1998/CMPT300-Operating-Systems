#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

struct envvar {
    char *name;
    char *value;
};

struct log {
    char *name;
    struct tm time;
    int return_value;
};

struct envvar variables[256];
int num_variables;
int fds[2];
char input[256], output[256], full_command[256];
char *command, *str, *new_name, *new_value, *theme;
bool updated;
FILE *fp, *file;

int run(char r_input[256]) {
    if (r_input == NULL) {
        // Get user input
        printf("Enter command: ");
        str = fgets(input, 256, stdin);  
        input[strcspn(input, "\n")] = 0; 
    } else {
        strncpy(input, r_input, 256);
        input[strcspn(input, "\n")] = 0; 
        input[strcspn(input, "\r")] = 0; 
    }
    strncpy(full_command, input, 256); 
    command = strtok(input, " ");
    // Create pipe
    if (pipe(fds) == -1) { 
        perror("Error: ");
        return (-1);
    }
    // Create fork
    int fc = fork();

    if (fc < 0) { // Failed to fork
        printf("Fork failed \n");
    } else if (fc == 0) { // Child process
        // Close the read end
        close(fds[0]);

        //if (command[0] == '$') {
        //    // Add or update env variabl
        //    exit(1);
        if (strcmp(command, "log") == 0) {
            // Log command
            exit(1);
        } else if (strcmp(command, "print") == 0) { 
            // print command 
            // Allow env variables
            command = strtok(NULL, " ");
            while (command != NULL) {
                write(fds[1], command, 256);
                command = strtok(NULL, " ");
            }
            close(fds[1]);
            exit(1);
        } else if (strcmp(command, "theme") == 0) {
            // Theme command
            char *code;
            command = strtok(NULL, " ");
            if (strcmp(command, "red") == 0) {
                code = "\e[0;31m";
            } else if (strcmp(command, "green") == 0) {
                code = "\e[0;32m";
            } else if (strcmp(command, "blue") == 0) {
                code = "\e[0;34m";
            } else {
                code = NULL;
            }
            while (code != NULL) {
                write(fds[1], code, 256);
                code = strtok(NULL, " ");
            }
            close(fds[1]);
            exit(1);
        } else if (strcmp(command, "exit") == 0) {
            // Exit command
            close(fds[1]);
            exit(1);
        } else { 
            // Non built in commands
            fp = popen(full_command, "r");
            if (fp == NULL) {
                printf("failed to run command\n");
                exit(1);
            } 
            while (fgets(output, 256, fp) != NULL) {
                write(fds[1], output, 256);
            }
            pclose(fp);
            close(fds[1]);
            exit(1);
        } 
        exit(1);  
    } else { // Parent process
        wait(NULL);
        close(fds[1]);
        if (strcmp(command, "print")==0) {
            while(read(fds[0], output, 256) > 0){
                if (output[0] == '$') {
                    for (int i = 0; i<num_variables; i++) {
                        if (strcmp(variables[i].name, output)) {
                            printf("%s ", variables[i].value);
                        }
                    }
                } else {
                    printf("%s ",  output);
                }
            }
            printf("\n");
        } else if (strcmp(command, "theme") == 0) {
            if (read(fds[0], output, 256)==0) {
                command = strtok(NULL, " ");
                printf("Unsupported theme: %s\n", command);
            } else {
                theme = output;
                printf("%s", theme);
            }
        } else if (strcmp(command, "exit") == 0) {
            printf("Bye! \n");
            return 0;
        } else {
            while(read(fds[0], output, 256) > 0){
                printf("%s", output);
            }
        }
        close(fds[0]);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char line[256];
    theme = "";
    num_variables = 0;
    int c;

    if (argc == 2) {
        file = fopen(argv[1], "r");
        if (file) {
            while (fgets(line, 256, file) != NULL) {
                run(line);
            }    
            fclose(file);
        } else {
            perror("Unable to read script file: ");
        }
    } else {
        while (true) {
            run(NULL);
        }        
    }


    return 0;
}