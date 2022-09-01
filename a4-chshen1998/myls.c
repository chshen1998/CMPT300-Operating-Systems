#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

DIR *curr_dir;

int printDirectories(bool i) {
    struct dirent *dir;
    char temp[256];

    while ((dir = readdir(curr_dir)) != NULL) {
        char output[] = "";

        if (dir->d_name[0] == '.') {
            continue;
        }

        if (i) {
            snprintf(temp, 256, "%lu", dir->d_ino);
            strcat(output, temp);
            strcat(output, " ");
        }

        strcat(output, dir->d_name);
        printf("%s \n", output);
    }      
    return 0;
}

int main(int argc, char **argv) {
    char *options, *curr_path, *dir_name;
    char path[256];
    bool o_i, o_l, o_R;
    FILE *fp;
    int path_index;
    
    if (argc >= 2) {
        if (argv[1][0] == '-') {
            options = argv[1];
            path_index = 2;
        } else {
            options = "";
            path_index = 1;
        }
    } else {
        options = "";
        path_index = 1;
    }

    o_i = false;
    o_l = false;
    o_R = false;
    for (int i=0; i<strlen(options); i++) {
        if (options[i] == 'i') {
            o_i = true;
        } else if (options[i] == 'l') {
            o_l = true;
        } else if (options[i] == 'R') {
            o_R = true;
        }
    }

    if (argc < path_index+1) {
        getcwd(path, sizeof(path));
        curr_dir = opendir(path);
        printDirectories(o_i);   
    } else if (argc == path_index+1) {
        curr_path = argv[path_index];
        curr_dir = opendir(curr_path);
        printDirectories(o_i);
    } else {
        for(int i = path_index; i<argc; i++) {
            curr_path = argv[i];
            curr_dir = opendir(curr_path);
            printf("%s:\n", curr_path);
            printDirectories(o_i);
            if (i < argc-1) {
               printf("\n"); 
            }
        }
    }
    
    return 0;
}