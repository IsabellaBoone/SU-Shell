/**
 * @file datastructures.h
 * @author Isabella Boone 
 * @author John Gable
 * @author Hannah Moats
 * @brief This file contains all data structures and enums needed. 
 * @version 0.1
 * @date 2021-03-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// file guard
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

// imports
#include <string.h> // for strings
#include "list.h" // for navigating lists

enum Token
{
  REDIRECT_INPUT,
  REDIRECT_OUTPUT_APPEND,
  REDIRECT_OUTPUT_TRUNCATE,
  NORMAL,
  FILENAME
};


/**
 * Main struct for the linked list.
 * Contains the type of user input(list or dashed), the contents  (the input),
 * and the recursive list.
 */
typedef struct Argument {
    char *contents;
    enum Token token; 
    struct list_head list;
} argument; 

/** Full line that is typed
 */
typedef struct Commandline {
  int num; // Number of subcommands
  char **subcommand; // 2d array of all subcommands
  char **stdin; // would hold where the input of the function comes from 
  char **stdout; // would hold where the command outputs to 
} commandline;

// Functions related to navigating data structures

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
void stringExtract(struct list_head *list_args, commandline *commandline);

#endif