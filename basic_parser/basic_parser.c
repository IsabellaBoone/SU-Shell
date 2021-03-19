#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//May need to remove when we remove the executor stuff 
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "list.h"

#define TEMP_INPUT_BUFFER 512  
#define INPUT_LENGTH 4094

enum State {
    WHITESPACE, CHARACTER, QUOTE
}; 

/**
 * Main struct for the linked list.
 * Contains the type of user input(list or dashed), the contents  (the input),
 * and the recursive list.
 */
struct argument {
    char *contents;
    struct list_head list;
};


/**
 * Receives a linked list of our list_head struct.
 * The function will traverse through our linked list and
 * free it from memory after deleting each node entry.
 */
void clear_list(struct list_head *list)
{
    struct argument *entry;    //Current entry  during traversal

    while (! list_empty(list)) {
        entry = list_entry(list->next, struct argument, list);
        list_del(&entry->list);
        free(entry->contents);
        free(entry);
    }

}


/**
 * Recurses through a given list_head struct's list,
 * and prints the contents to the console.
 */
void displayList(struct list_head *todo_list){
    struct list_head *start=todo_list->next; //Start at the first node after the head
    struct list_head *curr; //Tracks current node during traversal
    struct argument *entry; //Current nodes struct with contents

    

    for (curr = todo_list->next; curr->next != start; curr = curr->next) {
        entry = list_entry(curr, struct argument, list);
        printf("%s\n", entry->contents);
    }

}


/**
 * Take in the input string and length of the string as parameters.
 * Returns the amount of sentences in the input string
**/
void stringExtract(struct list_head *list_args, char *input, int length){
    int word_count = 0;
    int currentState = CHARACTER;

    char *temp = calloc(50, sizeof(char));  // Temporary word variable
    struct argument *arg; // Linked List

    int tempLocation = 0;
        
    for(int i=0; i<length; i++){

        // If we find characters to a word, add them to a temp variable
        if ((input[i] != ' ' && input[i] != '\n' && input[i] != '\t') && (input[i] != '"')) {
            currentState = CHARACTER; // Set current state

            strncat(temp, &input[i], 1); // Copy character to temp word variable

            //if we found the last word, and it has no space after
            //add to the list
            if(i == length-2){
                arg = malloc(sizeof(struct argument));
                arg->contents = strdup(temp);
                list_add(&arg->list, list_args);
                memset(temp, 0, 50);
            }
    
        } else if (input[i] == ' ' || input[i] == '\t'){
            if(currentState!=WHITESPACE){
                currentState = WHITESPACE; 
                arg = malloc(sizeof(struct argument));
                arg->contents = strdup(temp); //store the last full word into contents of an argument
                list_add(&arg->list, list_args); //add the argument to the list of args
                word_count++;   //increment which word we are on
                memset(temp, 0, 50);    //reset the temp word variable to blank
            }

        } else if (input[i] == '"') {
            if (currentState != QUOTE) {  //starting quote
                currentState = QUOTE; 
                i++; 
                while (input[i] != '"') {
                    //printf("%c\n", input[i]); 
                    strncat(temp, &input[i], 1);
                    i++;
                }
                arg = malloc(sizeof(struct argument));
                arg->contents = strdup(temp); //store the last full word into contents of an argument
                list_add(&arg->list, list_args); //add the argument to the list of args
                memset(temp, 0, 50);    //reset the temp word variable to blank
                currentState = WHITESPACE;
            }

        }
    }

    //The req specify ending the list of arguements with a NULL for exec
    arg = malloc(sizeof(struct argument));
    arg->contents = strdup("NULL");
    list_add(&arg->list, list_args);

    free(temp); 
    
}

/**
 * @brief Get the length of the list 
 * 
 * @param list 
 * @return int 
 */
int getListLength(struct list_head *list) {
    int count = 0; 
    struct list_head *curr;
    
    for (curr = list->next; curr != list; curr = curr->next) {
        count++; 
    } 
    return count;
}

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
void handleChildInExecutor(char *command, char *const *args) {
    execvp(command, args); 
    perror("The process failed to execute"); //Should we handle an error here if we are unable to 
    exit(-1); 
}

void handleParentInExecutor(pid_t pid, int option) {
    int status; 
    waitpid(pid, &status, option); 
}

void executor(char *command, char *const *args) {
    pid_t pid = fork(); 

    if (pid == 0) { //Child process 
        handleChildInExecutor(command, args); 
    } else {  //Parent process 
        handleParentInExecutor(pid, 0); 
    }
}


int main(int argc, char **argv) {
    LIST_HEAD(list_args); 
    char input[INPUT_LENGTH]; 

    fgets(input, INPUT_LENGTH, stdin); 

    if(argc>0){
        stringExtract(&list_args, input, strlen(input));
        //displayList(&list_args);
    }

    //finds the length of the list, used to allocate space for the array of character pointers 
    int list_len; 
    char *command; 
    list_len = getListLength(&list_args); 

    //initializes an array of character pointers that will be passed to exec()
    char **exec_arg_list = malloc(list_len * sizeof(char *)); 

    //takes the linked list, and turns it into an array list that can be passed to exec
    makeArgumentList(&list_args, exec_arg_list, list_len); 
    command = malloc((5 + strlen(exec_arg_list[0])) * sizeof(char));

    // command becomes: /bin/<command> 
    strcpy(command, "/bin/"); 
    strcat(command, exec_arg_list[0]); 

    //executes a basic command
    executor(command, exec_arg_list); 

    //frees the argument list
    free_exec_arg_list(exec_arg_list, list_len); 

    clear_list(&list_args); 


}
