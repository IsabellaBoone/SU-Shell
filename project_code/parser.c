// imports
#include <stdio.h>  // for io
#include <string.h> // for strings
#include <stdlib.h> // for malloc/free
#include "list.h"
#include "parser.h"

#define MAX 4096 // for max buffer size, 4kb
#define SPACE ' ' 
#define TAB '\t'

enum state {
  WHITESPACE, CHARACTERS
};

char** separate_args(char *buffer) {
  char** args = malloc(strlen(buffer) * sizeof(char)); // Array to store sorted arguments
  int i = 0, j = 0, arg_num = 0; 
  enum state curState = CHARACTERS;

  while (buffer[i] != '\0') {
    if (buffer[i] == SPACE || buffer[i] == TAB) {
      if(!curState == WHITESPACE) {
        ++arg_num; 
        j = 0; 
      } 
    } else {
      curState = CHARACTERS; 
      args[arg_num][j] = buffer[i]; 
      j++; 
    }
    ++i;
  }
  return args;
}


/**
 * @brief Main function for parser 
 * 
 * @param argv array is a list of pointers to argc number of strings
 */
void parse(char *argv) { 
  char** args_array = separate_args(argv); 

  // Declare list
  LIST_HEAD(list_arguments);

  // whatever stackoverflow says
  size_t size_of_args = sizeof(args_array[0]) / sizeof(args_array[0][0]);

  // Parse through our arguments given as parameters 
  for(int i = 1; i < size_of_args; i++) {
    // Allocate space for argument
    struct arg *arg = calloc(MAX, sizeof(arg)); 

    // Set its contents
    arg->contents = strdup(args_array[i]);

    // Set its type and add it to the appropriate list
    if (args_array[i][0] == TAB || args_array[i][0] == SPACE) {
      list_add(&arg->list, &list_arguments);
    } 
  }

  // If each list is not empty, print its contents
  // and clear the lists afterwards. 
  if(!list_empty(&list_arguments)) {
    printf("Arguments with dashes\n"); 
    print_traverse(&list_arguments); 
    clear_list(&list_arguments);
  }
}

void print_traverse(struct list_head *node) {
    struct list_head *curr;  
    struct arg *entry; 
    
    for (curr = node->next; curr != node; curr = curr->next) {
        //entry is the struct connected with the node in the list
        entry = list_entry(curr, struct arg, list); 
        printf("%s\n", entry->contents); 
    }
}

void clear_list(struct list_head *list) {
  struct arg *entry;
  while (! list_empty(list)) {
    entry = list_entry(list->next, struct arg, list);
    list_del(&entry->list);
    free(entry->contents);
    free(entry);
  }
}


