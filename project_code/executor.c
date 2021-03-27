#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h> // for memory allocation
#include <stdio.h> // for expert debugging

#include "executor.h"

/**
 * @brief Handles the execution of the child process 
 * 
 * @param command The type of command being executed 
 * @param args The list of args sent to exec 
 */
void handleChildInExecutor(char *command, char *const *args, char **env) {
    execvpe(command, args, env); 
    perror("The process failed to execute"); //Should we handle an error here if we are unable to 
    exit(-1); 
}

/**
 * @brief Handles the execution of the parent process 
 * 
 * @param pid The process id
 * @param option The option passed to waitpid 
 */
static void handleParentInExecutor(pid_t pid, int option) {
    int status; 
    waitpid(pid, &status, option); 
    //printf("Child exited: %d\n", status);
}

static void handle_input_output(struct subcommand *subcmd) {
    if (strcmp(subcmd->output, "stdout") != 0) {
        const char *filename = subcmd->output; 
        int fd = 0; 
        if (subcmd->type == REDIRECT_OUTPUT_TRUNCATE) {
            fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777); 
        } else if (subcmd->type == REDIRECT_OUTPUT_APPEND) {
            fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0777); 
        }
        close(STDOUT_FILENO); //closes stdout 
        dup2(fd, STDOUT_FILENO); 
    } if (strcmp(subcmd->input, "stdin") != 0) {
        const char *filename = subcmd->input; 
        int fd = open(filename, O_RDONLY);
        close(STDIN_FILENO);  
        dup2(fd, STDIN_FILENO); 
    }
}

/**
 * @brief Executes the command, given the type of command and the argument array 
 * 
 * @param command The type of command that is being executed, Ex. /bin/ls
 * @param args The array of args that exec() takes in
 */
void execute(char *command, char *const *args, struct subcommand *subcmd, char **env) {
    pid_t pid = fork(); 

    if (pid == 0) { //Child process 
        //if there is output
        handle_input_output(subcmd); 
        handleChildInExecutor(command, args, env); 
    } else {  //Parent process 
        handleParentInExecutor(pid, 0); 
    }

}

/**
 * @brief Runs the command typed on the command line 
 * 
 * @param len The length of the linked listssss
 * @param list_args The linked list of args that are being executed 
 */
void run_command(int len, int subcommand_count, struct list_head *list_commands, char **env) {
    //printf("length of list: %d - Subcommand Count: %d\n", len, subcommand_count); 
    struct subcommand *entry; 

    if(subcommand_count>=2){
        // //initializes an array of character pointers that will be passed to exec()
        // char **exec_arg_list = malloc(new_length * sizeof(char *)); 
        
        //Loops through each subcommand and executes
        struct list_head *curr;  
        int i=0;
        int prev_output=0;  //we need this to hold the child output for our loop
        int pipes[2];   //standard pipes

        for (curr = list_commands->next; curr != list_commands; curr = curr->next) {
            //Looks at one subcommand 
            entry = list_entry(curr, struct subcommand, list); 
            char *command = calloc((1 + strlen(entry->exec_args[0])),  sizeof(char));

            // command becomes: /bin/<command> 
            strcat(command, entry->exec_args[0]); 

            // executes a basic command
            
            int pipe_code = pipe(pipes);
            if(pipe_code < 0){
                perror("Could not create pipes.\n");
                return;
            }

            
            pid_t pid = fork(); 
            

            if (pid == 0) { //Child process 
                if(i<subcommand_count-1){   //All children except the last must write to parent
                    dup2(prev_output, 0);   //read prev_output set by parent (initially empty)
                    close(prev_output);     //close for safety
                    dup2(pipes[1], 1);      //write output back to parent
                    close(pipes[1]);        //close for safety
                }else{  //The last child doesnt output to the parent for the loop 
                    dup2(prev_output, 0);   //Read final pipe from parent
                    close(prev_output);     //close for safety
                }

                handle_input_output(entry);
                handleChildInExecutor(command, entry->exec_args, env);

            } else {  //Parent process
                prev_output=pipes[0];   //get output from the child, and ensure that we can save it for next child

                // close(pipes[0]);
                // close(pipes[1]); 
                
                //handleParentInExecutor(pid, 0);
            }

            i++;
            free(command); 
        }
    }else{
        struct list_head *curr;  
        for (curr = list_commands->next; curr != list_commands; curr = curr->next) {
            //Looks at one subcommand 
            entry = list_entry(curr, struct subcommand, list); 
            char *command = calloc((1 + strlen(entry->exec_args[0])),  sizeof(char));

            // command becomes: /bin/<command> 
            strcat(command, entry->exec_args[0]); 
            execute(command, entry->exec_args, entry, env);
            free(command); 
        }

    }
}
