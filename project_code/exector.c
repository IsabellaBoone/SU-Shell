//May need to remove when we remove the executor stuff 
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "parser.h"
#include "list.h"

/**
 * @brief makes a list of arguments
 * 
 * @param 
 * @param list The list that will be turned into an array of characetr pointers 
 * @return char* Returns a list of arguments in the char **args
 */
void makeArgumentList(struct list_head *list, char **args, int len) {
    struct list_head *curr;  
    struct argument *entry; 
    int i = len - 1; 
    
    for (curr = list->next; curr != list; curr = curr->next) {
        entry = list_entry(curr, struct argument, list); 
        args[i] = strdup(entry->contents); 
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
 */
void free_exec_arg_list(char **args, int len) {
    int i; 
    for (i = 0; i < len; i++) {
        free(args[i]); 
    }
    free(args); 

}

/*****************************************************************************
 * I just decided to put the executor functions in the basic_parser so it'd 
 * be easier to test with the basic parser.
 * basic_executor.c 
 * **************************************************************************/

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

/**
 * @brief Executes the command, given the type of command and the argument array 
 * 
 * @param command The type of command that is being executed, Ex. /bin/ls
 * @param args The array of args that exec() takes in
 */
void execute(char *command, char *const *args) {
    pid_t pid = fork(); 

    if (pid == 0) { //Child process 
        handleChildInExecutor(command, args); 
    } else {  //Parent process 
        handleParentInExecutor(pid, 0); 
    }
}

/**
 * @brief Runs the command typed on the command line 
 * 
 * @param len The length of the linked list 
 * @param list_args The linked list of args that are being executed 
 */
void run_command(int len, struct list_head *list_args) {
    //initializes an array of character pointers that will be passed to exec()
    char **exec_arg_list = malloc(len * sizeof(char *)); 

    //takes the linked list, and turns it into an array list that can be passed to exec
    makeArgumentList(list_args, exec_arg_list, len); 
    char *command = malloc((6 + strlen(exec_arg_list[0])) * sizeof(char));

    // command becomes: /bin/<command> 
    strcpy(command, "/bin/"); 
    strcat(command, exec_arg_list[0]); 

    //executes a basic command
    execute(command, exec_arg_list); 

    //frees the argument list
    free(command);
    free_exec_arg_list(exec_arg_list, len); 
}