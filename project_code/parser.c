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
int find_num_subcommands(char input[], int len)
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
void clear_list_argument(struct list_head *list)
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
 * @brief Navigate through list_args and print contents to console. 
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

/**
 * @brief Takes in the arg list, sets the input, output, and type for the subcommand. 
 * Also removes any filesnames or redirects from the list_args so it can be processed 
 * and made into an array for exec. 
 * 
 * @param arg The list of args from the command line. Will look like "ls", "-l", "\0"
 * @param subcommand The sub command of the commandline that is being "filled" in 
 */
static void get_input_output(struct list_head *arg, struct subcommand *subcommand) {
    struct list_head *curr;  
    argument *entry; 
    //Assigns default values to the struct 
    subcommand->input = strdup("stdin");    
    subcommand->output = strdup("stdout"); 
    subcommand->type = NORMAL; 
    
    for (curr = arg->next; curr != arg; curr = curr->next) {
        entry = list_entry(curr, argument, list); 
        if (entry->token == REDIRECT_OUTPUT_APPEND) {
            subcommand->type = entry->token; 
            entry = list_entry(curr->next, argument, list);
            subcommand->output = strdup(entry->contents); 

            //Delete the current entry target and free from memory
            list_del(&entry->list); 
            free(entry);

            entry = list_entry(curr, argument, list);
            list_del(&entry->list); 
            free(entry);

            //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
            arg=curr;

        } else if (entry->token == REDIRECT_OUTPUT_TRUNCATE) {
            subcommand->type = entry->token; 
            entry = list_entry(curr->next, argument, list);
            printf("1: (%s)\n", entry->contents);
            subcommand->output = strdup(entry->contents); 

            //Delete the current entry target and free from memory
            list_del(&entry->list); 
            free(entry);

            entry = list_entry(curr, argument, list);
            printf("2: (%s)\n", entry->contents);
            list_del(&entry->list); 
            free(entry);

            //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
            arg=curr;
            
        } else if (entry->token == REDIRECT_INPUT) {
            entry = list_entry(curr->next, argument, list);
            printf("Input: (%s)\n", entry->contents);
            subcommand->input = strdup(entry->contents);
            
            //Delete the current entry target and free from memory
            list_del(&entry->list); 
            free(entry);
            
            entry = list_entry(curr, argument, list);
            list_del(&entry->list); 
            free(entry);

            //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
            arg=curr;
        }
    }
}

/**
 * @brief Takes the remaining arguments of list_args and stores them as a 2D array
 * in subcommand. 
 * 
 * @param list_args The list of args that is being placed in an array 
 * @param sub The subcommand of the commandline that the array belongs to
 */
static void make_exec_args_array(struct list_head *list_args, struct subcommand *sub) {
  int num_args = getListLength(list_args); 
  sub->exec_args = malloc(num_args * sizeof(char *)); 
  //loop through args list and assign exec_args[i] the value of contents
  struct list_head *curr;  
  argument *entry; 
  int i = 0; 
    
  for (curr = list_args->next; curr != list_args->prev; curr = curr->next) {
      entry = list_entry(curr, argument, list); 
      sub->exec_args[i] = strndup(entry->contents, strlen(entry->contents)); 
      i++; 
  }
  sub->exec_args[num_args-1] = NULL; 

}

/**
 * @brief Constructs a struct that holds information about the individual subcommand of 
 * from the command line. The subcommand is added to a list of commands, which represents the entire 
 * command line. 
 * 
 * @param list_commands The list which the subcommand will be added to
 * @param list_args The parsed argument list that is being turned into a subcommand. 
 */
static void make_subcommand(struct list_head *list_commands, struct list_head *list_args) {
  struct subcommand *sub = malloc(sizeof(struct subcommand)); 
  get_input_output(list_args, sub); //fills in the struct fields: input, output, type
  make_exec_args_array(list_args, sub); //fills in the struct field: exec_args ==> "ls", "-l", NULL
  list_add_tail(&sub->list, list_commands); 
}


void add_arg_to_list(char *temp, int token, argument *arg, struct list_head *list_args){
  arg = malloc(sizeof(argument)); 
  arg->contents = strdup(temp); // Copy temp to contents
  arg->token = token; // Set token to normal
  list_add_tail(&arg->list, list_args); // Add to the end of the list
  memset(temp, 0, 50);
}


void parse_commandline(struct list_head *list_args, commandline *commandline, struct list_head *list_commands)
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
        if (current_character == REDIR_OUT) {

          if(strlen(temp)>0){
            add_arg_to_list(temp, NORMAL, arg, list_args);
          }

          // If we encounter two redir_in symbols ">>"
          if (commandline->subcommand[i][j + 1] == REDIR_OUT) {
            strncat(temp, &commandline->subcommand[i][j], 2); // Copy symbols to temp
            printf("TOKEN: %s\n", temp);
            add_arg_to_list(temp, REDIRECT_OUTPUT_APPEND, arg, list_args);
            j++;
          } else {
            // Otherwise, we only encountered one redir_in '>'
            strncat(temp, &commandline->subcommand[i][j], 1); // Copy symbol to temp
            add_arg_to_list(temp, REDIRECT_OUTPUT_TRUNCATE, arg, list_args);
          }

          
        } else if (current_character == REDIR_IN) {
          // If we encounter the redirect output symbol

          //If the temp var already has an argument in it prior to redir_in
          if(strlen(temp)>0){
            add_arg_to_list(temp, NORMAL, arg, list_args);
          }
          
          
          strncat(temp, &commandline->subcommand[i][j], 1); // Copy symbol to temp

          //Always add the redir_in to the list
          add_arg_to_list(temp, REDIRECT_INPUT, arg, list_args);
          
        } else {
          // Current char is a character
          currentState = CHARACTER; // Set current state
          strncat(temp, &commandline->subcommand[i][j], 1); // Copy character

          // if we found the last word, and it has no space after, add it to the list
          if (j == (strlen(commandline->subcommand[i]) - 1)) {
            add_arg_to_list(temp, NORMAL, arg, list_args);
          }
        } 
      } else if (current_characters_state == WHITESPACE) {
        // If we see a space or tab
        if (currentState != WHITESPACE) {
          add_arg_to_list(temp, NORMAL, arg, list_args);  //add the current content of temp to the list of args
          currentState = WHITESPACE;
          word_count++;                    //increment which word we are on
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

          add_arg_to_list(temp, NORMAL, arg, list_args);
          currentState = WHITESPACE;
        }
      }
    }
    //The req specify ending the list of arguements with a NULL for exec

    
    arg = malloc(sizeof(argument));
    arg->contents = strdup("\0");
    arg->token = NORMAL;
    list_add_tail(&arg->list, list_args);

    //Makes a subcomamnd, then clears the list_args so that more args can be scanned
    //at this point list_args == "ls" "-l" "\0" 
    display_list(list_args);
    make_subcommand(list_commands, list_args); 
    clear_list_argument(list_args); 
  }
  free(temp);
}



