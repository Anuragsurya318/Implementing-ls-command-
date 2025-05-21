#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

long int file_modified(char *filename) {
    struct stat sb;
    if(stat(filename, &sb) == 0){
        //No error, on success
        return sb.st_mtime;
    }
    else{
        //error, on failure
        return 0;
    }
}

int is_rebuild_needed(char *target, char deps[][100], int dep_count) {
    long int target_time = file_modified(target);
    if (target_time == 0) return 1;  // Target doesn't exist

    for (int i = 0; i < dep_count; i++) {
        long int dep_time = file_modified(deps[i]);
        if (dep_time > target_time) {
            return 1;
        }
    }
    return 0;
}

int main() {
    char line[256];
    FILE *file = fopen("Makefile.txt", "r");

    if (!file) {
        perror("Failed to open Makefile.txt");
        return 1;
    }

    while (fgets(line, sizeof(line), file)!=NULL) {
        // Parsing target and dependencies
        char target[100], dep_string[200], cmd[256];
        if (sscanf(line, "%s : %[^\n]", target, dep_string) != 2)
            continue;

        // Parsing dependencies into array
        char deps[10][100];
        int dep_count = 0;

        char *token = strtok(dep_string, " ");
        while (token && dep_count < 10) {
            strncpy(deps[dep_count++], token, 100);
            token = strtok(NULL, " ");
        }

        // Reading command line
        if (fgets(cmd, sizeof(cmd), file)==NULL) {
            fprintf(stderr, "Missing command for target %s\n", target);
            continue;
        }

        //Removing tab and white spaces 
        cmd[strlen(cmd)-1] = '\0';
        char *command = cmd;
        while (*command == ' ' || *command == '\t'){
            command++;
        }

        // Checking if rebuild is needed
        if (is_rebuild_needed(target, deps, dep_count)) {
            printf("Building %s: %s\n", target, command);
            system(command);
        } else {
            printf("%s is up to date.\n", target);
        }
    }

    fclose(file);
    return 0;
} 
