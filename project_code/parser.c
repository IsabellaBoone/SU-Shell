#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "list.h"

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


struct {
  int num;
  char **sentences;
} sentence_info;

//Finds the number of sentences in the input string and returns that value. 
int find_num_sentences(char input[], int len) {
  int i, count = 1; 

  for (i = 0; i < len; i++) {
    if (input[i] == '|') {
        count++; 
    }
  }

  return count; 
} 

//Copies an individual sentence to a pointer 
void copy_sentence(char **sentences, char *sentence, int i) {
  strcpy(sentences[i], sentence);
}


//Copies all the sentences from input into the array of pointers 
void copy_sentences(char input[], int num, char **sentences) {
  int i, len; 
  char* sentence = strtok(input, "|"); 
 
  len = strlen(sentence); 

  for (i = 0; i < num; i++) {
    len = strlen(sentence);
    sentences[i] = malloc(len + 2);
    copy_sentence(sentences, sentence, i);

    sentence = strtok(NULL, "|"); 
  }
}


//Prints the number of sentences in input 
void print_num_sentences(int num) {
  printf("num: %d\n", num);
}


//Prints the sentences in the correct format 
void print_sentences(int num, char **sen) {
  int i = 0; 
  for (i = 0; i < num; i++) {
    printf("%d : (%s)\n", i, sen[i]); 
  }
}


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
        printf("(%s)\n", entry->contents);
    }

}


/**
 * Take in the input string and length of the string as parameters.
 * Returns the amount of sentences in the input string
**/
void stringExtract(struct list_head *list_args, char *input, int length){
    int word_count = 0;
    int currentState = CHARACTER;

    char *temp = calloc(100, sizeof(char));  // Temporary word variable
    struct argument *arg; // Linked List

    
    for(int i=0; i<length; i++){

        // If we find characters to a word, add them to a temp variable
        if ((input[i] != ' ' && input[i] != '\n' && input[i] != '\t') && (input[i] != '"')) {
            currentState = CHARACTER; // Set current state

            strncat(temp, &input[i], 1); // Copy character to temp word variable

            //if we found the last word, and it has no space after
            //add to the list
            if(i == length-1){
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
    arg->contents = strdup("\0");
    list_add(&arg->list, list_args);

    
    free(temp); 
    
}





