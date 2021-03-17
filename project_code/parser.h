// imports
#include <stdio.h>  // for io
#include <string.h> // for strings
#include <stdlib.h> // for malloc/free
#include "list.h"


/**
 * Struct used to keep track of numbers and sentences
 * and the sentences themselves.
 */
struct arg {
  char* contents; // ls, -a, -l
  struct list_head list; // 
};

void parse(char *argv); 

char** separate_args(char *buffer);

void print_traverse(struct list_head *node);

void clear_list(struct list_head *list);