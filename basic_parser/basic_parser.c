#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#define TEMP_INPUT_BUFFER 512  

enum State {
    WHITESPACE, CHARACTER 
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
 * Recurses through a given list_head struct's list,
 * and prints the contents to the console.
 */
void displayList(struct list_head todo_list){
    struct list_head *start=todo_list.next; //Start at the first node after the head
    struct list_head *curr; //Tracks current node during traversal
    struct argument *entry; //Current nodes struct with contents

    for (curr = todo_list.next; curr->next != start; curr = curr->next) {
        entry = list_entry(curr, struct argument, list);
        printf("%s\n", entry->contents);
    }

}


/**
 * Take in the input string and length of the string as parameters.
 * Returns the amount of sentences in the input string
**/
void stringExtract(struct list_head list_args, char *input, int length){
    int word_count = 0;
    int currentState = CHARACTER;

    struct sentence_struct s_struct;
    char *temp = calloc(50, sizeof(char));
    //s_struct.sentence_array = calloc(10, sizeof(char*));
    struct argument *arg = malloc(sizeof(struct argument));
    int tempLocation = 0;

    for(int i=0; i<length; i++){
        if (input[i] != ' ' && input[i] != '"'){
            currentState = CHARACTER;
            temp[tempLocation] = input[i];
            tempLocation++;
        }else if (input[i] == ' ' || input[i] == '\t'){
            if(currentState!=WHITESPACE){
                //s_struct.sentence_array[word_count] = calloc(strlen(temp)+1, sizeof(char));
                //strncpy(s_struct.sentence_array[word_count], temp, 15);
                //printf("%s", s_struct.sentence_array[word_count]);
                arg->contents = strdup(temp);
                list_add(&arg->list, &list_args);
                currentState = WHITESPACE;
                word_count++;
                tempLocation=0;
                memset(temp, 0, 50);
            }
        
        }
    }
    
}



int main(int argc, char **argv) {

    LIST_HEAD(list_args);

    if(argc>0){
        stringExtract(list_args, argv[1], strlen(argv[1]));
        displayList(list_args);
    }

}
