#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define TEMP_INPUT_BUFFER 512  
#define INPUT_LENGTH 4094

enum State {
    WHITESPACE, CHARACTER, QUOTE
}; 


//Basic sentence struct to hold our sentence counts and pointer array
struct sentence_struct
{
    int sentence_count;
    char **sentence_array;
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

    
}



int main(int argc, char **argv) {

    LIST_HEAD(list_args);
    char input[INPUT_LENGTH]; 

    fgets(input, INPUT_LENGTH, stdin); 
    //input[strlen(input) - 1] = '\0'; 

    if(argc>0){
        stringExtract(&list_args, input, strlen(input));
        displayList(&list_args);
    }


}
