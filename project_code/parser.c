/**
 * @file parser.c
 * @author Hannah Moats 
 * @author John Gable 
 * @author Isabella Boone
 * @brief Parses the command line and fills in the list_commands which is a list 
 * of subcommands. Each subcommand, contains info on input and output, and a 2D array. 
 * @version 0.1
 * @date 2021-03-28
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "list.h"
#include "datastructures.h"
#include "internal.h"
#include "error.h"

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
 * @author Hannah Moats 
 * 
 * @param input String to search through
 * @param len int length of String
 * @return int number of subcommands found
 */
int find_num_subcommands(char input[], int len)
{
  int count = 1; // Number of subcommands counted 
  for (int i = 0; i < len; i++) // For every char in the string
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
 * @author Hannah Moats  
 * 
 * @param subcommand 2D char array to copy from.
 * @param sentence char * destination to copy to
 * @param i which sentence in subcommand to copy
 */
void copy_subcommand(char **subcommand, char *destination, int i)
{
  strcpy(subcommand[i], destination); // Copy subcommand to destination
}

/**
 * @brief Copy a String of subcommands into a 2D array of subcommands. 
 * @author Hannah Moats 
 * 
 * @param input String to break apart
 * @param num int number of subcommands in String
 * @param subcommand 2D char array to copy subcommands into
 */
void copy_subcommands(char input[], int num, char **subcommand)
{
  // If is a newline
  if(input[0]!=NEWLINE){ 
    int i, len;
    char *cmd = strtok(input, "|"); // Take the command before the pipe 

    len = strlen(cmd); // Get length of command 

    for (i = 0; i < num; i++)
    {
      len = strlen(cmd);
      subcommand[i] = malloc(len + 2); // ALlcoate space 
      copy_subcommand(subcommand, cmd, i); // Copy subcommand

      cmd = strtok(NULL, "|"); 
    }
  }
}

/**
 * @brief Print number of subcommands in the input
 * @author Hannah Moats 
 * 
 * @param num int number of subcommands
 */
void print_num_subcommands(int num)
{
  printf("num: %d\n", num); // Print number of subcommands
}

/**
 * @brief Print subcommand in correct form. 
 * 
 * @param num number of commands in subcommands
 * @param subcommands 2D char array of subcommands to print 
 */
void print_subcommands(int num, char **subcommands)
{
  for (int i = 0; i < num; i++)
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
  argument *entry; //Current entry during traversal

  // While list is not empty
  while (!list_empty(list))
  {
    // Delete entry from list and free its contents and itself.
    entry = list_entry(list->next, argument, list);
    list_del(&entry->list);
    free(entry->contents);
    free(entry);
  }
}

/**
 * @brief Navigate through list_args and print contents to console. 
 * @author John Gable 
 * 
 * @param list struct list_head to print
 */
void display_list(struct list_head *list)
{
  struct list_head *start = list->next; // Start at the first node after the head
  struct list_head *curr; // Tracks current node during traversal
  argument *entry; // Current nodes struct with contents

  // Iterate through list until we reach the beginning node again. 
  for (curr = list->next; curr->next != start; curr = curr->next)
  {
    entry = list_entry(curr, argument, list); // Update entry 
    printf("(%s), (%d)\n", entry->contents, entry->token); // Print
  }
}

/**
 * @brief Check whether or not a char is considered whitespace.
 * 
 * @param c char to check
 * @return int 1 if it is considered whitespace, 0 if it is not. 
 */
int is_whitespace(char c){
  if(c == SPACE || c == TAB){
    return 1;
  }
  return 0;
}

/**
 * @brief Check whether or not a char is considered a quote. 
 * 
 * @param c char to check. 
 * @return int 1 if it is considered a quote, 0 if it is not. 
 */
int is_quote(char c){
  if(c == QUOTATIONMARK){
    return 1;
  }
  return 0;
}

/**
 * @brief Check whether or not a char is considered a redirect symbol. 
 * 
 * @param c char to check.
 * @return int 1 if it is considered a redirect symbol, 0 if it is not. 
 */
int is_redir(char c){
  if(c == REDIR_IN || c == REDIR_OUT ){
    return 1;
  }
  return 0;
}

/**
 * @brief Checks whether or not a char is considered a character. 
 * 
 * @param c char to check
 * @return int 1 if it is considered character, 0 if it is not. 
 */
int is_character(char c){
  if(!is_redir(c) && !is_whitespace(c) && !is_quote(c)){
    return 1;
  }

  return 0;
}

/**
 * @brief Check character state of a char.  
 * 
 * @param c char to check. 
 * @return int return 1: whitespace, 2: character, 3: quote, 5:redir
 */
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
  // Assign default values to the struct 
  subcommand->input = strdup("stdin");    
  subcommand->output = strdup("stdout"); 
  subcommand->type = NORMAL; 
    
  // Iterate through list until we reach the beginning again.
  for (curr = arg->next; curr != arg; curr = curr->next) {
    entry = list_entry(curr, argument, list); // Update entry 
    // If token is redirect append
    if (entry->token == REDIRECT_OUTPUT_APPEND) {
      subcommand->type = entry->token; // Make subcommand token match
      entry = list_entry(curr->next, argument, list);
      subcommand->output = strdup(entry->contents); // Make subcommand output match

      //Delete the current entry target and free from memory
      list_del(&entry->list); 
      free(entry);

      entry = list_entry(curr, argument, list);
      list_del(&entry->list); 
      free(entry);

      //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
      arg=curr;

    // If token is redirect truncate
    } else if (entry->token == REDIRECT_OUTPUT_TRUNCATE) {
      subcommand->type = entry->token;
      entry = list_entry(curr->next, argument, list);
      //printf("1: (%s)\n", entry->contents);
      subcommand->output = strdup(entry->contents);

      //Delete the current entry target and free from memory
      list_del(&entry->list); 
      free(entry);

      entry = list_entry(curr, argument, list);
      //printf("2: (%s)\n", entry->contents);
      list_del(&entry->list); 
      free(entry);

      //Adjust start in order to satisfy the loop condition incase its referenced node was deleted  
      arg=curr;

      // If token is redirect input      
    } else if (entry->token == REDIRECT_INPUT) {
      entry = list_entry(curr->next, argument, list); // Update entry
      //printf("Input: (%s)\n", entry->contents);
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
  // Loop through args list and assign exec_args[i] the value of contents
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
 * @brief Checks to see if the command is an internal command
 * 
 * @param arg1 The name of the command
 * @return int Returns 0 if the command is an internal command, else 1 if it is not
 */
int check_internal_command(struct list_head *list_args) {
  char *internal_cmds[] = {"setenv", "getenv", "unsetenv", "cd", "pwd", "exit"};
  argument *entry = list_entry(list_args->next, argument, list); 
  int i; 
  for (i = 0; i < 6; i++) {
    if (strcmp(internal_cmds[i], entry->contents) == 0) {
      return 0; 
    }
  }
  return 1; 
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

  if(check_internal_command(list_args)){
      get_input_output(list_args, sub); //fills in the struct fields: input, output, type
  } else {
    sub->input = strdup("stdin"); 
    sub->output = strdup("stdout"); 
  }
  make_exec_args_array(list_args, sub); //fills in the struct field: exec_args ==> "ls", "-l", NULL
  list_add_tail(&sub->list, list_commands); 
  
}


void add_arg_to_list(char *temp, int token, argument *arg, struct list_head *list_args, int *space_count){
  arg = malloc(sizeof(argument)); 
  arg->contents = strdup(temp); // Copy temp to contents
  arg->token = token; // Set token to normal
  list_add_tail(&arg->list, list_args); // Add to the end of the list
  memset(temp, 0, 50);
  *space_count = 0; 
}

/**
 * @brief Ensures that the commandline appropiately uses the redirect operators. 
 * @author Hannah Moats 
 * 
 * @param args The current commandline argument being evaluated 
 * @param total_cmds The total number of subcommands on the commandline
 * @return int Returns -1, if command line error, else returns 0 with no error 
 */
static int check_validity_of_cmdline_redirects(struct list_head *list_args, int total_cmds, int current_cmd, int stdins, int stdouts) {
  if (total_cmds == 1) {
    //can't have two standard outs 
    //can't have more than one standard ins
    if (stdins > 1 || stdouts > 1) {
      fprintf(stderr, ERROR_INVALID_CMDLINE); 
      return -1; 
    }
  } else if (total_cmds > 1 && current_cmd == 1) { 
    if (stdins > 1 || stdouts != 0) {
      fprintf(stderr, ERROR_INVALID_CMDLINE);
      return -1;
    }
  } else if (total_cmds > 1 && current_cmd < total_cmds) {
    if (stdins != 0 || stdouts != 0) {
      fprintf(stderr, ERROR_INVALID_CMDLINE);
      return -1; 
    }
  } else if (total_cmds > 1 && current_cmd == total_cmds) {
    if (stdins != 0 || stdouts > 1) {
      fprintf(stderr, ERROR_INVALID_CMDLINE);
      return -1; 
    }
  }
  return 0; 
}

/**
 * @brief Frees the argument list used to store the parsed argument values. 
 * Frees the temp value used in scanning the arguments. 
 * 
 * @param list_args The list of parsed arguments. 
 * @param temp The temporary buffer. 
 */
void free_malloced_parser_values(struct list_head *list_args, char *temp) {
  clear_list_argument(list_args); 
  free(temp);
  return; 
}

/**
 * @brief Checks to ensure that the second character after the redirect is not a space. 
 * If it is a space then there is an error. 
 * 
 * @param list_args The list of arguments being freed, if error
 * @param temp The char pointer being freed, if error
 * @param check_char The character that is being checked.
 * @return int Returns -1 on error, else returns 0
 */
static int space_check_after_redirect(struct list_head *list_args, char *temp, char check_char) {
  if (is_whitespace(check_char) == 1) { //check for spacing after the input redir
    fprintf(stderr, ERROR_INVALID_CMDLINE); 
    free_malloced_parser_values(list_args, temp); 
    return -1;
  }
  return 0; 
}

/**
 * @brief Checked the number of spaces before the redirect, should be one. If the
 * number of spaces is greater than one then there is an error.
 * 
 * @author Hannah Moats 
 * @param list_args The list of arguments to be freed if there is an error.
 * @param temp The char pointer, that needs freed if there is an error. 
 * @param space_count The number of spaces, before the redirect. 
 * @return int Returns -1 if there is an error, else returns 0. 
 */
static int space_check_before_redirect(struct list_head *list_args, char *temp, int space_count) {
  if (space_count > 1) { //more than one space around a redirect throw error
    fprintf(stderr, ERROR_INVALID_CMDLINE);
    free_malloced_parser_values(list_args, temp);  
    return -1; 
  }
  return 0; 
}

/**
 * @brief Creates a list of commands, or subcommand structs, where each 
 * subcommand is parsed and marked with the appropiate input and output. 
 * 
 * @param list_args A list used to store the parsed arguments of a subcommand
 * @param commandline The struct which stores unparsed subcommands, and number of subcommands. 
 * @param list_commands The list that stores the subcommands 
 * @return int Returns -1 if there was an error, else returns 0
 */
int parse_commandline(struct list_head *list_args, commandline *commandline, struct list_head *list_commands)
{
  int word_count = 0; //Count for how many words we have parsed out of the commandline sentences
  int currentState = WHITESPACE; // Start in whitespace state by default
  int redirect_in_count = 0; 
  int redirect_out_count = 0; 
  int space_count = 0; 

  char *temp = calloc(100, sizeof(char)); // Temporary word variable
  argument *arg; // Linked List of arguments
  
  // For every subcommand 
  for (int i = 0; i < commandline->num; i++)
  {
    redirect_in_count = 0; 
    redirect_out_count = 0; 
    space_count = -1; //minus 1 since the initial state is whitespace
    // For every character in the subcommand
    for (int j = 0; j < strlen(commandline->subcommand[i]); j++){
      char current_character = commandline->subcommand[i][j]; // Purely for readability's sake
      int current_characters_state = check_character_state(current_character);  //check for the current_characters type

      // If the current char is not a space, tab, or quotation mark
      if (current_characters_state == CHARACTER || current_characters_state == REDIR) {
        // If we encounter redirect symbol
        if (current_character == REDIR_OUT) {
          if (space_check_before_redirect(list_args, temp, space_count) == -1) {
            return -1; 
          }
          if(strlen(temp)>0){
            add_arg_to_list(temp, NORMAL, arg, list_args, &space_count);
          }

          // If we encounter two arrow redir_out symbol ">>"
          if (commandline->subcommand[i][j + 1] == REDIR_OUT) {
            if (space_check_after_redirect(list_args, temp, commandline->subcommand[i][j+3]) == -1) {
              return -1; 
            }
            strncat(temp, &commandline->subcommand[i][j], 2); // Copy symbols to temp
            add_arg_to_list(temp, REDIRECT_OUTPUT_APPEND, arg, list_args, &space_count);
            j++;
            redirect_out_count++; 
          } else {
            if (space_check_after_redirect(list_args, temp, commandline->subcommand[i][j+2]) == -1) {
              return -1; 
            }
            // Otherwise, we only encountered one redir_out '>'
            strncat(temp, &commandline->subcommand[i][j], 1); // Copy symbol to temp
            add_arg_to_list(temp, REDIRECT_OUTPUT_TRUNCATE, arg, list_args, &space_count);
            redirect_out_count++; 
          }
        } else if (current_character == REDIR_IN) {
          // If we encounter the redirect output symbol

          //If the temp var already has an argument in it prior to redir_in
          if(strlen(temp)>0){
            add_arg_to_list(temp, NORMAL, arg, list_args, &space_count);
          }
          
          strncat(temp, &commandline->subcommand[i][j], 1); // Copy symbol to temp 
          if (space_check_before_redirect(list_args, temp, space_count) != -1) { 
            if (space_check_after_redirect(list_args, temp, commandline->subcommand[i][j + 2]) == -1) { 
              return -1; 
            }
            //Always add the redir_in to the list
            add_arg_to_list(temp, REDIRECT_INPUT, arg, list_args, &space_count);
            redirect_in_count++;
          } else { //else invalid spacing 
            return -1; 
          }
          
        } else {
          // Current char is a character
          currentState = CHARACTER; // Set current state
          strncat(temp, &commandline->subcommand[i][j], 1); // Copy character

          // if we found the last word, and it has no space after, add it to the list
          if (j == (strlen(commandline->subcommand[i]) - 1)) {
            add_arg_to_list(temp, NORMAL, arg, list_args, &space_count);
          }
        } 
      } else if (current_characters_state == WHITESPACE) {
        // If we see a space or tab
        space_count++; 
        if (currentState != WHITESPACE) {
          add_arg_to_list(temp, NORMAL, arg, list_args, &space_count);  //add the current content of temp to the list of args
          space_count++; 
          currentState = WHITESPACE;
          word_count++;                    //increment which word we are on
        }
      } else if (current_characters_state == QUOTE) {
        if (currentState != QUOTE) {
          currentState = QUOTE;
          j++;
          while (commandline->subcommand[i][j] != QUOTATIONMARK)
          {
            strncat(temp, &commandline->subcommand[i][j], 1);
            j++;
          }

          add_arg_to_list(temp, NORMAL, arg, list_args, &space_count);
          currentState = WHITESPACE;
        }
      }
    }
    //The req specify ending the list of arguements with a NULL for exec
    
    arg = malloc(sizeof(argument));
    arg->contents = strdup("\0");
    arg->token = NORMAL;
    list_add_tail(&arg->list, list_args);

    int error_check = check_validity_of_cmdline_redirects(list_args, commandline->num, i + 1, redirect_in_count, redirect_out_count); 
    if (error_check == -1) {
      free_malloced_parser_values(list_args, temp); 
      return -1; 
    }
    //Makes a subcomamnd, then clears the list_args so that more args can be scanned
    //at this point list_args == "ls" "-l" "\0" 
    make_subcommand(list_commands, list_args); 
    clear_list_argument(list_args); 
  }
  free(temp);
  return 0; 
}