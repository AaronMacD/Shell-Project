#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/resource.h>

int main(void) {


    //Perform necessary initialization
    char input[50];
    //char *args[10];
    //char *token;
    struct rusage rsc_usage;
    int unknown_cmd_count = 0;

    pid_t pid;
    int status;

    printf("Welcome to dumShell. Please enter commands or quit to close the shell.\n");


    while (1) {
        //prompt user for command
        printf("Enter commmand > ");
        //parse the command
        char *args[20];
        if (fgets(input, sizeof(input), stdin) != NULL) {
            char *token = strtok(input, " \n");
            int i = 0;

            while (token != NULL) {
                //printf ("[%s]\n", token);
                //printf ("[%s]\n", args[i]);

                args[i] = token;
                token = strtok(NULL, " \n");
                i++;
            }
            token = NULL;
            //printf("\"%s\"\n", args[0]);
        }
        else {
            printf("Error reading input.");
        }
        //if (command is "quit") break the loop
        if (strcmp(args[0],"quit") == 0) {
            printf("Unknown commands entered: %d.\nClosing dumShell. Goodbye!\n", unknown_cmd_count);
            break;
        }

        //fork a new process
        pid = fork();
        if (pid < 0) {
            printf("Failed to fork. Error: \"%s\". \n", strerror(errno));
        }
        //(a) the child calls exec*() to run the command and exit()
        else if (pid == 0) {
            int result = execvp(args[0], args);
            if (result < 0) {
                printf("Exec call failed. Error: \"%s\". \n", strerror(errno));
                exit(1);
            }
            exit(10);

        }
        //(b) the parent calls wait() to retrieve the child status
        else {
            const int result = wait(&status);
            //printf("status = %d\n", WEXITSTATUS(status));
            if (result < 0) {
                printf("Wait call failed. Error: \"%s\". \n", strerror(errno));
            }
            else if (WEXITSTATUS(status)== 1) {
                //child already prints error code so just need to increment counter
                unknown_cmd_count++;
            }
            //after wait, print usage statistics
            else {
                getrusage(RUSAGE_CHILDREN, &rsc_usage);
                printf("User CPU time used for %d is %ld microseconds.\n", result, rsc_usage.ru_utime.tv_usec);
                printf("Number of involuntary context switches of %d is %ld. \n", result, rsc_usage.ru_nivcsw);
            }

        }
        memset(input, 0, sizeof input);
        memset(args, 0,sizeof args);

    }
    return 0;
}
