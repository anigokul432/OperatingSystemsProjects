#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    int arg_count = argc - 1;  
    pid_t pids[arg_count];

    if (arg_count <= 0) {
        printf("Must have atleast one shell command\n");
        return 1;
    }

    int pipes_struct[arg_count - 1][2];

    // Instantiate arg_count - 1 number of pipes_struct
    for (int i = 0; i < arg_count - 1; i++) {
        if (pipe(pipes_struct[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    for (int i = 0; i < arg_count; i++) {
        // Fork a child process for each arg_count
        pids[i] = fork();
        
        // Error out if fork fails
        if (pids[i] == -1) {
            perror("fork");
            exit(1);
        } 
        
        if (pids[i] == 0) {
            // Child process
            if (i >= 1) {
                // This makes sure stdout points to the end of the last pipe
                dup2(pipes_struct[i - 1][0], STDIN_FILENO);
                // Read end is closed
                close(pipes_struct[i - 1][0]);
            }

            // logic for all other cases
            if (i < arg_count - 1) {
                dup2(pipes_struct[i][1], STDOUT_FILENO);
                close(pipes_struct[i][1]);
            } else {
                dup2(STDOUT_FILENO, STDOUT_FILENO);
            }

            // Make sure all pipes_struct are closed
            for (int j = 0; j < arg_count - 1; j++) {
                close(pipes_struct[j][0]);
                close(pipes_struct[j][1]);
            }

            // Execute the command
            execlp(argv[i + 1], argv[i + 1], NULL);
            perror("execlp");
            exit(1);
        }
    }

    // close parent pipes
    for (int i = 0; i < arg_count - 1; i++) {
        close(pipes_struct[i][0]);
        close(pipes_struct[i][1]);
    }

    // Wait for all child processes to complete
    for (int i = 0; i < arg_count; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            // Error out if there are still orphans
            printf("Command '%s' failed with exit code %d\n", argv[i + 1], WEXITSTATUS(status));
            return 1;
        }
    }

    return 0;
}
