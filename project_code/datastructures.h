/**
 * @file datastructures.h
 * @author Isabella Boone 
 * @author John Gable
 * @author Hannah Moats
 * @brief This file contains all data structures and enums needed 
 *   for holding a command line argument. 
 * @version 0.1
 * @date 2021-03-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// File Guard
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

// Imports
#include <string.h> // for strings
#include "list.h" // for navigating lists

/**
 * @brief Enum to describe what type of argument is held. 
 */
enum Token
{
  REDIRECT_INPUT,
  REDIRECT_OUTPUT_APPEND,
  REDIRECT_OUTPUT_TRUNCATE,
  NORMAL,
  FILENAME
};

/**
 * @brief Struct to hold individual arguments in a subcommand. 
 * 
 * @param contents the input
 * @param Token enum to describe what type of argument we have
 * @param list_head list to navigate through, linking to next and prev argument
 */
typedef struct Argument {
    char *contents; // Contents
    enum Token token; 
    struct list_head list;
} argument; 


/**
 * @brief Commandline - the full line of input from the user
 * 
 * @param num int number of subcommands
 * @param subcommand 2D char array of all subcommands
 * @param stdin 2D char array to hold where input of the function comes from
 * @param stdout 2D char array to hold where the command outputs to
 */
typedef struct Commandline {
  int num; 
  char **subcommand; 
  char **stdin;
  char **stdout; 
} commandline;

/**
 * @brief A subcommand
 * 
 * @param list_head part of a list of subcommands
 * @param exec_args 2D char array of arguments
 */
typedef struct Subcommand
{
  struct list_head list; // part of a list of subcommands
  char **exec_args; // equivalent to lsargs in hw3
  // struct argument *args; // a pointer to the head of arg_list
  // enum command_type command; // internal environment, job internal, normal
} subcommand;

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

/**
 * @brief Print number of subcommands in the input
 * 
 * @param num int number of subcommands
 */
void print_num_subcommands(int num);


/**
 * @brief Print subcommand in correct form. 
 * 
 * @param num number of commands in subcommands
 * @param subcommands 2D char array of subcommands to print 
 */
void print_subcommands(int num, char **subcommands);

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
void display_list(struct list_head *list);

/**
 * @brief Parse through a list of arguments character by character. 
 * 
 * @param list_args List of arguments to parse through
 * @param commandline to be parsed through
 */
void parse_commandline(struct list_head *list_args, commandline *commandline);

#endif