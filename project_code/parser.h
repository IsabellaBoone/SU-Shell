// imports
#include <stdio.h>  // for io
#include <string.h> // for strings
#include <stdlib.h> // for malloc/free
#include "list.h"

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

/** Full line that is typed
 */
struct commandline_t {
  int num; // Number of subcommands
  char **subcommand; // 2d array of all subcommands
  char **stdin; // would hold where the input of the function comes from 
  char **stdout; // would hold where the command outputs to 
};

//Finds the number of subcommand in the input string and returns that value. 
int find_num_sentences(char input[], int len);

//Copies an individual sub-command to a pointer 
void copy_sentence(char **subcommand, char *sentence, int i);


//Copies all the subcommand from input into the array of pointers 
void copy_sentences(char input[], int num, char **subcommand);


//Prints the number of subcommand in input 
void print_num_sentences(int num);


//Prints the subcommand in the correct format 
void print_sentences(int num, char **sen);


/**
 * Receives a linked list of our list_head struct.
 * The function will traverse through our linked list and
 * free it from memory after deleting each node entry.
 */
void clear_list(struct list_head *list);


/**
 * Recurses through a given list_head struct's list,
 * and prints the contents to the console.
 */
void displayList(struct list_head *todo_list);


/**
 * Take in the input string and length of the string as parameters.
 * Returns the amount of subcommand in the input string
**/
void stringExtract(struct list_head *list_args, struct commandline_t *commandline);

/**
 * @brief Get the length of the list 
 * 
 * @param list 
 * @return int 
 */
int getListLength(struct list_head *list);

/**
 * @brief makes a list of arguments
 * 
 * @param 
 * @param list The list that will be turned into an array of characetr pointers 
 * @return char* Returns a list of arguments in the char **args
 */
void makeArgumentList(struct list_head *list, char **args, int len);