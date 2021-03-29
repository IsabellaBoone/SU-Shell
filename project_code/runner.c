/**
 * @file runner.c
 * @author Isabella Boone 
 * @author John Gable
 * @author Hannah Moats
 * @brief Handle running user input and .sushrc file. 
 * @version 0.1
 * @date 2021-03-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h> // for i/o
#include <stdlib.h> // for memory allocation
#include <sys/stat.h> // for stat system call

// Imports from our files
#include "runner.h"

/**
 * @brief Clear a list of commands. 
 * 
 * @param list list_head to be cleared. 
 */
void clear_list_command(struct list_head *list) {
  struct subcommand *entry; 
  while (!list_empty(list)) {
    entry = list_entry(list->next, struct subcommand, list);
    
    // Free 2D array exec_args
    int i = 0; // Count where we are in array
    while (entry->exec_args[i] != NULL) {
      free(entry->exec_args[i]); // Free array elements
      i++; 
    }
    // Free entry and everything inside it
    free(entry->exec_args); 
    free(entry->input); 
    free(entry->output); 
    list_del(&entry->list); 
    free(entry); 
  }
}

/**
 * @brief Free the commandline struct. 
 * 
 * @param cmdline commandline to be freed.
 */
void free_commandline_struct(commandline cmdline) {
  // Free everything inside commandline
  for(int i=0;i<cmdline.num; i++){
    free(cmdline.subcommand[i]);
  }
  free(cmdline.subcommand); // Free commandline
}

/**
 * @brief Free everything on exit. 
 * 
 * @param list_commands list_head list of commands to be cleared. 
 * @param list_env list_head list of environment variables to be cleared. 
 * @param cmdline commandline to be cleared. 
 */
void freeing_on_exit(struct list_head *list_commands, struct list_head *list_env, commandline cmdline) {
  clear_list_command(list_commands); // Clear command
  clear_list_env(list_env); // Clear environments
  free_commandline_struct(cmdline); // Free commandline
}


/**
 * @brief Checks to ensure that SUSHHOME environment variable has been set. 
 * 
 * @param list_env list_head to get SUSHHOME environment variable from. 
 * @return int if set, 1, else, 0. 
 */
int sushhome_exists(struct list_head *list_env){
  char* sushhome = get_env(list_env, "SUSHHOME"); // Get environment variable
  if(sushhome != NULL){
    return 1; 
  }
  return 0;
}

/**
 * @brief Run the .sushrc file. 
 * 
 * @param list_commands list_head List of commands
 * @param list_env list_head list of environment variables
 * @param list_args list_head list of arguments
 * @param cmdline commandline entry
 * @param input char* input 
 */
void run_rc_file(struct list_head *list_commands, struct list_head *list_env, struct list_head *list_args, commandline cmdline, char *input) {
  // set_env(list_env, "SUSHHOME", "input.txt");

  if(sushhome_exists(list_env)){
    struct stat sb; // Keep track of information regarding the .sushrc file
    char* sushhome = get_env_value(list_env, "SUSHHOME"); // Get location to look for sushrc
    int stat_status = stat(sushhome, &sb); 
    if ((sb.st_mode & S_IRUSR) || (sb.st_mode & S_IXUSR)) { // if true file is valid, read from file
      FILE *file = fopen(sushhome, "r"); // open .sushrc and read from it
      // read from file and execute commands 
      while (fgets(input, INPUT_LENGTH-1, file)!=NULL) {
        int len = strlen(input); 
        input[len-1] = '\0';
        cmdline.num = find_num_subcommands(input, len);
        //creates an array of pointers, in proportion to the number of subcommands
        cmdline.subcommand = malloc(cmdline.num *  sizeof(char *)); 
        copy_subcommands(input, cmdline.num, cmdline.subcommand);
        parse_commandline(list_args, &cmdline, list_commands); 

        int internal_code = handle_internal(list_commands, list_env);
        if(internal_code == 1) {
          char **new_envp = make_env_array(list_env); 
          run_command(cmdline.num, list_commands, new_envp);
          int list_len = getListLength(list_env); 
          free_env_array(new_envp, list_len);  
        } else if (internal_code == 6) { //TODO: is this alright?
          freeing_on_exit(list_commands, list_env, cmdline);
          exit(0);
        }
                
        free_commandline_struct(cmdline);   
        clear_list_command(list_commands);  
      } 
      int flcose_status = fclose(file); 
      //should probably check the status 
    }
  }
}

/**
 * @brief Run the user input. 
 * 
 * @param list_commands list_head list of commands
 * @param list_env list_head of environment variables
 * @param list_args list_head list of arguments
 * @param cmdline commandline full line of input from user
 * @param input char* input to run
 * @param argc int num args
 */
void run_user_input(struct list_head *list_commands, struct list_head *list_env, struct list_head *list_args, commandline cmdline, char *input, int argc) {
  while(1) {
    fgets(input, INPUT_LENGTH, stdin);
    if(input[0] != '\n'){
      //printf("%s", get_env(list_env, "PS1")); 
      printf("%s", get_env_value(list_env, "PS1")); 
      fflush(stdout);
      int len = strlen(input); 
      input[len-1] = '\0';

      cmdline.num = find_num_subcommands(input, len);
            
      // Creates an array of pointers, in proportion to the number of subcommands
      cmdline.subcommand = malloc(cmdline.num *  sizeof(char *)); 
      copy_subcommands(input, cmdline.num, cmdline.subcommand);
      int valid_cmdlin = parse_commandline(list_args, &cmdline, list_commands);
            
      if (valid_cmdlin == 0) { // If there were no errors when parsing 
        //Checks if an internal command, if it is then it is run, else a normal command is run
        int internal_code = handle_internal(list_commands, list_env);
        if(internal_code == 1) {
          char **new_envp = make_env_array(list_env); 
          run_command(cmdline.num, list_commands, new_envp);
          int list_len = getListLength(list_env); 
          free_env_array(new_envp, list_len);  
        } else if (internal_code == 6) { // If internal code was to exit
          freeing_on_exit(list_commands, list_env, cmdline);
          exit(0);
        }
      }

      // Free what we no longer need
      free_commandline_struct(cmdline);
      clear_list_command(list_commands); 

      //printf("%s", get_env(list_env, "PS1")); 
      printf("%s", get_env_value(list_env, "PS1")); 
      fflush(stdout);
    }
  }
}

//void run_file_input();