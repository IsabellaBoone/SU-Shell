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

typedef struct Subcommand
{
  struct list_head list; // part of a list of subcommands
  // struct argument *args; // a pointer to the head of arg_list
  char **exec_args; // equivalent to lsargs in hw3
  // enum command_type command; // internal environment, job internal, normal
} subcommand;


/**
 * Functions related to navigating data structures
 */

/**
 * @brief Find the number of subcommands in the input string and returns that value. 
 * 
 * @param input String to search through
 * @param len int length of String
 * @return int number of subcommands found
 */
int find_num_sentences(char input[], int len);

/**
 * @brief Copy an individual subcommand to a pointer
 * 
 * @param subcommand 2D char array to copy from.
 * @param sentence char * destination to copy to
 * @param i which sentence in subcommand to copy
 */
void copy_subcommand(char **subcommand, char *destination, int i);

/**
 * @brief Copy a String of subcommands into a 2D array of subcommands. 
 * 
 * @param input String to break apart
 * @param num int number of subcommands in String
 * @param subcommand 2D char array to copy subcommands into
 */
void copy_subcommands(char input[], int num, char **subcommand);


//Prints the number of subcommand in input 
/**
 * @brief Print number of subcommands in the input
 * 
 * @param num int number of subcommands
 */
void print_num_subcommands(int num);


//Prints the subcommand in the correct format 
/**
 * @brief Print subcommand in correct form. 
 * 
 * @param num 
 * @param sen 
 */
void print_subcommands(int num, char **sen);

/**
 * @brief Traverse through linked list and free it from memory
 * after deleting each node entry. 
 * 
 * @param list struct list_head to clear
 */
void clear_list(struct list_head *list);

/**
 * @brief Navigate through list and print contents to console. 
 * 
 * @param list struct list_head to print
 */
void displayList(struct list_head *list);


/**
 * Take in the input string and length of the string as parameters.
 * Returns the amount of subcommand in the input string
**/

/**
 * @brief Parse through a list of arguments character by character. 
 * 
 * @param list_args List of arguments to parse through
 * @param commandline 
 */
void stringExtract(struct list_head *list_args, commandline *commandline);

#endif