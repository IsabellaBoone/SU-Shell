/**
 * @file executor.c
 * @author Hannah Moats 
 * @author John Gable 
 * @author Isabella Boone
 * @brief Handles the execution of the commands, inlcuding pipes
 * @version 0.1
 * @date 2021-03-28
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#define _GNU_SOURCE
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h> // for memory allocation
#include <stdio.h> // for input/output

#include "executor.h"
#include "error.h"

/**
 * @brief Handles the execution of the child process 
 * @author Hannah Moats 
 * @param command char* Type of command being executed 
 * @param args char* const The list of args sent to exec 
 * @param env char** array of environment variables
 */
void handleChildInExecutor(char *command, char *const *args, char **env) {
  pid_t pid2 = execvpe(command, args, env); // Execute command 
  fprintf(stderr, ERROR_EXEC_FAILED, strerror(errno)); // Should only be reaches when error happens with execvpe
  exit(-1); // Exit with error 
}

/**
 * @brief Handles the execution of the parent process 
 * @author Hannah Moats
 * @param pid The process id
 * @param option The option passed to waitpid 
 */
void handleParentInExecutor(pid_t pid, int option) {
  int status; 
  waitpid(pid, &status, option); // Wait for child to die
  // printf("Child exited: %d\n", status);
}

/**
 * @brief Handles getting input from a file, outputting to a file for commands. 
 *
 * @param subcmd The command who's input and output is being handled. 
 * @return int Returns -1 for error, 0 no error
 */
static int handle_input_output(struct subcommand *subcmd) {
  // If subcmd->output is anything other than stdout (default)
  if (strcmp(subcmd->output, "stdout") != 0) {
    const char *filename = subcmd->output; // Get filename
    int fd = 0; // File descriptor 

    // If type is truncate, open file with trucate flags
    if (subcmd->type == REDIRECT_OUTPUT_TRUNCATE) {
      fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777); 

    // If type is append, open file with append flags
    } else if (subcmd->type == REDIRECT_OUTPUT_APPEND) {
      fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0777); 
    }
    close(STDOUT_FILENO); // Close STDOUT
    dup2(fd, STDOUT_FILENO); // Connect FD and STDOUT 

  // If subcmd->input is anything other than "stdin" (default) 
  } if (strcmp(subcmd->input, "stdin") != 0) {
    const char *filename = subcmd->input; // Get filename
    int fd = open(filename, O_RDONLY); // File descriptor, open file read only
    if (fd == -1) {
        fprintf(stderr, ERROR_EXEC_INFILE, strerror(errno));  
        return -1; 
    } else {
      close(STDIN_FILENO); // Close STDIN
      dup2(fd, STDIN_FILENO); // Connect FD and STDIN
    }
  }
  return 0; 
}

/**
 * @brief Executes the command, given the type of command and the argument array 
 * 
 * @param command The type of command that is being executed, Ex. /bin/ls
 * @param args The array of args that exec() takes in
 */
void execute(char *command, char *const *args, struct subcommand *subcmd, char **env) {
  pid_t pid = fork(); 

  if (pid == 0) { // Child process 
    // if there is output
   if ( handle_input_output(subcmd) != -1) {
      handleChildInExecutor(command, subcmd->exec_args, env);
    }
  } else { // Parent process 
    handleParentInExecutor(pid, 0); 
  }

}



/**
 * @brief Runs the command typed on the command line including pipes
 * 
 * @param len The length of the linked listssss
 * @param list_args The linked list of args that are being executed 
 */
void run_command(int subcommand_count, struct list_head *list_commands, char **env) {
  struct subcommand *entry; 
  struct list_head *curr;  

  // If we have more than one subcommand
  
  if(subcommand_count>=2){
    int i=0; // Keep track of what subcommand we are on
    int prev_output=0; // We need this to hold the child output for our loop
    int pipes[2]; // Standard pipes
    
    // Initializes an array of character pointers that will be passed to exec()
    // char **exec_arg_list = malloc(new_length * sizeof(char *)); 
    
    // Iterate through entire list until we reach the beginnign again. 
    for (curr = list_commands->next; curr != list_commands; curr = curr->next) {
      entry = list_entry(curr, struct subcommand, list); // Update entry to look at current subcommand
      char *command = calloc((1 + strlen(entry->exec_args[0])), sizeof(char)); // Allcoate space for command

      // command becomes: <command> 
      strcat(command, entry->exec_args[0]); 

      // make pipes
      if(i<subcommand_count-1){
        int pipe_code = pipe(pipes);

        // If there was an error creating pipes
        if(pipe_code < 0){
          perror("Could not create pipes.\n");
          return;
        }
      }
      
    
      pid_t pid = fork(); 

      if (pid == 0) { // Child process 
        if(i<subcommand_count-1){ // All children except the last must write to parent
          dup2(prev_output, STDIN_FILENO); // read prev_output set by parent (initially empty)
          dup2(pipes[1], STDOUT_FILENO); // write output back to parent
          close(prev_output);
          close(pipes[0]);
          close(pipes[1]);
        }else{ // The last child doesnt output to the parent for the loop 
          dup2(prev_output, STDIN_FILENO); // Read final pipe from parent
          close(prev_output);
          close(pipes[0]);
          close(pipes[1]);
        }
                 
        if ( handle_input_output(entry) != -1) {
          handleChildInExecutor(command, entry->exec_args, env);
          close(prev_output);
          close(pipes[0]);
          close(pipes[1]);
        }
                
      } else { // Parent process
        prev_output=pipes[0]; //get output from the child, and ensure that we can save it for next child
         
        if(i==subcommand_count-1){
          close(prev_output);
          close(pipes[0]);
          close(pipes[1]); 
        }
        
        int status;
        waitpid(pid, &status, 0);
      }

      i++;
      free(command); 
    }
  }else{ // Else, if we only have one command
    curr = list_commands->next; 
    entry = list_entry(curr, struct subcommand, list); // Update entry to look at current subcommand
    char *command = calloc((1 + strlen(entry->exec_args[0])), sizeof(char));

    strcat(command, entry->exec_args[0]); // command becomes: /bin/<command> 
    execute(command, entry->exec_args, entry, env); // Execute command
    free(command); 

  }
}
