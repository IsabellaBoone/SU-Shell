/**
 * @file sush.c
 * @author Isabella Boone 
 * @author John Gable 
 * @author Hannah Moats
 * @brief 
 * @version 0.1
 * @date 2021-03-16
 */

// Imports
#include <stdio.h> // for I/O
#include <stdlib.h> // for memory allocation

// Imports from our files
#include "runner.h"

#define INPUT_LENGTH 4094 // Max input length for strings

/**
 * @brief Project 2: Shell Project 
 * 
 * @return int 
 */
int main(int argc, char **argv, char **envp) {
  commandline cmdline;
  LIST_HEAD(list_args); //TODO should probably put this in parser.c and have it visible only there 
  LIST_HEAD(list_commands); // a list of subcommand structs, represents the comamndline
  LIST_HEAD(list_env); // List of environment variables

  char input[INPUT_LENGTH]; 
  make_env_list(&list_env, envp); //creates a linked list of environment variables
  run_rc_file(&list_commands, &list_env, &list_args,  cmdline, input);

  //checks if PS1 is set, if it is then there is no need to set the environment
  if (get_env(&list_env, "PS1") == NULL) {
    set_env(&list_env, "PS1", ">"); 
  }

  //scan for user input
  run_user_input(&list_commands, &list_env, &list_args, cmdline, input, argc); 
  clear_list_env(&list_env); 
}

