#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() 
{
    char *cmd = NULL;
    size_t size = 0;
    char path[100];

    while(1)
    {
        printf("$ ");
        /* get input */
        if((getline(&cmd, &size, stdin)) < 0)
        {
            perror("getline failed\n");
            exit(1);
        }

        /* if "quit", exit */
        if(strncmp(cmd, "quit", 4) == 0)
        {
            free(cmd);
            exit(0);
        }

        /* separate command name and options */
        /* ---------your code here---------- */
        cmd[strlen(cmd) - 1] = '\0';

        int i = 0;
        char *ptr = strtok(cmd, " ");
        char *args[20];

        while(ptr != NULL)
        {
            args[i++] = ptr;
            ptr = strtok(NULL, " ");
        }
        args[i] = NULL;

        sprintf(path, "/bin/%s", args[0]);

        if(fork() == 0)
        {
            execv(path, args);
        }
        wait(NULL);

        /* execute command using fork()
           you only need to use fork() once! */
        /* ---------your code here ----------*/
    }
}