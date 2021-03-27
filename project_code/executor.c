#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h> // for memory allocation
#include <stdio.h> // for expert debugging
#include "datastructures.h"
#include "list.h"

// struct subcommand {
//     char *stdin; 
//     char *stdout; 
//     enum Token output_type; 
// }; 

/*
void get_input_output_old(struct list_head *arg, struct subcommand *subcommand) {
    struct list_head *curr;  
    argument *entry; 
    subcommand->stdin = NULL; 
    subcommand->stdout = NULL; 
    
    for (curr = arg->next; curr != arg; curr = curr->next) {
        entry = list_entry(curr, argument, list); 
        if (entry->token == REDIRECT_OUTPUT_APPEND) {
            subcommand->output_type = entry->token; 
            entry = list_entry(curr->next, argument, list);
            subcommand->stdout = strdup(entry->contents); 

            //Delete the current entry target and free from memory
            list_del(&entry->list); 
            free(entry);

            entry = list_entry(curr, argument, list);
            list_del(&entry->list); 
            free(entry);

            //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
            arg=curr;

        } else if (entry->token == REDIRECT_OUTPUT_TRUNCATE) {
            subcommand->output_type = entry->token; 
            entry = list_entry(curr->next, argument, list);
            printf("1: (%s)\n", entry->contents);
            subcommand->stdout = strdup(entry->contents); 

            //Delete the current entry target and free from memory
            list_del(&entry->list); 
            free(entry);

            entry = list_entry(curr, argument, list);
            printf("2: (%s)\n", entry->contents);
            list_del(&entry->list); 
            free(entry);

            //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
            arg=curr;
            
        } else if (entry->token == REDIRECT_INPUT) {
            entry = list_entry(curr->next, argument, list);
            printf("1: (%s)\n", entry->contents);
            subcommand->stdin = strdup(entry->contents);
            
            //Delete the current entry target and free from memory
            list_del(&entry->list); 
            free(entry);
            
            entry = list_entry(curr, argument, list);
            list_del(&entry->list); 
            free(entry);

            //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
            arg=curr;
        }
    }
}


/**
 * @brief makes a list of arguments
 * 
 * @param 
 * @param list The list that will be turned into an array of characetr pointers 
 * @return char* Returns a list of arguments in the char **args
 *
void makeArgumentList(struct list_head *list_args, char **args, int len) {
    struct list_head *curr;  
    argument *entry; 
    int i = len - 1; 
    
    for (curr = list_args->next; curr != list_args; curr = curr->next) {
        entry = list_entry(curr, argument, list); 
        args[i] = strndup(entry->contents, strlen(entry->contents)); 
        i--; 
    }
    
    //need to manually set the last argument to NULL, even though it is in linked list
    //may want to just not add null on the linked list 
    args[len - 1] = NULL; 
    //the exec args list should look something like 
    //args = {"element1", "element2", NULL}; 

}


/**
 * @brief Frees the memory contents created to store the argument list 
 * 
 * @param args The malloced array of char pointers being freed
 * @param len The length of the list being freed 
 *
void free_exec_arg_list(char **args, int len) {
    int i; 
    for (i = 0; i < len; i++) {
        free(args[i]); 
    }
    free(args); 

}*/

/**
 * @brief Handles the execution of the child process 
 * 
 * @param command The type of command being executed 
 * @param args The list of args sent to exec 
 */
void handleChildInExecutor(char *command, char *const *args) {
    execvp(command, args); 
    perror("The process failed to execute"); //Should we handle an error here if we are unable to 
    exit(-1); 
}

/**
 * @brief Handles the execution of the parent process 
 * 
 * @param pid The process id
 * @param option The option passed to waitpid 
 */
void handleParentInExecutor(pid_t pid, int option) {
    int status; 
    waitpid(pid, &status, option); 
}

void handle_input_output(struct subcommand *subcmd) {
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
void execute(char *command, char *const *args, struct subcommand *subcmd) {
    pid_t pid = fork(); 
    int pipes[2];
    int prev_output;
    int pipe_code = pipe(pipes);
    if(pipe_code < 0){
        perror("Could not create pipes.\n");
        return;
    }

    prev_output=pipes[0];

    if (pid == 0) { //Child process 
        
        dup2(pipes[0], STDIN_FILENO);  //Dup to parent pipe
        dup2(pipes[1], STDOUT_FILENO);  //Dup to parent pipe

        //if there is output
        handle_input_output(subcmd); 
        handleChildInExecutor(command, args); 
    } else {  //Parent process 
        pipes[0] = prev_output;
        handleParentInExecutor(pid, 0); 
    }
}

/**
 * @brief Runs the command typed on the command line 
 * 
 * @param len The length of the linked list 
 * @param list_args The linked list of args that are being executed 
 */
void run_command(int len, int subcommand_count, struct list_head *list_commands) {
    printf("length of list: %d - Subcommand Count: %d\n", len, subcommand_count); 
    struct subcommand *entry; 
    // struct subcommand subcmd; 
    // get_input_output_old(list_args, &subcmd);  
    // int new_length = getListLength(list_args); 
    // display_list(list_args);

    if(subcommand_count>1){
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
            strcpy(command, "/bin/"); 
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
                handleChildInExecutor(command, entry->exec_args);

            } else {  //Parent process
                prev_output=pipes[0];   //get output from the child, and ensure that we can save it for next child
            }

            i++;
            free(command); 
        }
    }
}
