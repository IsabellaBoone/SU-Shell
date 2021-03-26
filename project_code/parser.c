#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "list.h"
#include "datastructures.h"

#define MAX_BUFFER 4096

#define SPACE ' '
#define TAB '\t'
#define NEWLINE '\n'
#define QUOTATIONMARK '"'
#define PIPE '|'
#define REDIR_IN '<'
#define REDIR_OUT '>'


/**
 * @brief Current state on parser 
 */
enum State
{
  START,
  WHITESPACE,
  CHARACTER,
  QUOTE,
  OUTPUT,
  REDIR,
  INPUT
};

/**
 * @brief Find the number of subcommands in the input string and returns that value. 
 * 
 * @param input String to search through
 * @param len int length of String
 * @return int number of subcommands found
 */
int find_num_sentences(char input[], int len)
{
  int i, count = 1;

  for (i = 0; i < len; i++)
  {
    if (input[i] == PIPE)
    {
      count++;
    }
  }

  return count;
}

/**
 * @brief Copy an individual subcommand to a pointer
 * 
 * @param subcommand 2D char array to copy from.
 * @param sentence char * destination to copy to
 * @param i which sentence in subcommand to copy
 */
void copy_subcommand(char **subcommand, char *destination, int i)
{
  strcpy(subcommand[i], destination);
}

/**
 * @brief Copy a String of subcommands into a 2D array of subcommands. 
 * 
 * @param input String to break apart
 * @param num int number of subcommands in String
 * @param subcommand 2D char array to copy subcommands into
 */
void copy_subcommands(char input[], int num, char **subcommand)
{
  int i, len;
  char *sentence = strtok(input, "|");

  len = strlen(sentence);

  for (i = 0; i < num; i++)
  {
    len = strlen(sentence);
    subcommand[i] = malloc(len + 2);
    copy_subcommand(subcommand, sentence, i);

    sentence = strtok(NULL, "|");
  }
}

/**
 * @brief Print number of subcommands in the input
 * 
 * @param num int number of subcommands
 */
void print_num_subcommands(int num)
{
  printf("num: %d\n", num);
}

/**
 * @brief Print subcommand in correct form. 
 * 
 * @param num number of commands in subcommands
 * @param subcommands 2D char array of subcommands to print 
 */
void print_subcommands(int num, char **subcommands)
{
  int i = 0;
  for (i = 0; i < num; i++)
  {
    printf("%d : (%s)\n", i, subcommands[i]);
  }
}

/**
 * @brief Traverse through linked list and free it from memory
 * after deleting each node entry. 
 * 
 * @param list struct list_head to clear
 */
void clear_list(struct list_head *list)
{
  argument *entry; //Current entry  during traversal

  while (!list_empty(list))
  {
    entry = list_entry(list->next, argument, list);
    list_del(&entry->list);
    free(entry->contents);
    free(entry);
  }
}

/**
 * @brief Navigate through list and print contents to console. 
 * 
 * @param list struct list_head to print
 */
void display_list(struct list_head *list)
{
  struct list_head *start = list->next; //Start at the first node after the head
  struct list_head *curr; //Tracks current node during traversal
  argument *entry; //Current nodes struct with contents

  for (curr = list->next; curr->next != start; curr = curr->next)
  {
    entry = list_entry(curr, argument, list);
    printf("(%s), (%d)\n", entry->contents, entry->token);
  }
}




int is_whitespace(char c){
  if(c == SPACE || c == TAB){
    return 1;
  }

  return 0;
}


int is_quote(char c){
  if(c == QUOTATIONMARK){
    return 1;
  }

  return 0;
}

int is_redir(char c){
  if(c == REDIR_IN || c == REDIR_OUT ){
    return 1;
  }

  return 0;
}


/**
 * Receives an individual character, and checks to ensure
 * the character is not a space, tab, or quote.
 * 
 * If it is not, return 1 for true (is a char),
 * else return 0 (not a char).
**/
int is_character(char c){
  if(!is_redir(c) && !is_whitespace(c) && !is_quote(c)){
    printf("%c\n", c);
    return 1;
  }

  return 0;
}


int check_character_state(char c){
  if(is_character(c)){
    return CHARACTER;
  }else if(is_whitespace(c)){
    return WHITESPACE;
  }else if(is_quote(c)){
    return QUOTE;
  }else if(is_redir(c)){
    return REDIR;
  }
}



void parse_commandline(struct list_head *list_args, commandline *commandline)
{
  int word_count = 0; //Count for how many words we have parsed out of the commandline sentences
  int currentState = WHITESPACE; // Start in whitespace state by default

  char *temp = calloc(100, sizeof(char)); // Temporary word variable
  argument *arg; // Linked List of arguments
  
  // For every subcommand 
  for (int i = 0; i < commandline->num; i++)
  {
    // For every character in the subcommand
    for (int j = 0; j < strlen(commandline->subcommand[i]); j++){
      char current_character = commandline->subcommand[i][j]; // Purely for readability's sake
      int current_characters_state = check_character_state(current_character);  //check for the current_characters type

      // If the current char is not a space, tab, or quotation mark
      if (current_characters_state == CHARACTER || current_characters_state == REDIR) {
        arg = malloc(sizeof(argument)); 
        // If we encounter redirect symbol
        if (current_character == REDIR_IN) {
          // If we encounter two redir_in symbols ">>"
          if (commandline->subcommand[i][j + 1] == REDIR_IN) {
            arg->token = REDIRECT_OUTPUT_APPEND; // Set token to redirect + append
            strncat(temp, &commandline->subcommand[i][j], 2); // Copy symbols to temp
          } else {
            // Otherwise, we only encountered one redir_in '>'
            arg->token = REDIRECT_OUTPUT_TRUNCATE; // Set token to redirect + truncate
            strncat(temp, &commandline->subcommand[i][j], 1); // Copy symbol to temp
          }

          arg->contents = strdup(temp); // Store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); // Add the argument to the list of args
          memset(temp, 0, 50); // Clear memory

          // Increment J so we don't run this again
          if (arg->token == REDIRECT_OUTPUT_APPEND) {
            j++;
          }
        } else if (current_character == REDIR_OUT) {
          // If we encounter the redirect output symbol
          arg = malloc(sizeof(argument)); 
          arg->token = REDIRECT_INPUT; // Set token to redirect input 
          strncat(temp, &commandline->subcommand[i][j], 1); // Copy symbol to temp
          arg->contents = strdup(temp); // Store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); // Add the argument to the list of args
          memset(temp, 0, 50); // Clear memory
        } else {
          // Current char is a character
          currentState = CHARACTER; // Set current state
          strncat(temp, &commandline->subcommand[i][j], 1); // Copy character

          // if we found the last word, and it has no space after, add it to the list
          if (j == (strlen(commandline->subcommand[i]) - 1)) {
            arg = malloc(sizeof(argument)); 
            arg->contents = strdup(temp); // Copy temp to contents
            arg->token = NORMAL; // Set token to normal
            list_add_tail(&arg->list, list_args); // Add to the end of the list
            memset(temp, 0, 50);
          }
        } 
      } else if (current_characters_state == WHITESPACE) {
        // If we see a space or tab
        if (currentState != WHITESPACE) {
          currentState = WHITESPACE;
          arg = malloc(sizeof(argument));
          arg->token = NORMAL;             // set token to normal
          arg->contents = strdup(temp);    //store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); //add the argument to the list of args
          word_count++;                    //increment which word we are on
          memset(temp, 0, 50);             //reset the temp word variable to blank
        }
      } else if (current_characters_state == QUOTE) {
        if (currentState != QUOTE) {
          currentState = QUOTE;
          j++;
          while (current_character != QUOTATIONMARK)
          {
            strncat(temp, &commandline->subcommand[i][j], 1);
            j++;
          }
          arg->token = NORMAL; // set token to normal
          arg = malloc(sizeof(argument));
          arg->contents = strdup(temp);    //store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); //add the argument to the list of args
          memset(temp, 0, 50);             //reset the temp word variable to blank
          currentState = WHITESPACE;
        }
      }
    }
    //The req specify ending the list of arguements with a NULL for exec
    arg = malloc(sizeof(argument));
    arg->contents = strdup("\0");
    arg->token = NORMAL;
    list_add_tail(&arg->list, list_args);
  }


  free(temp);
}



