/**
 * @file internal.c
 * @author Isabella Boone 
 * @author Hannah Moats
 * @author John Gable
 * @brief Handle Internal Commands
 * @date 2021-03-22
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "datastructures.h"
#include "list.h"
#include "error.h"
#include "environ.h"

#define BUFFER_SIZE 4096

// Struct for internal commands, used to create a table of commands 
typedef struct internal {
  const char *name; 
  int (*handler)(struct list_head *subcommand); 
} internal_t;  

/**
 * @brief If an error occurred then an error will be printed to the screen
 * with the name of the internal command that caused the error. 
 * 
 * @param status The value returned by a function that is being checked 
 * @param command The command that caused the error 
 */
void check_status(int status, char *command) {
  if (status == -1) {
    fprintf(stderr, ERROR_INVALID_CMD, command); //TODO: am I using the right error, the command isn't wrong, just an error in processing
  }
}

/**
 * @brief Get the first parsed argument that was entered on the command line.
 * This argument should be the name of the internal command that is being called.
 * 
 * @param subcommand The parsed command written on the command line
 * @return char* The name of the internal command (first argument)
 */
char * get_internal_command(struct list_head *subcommand) {
  argument *entry = list_entry(subcommand->next, argument, list); 
  return entry->contents; 
}

/**
 * @brief Get the second parsed argument that was entered on the command line. 
 * This argument should be either a path name, or a name of an environment variable. 
 * 
 * @param subcommand The parsed command written on the command line
 * @return char* The name of an environment variable or path (second argument)
 */
char * get_second_argument(struct list_head *subcommand) {
  argument *entry = list_entry(subcommand->next->next, argument, list); 
  return entry->contents; 
}

/**
 * @brief Get the third parsed argument that was entered on the command line. 
 * This argument should be a value used to set an environment variable. 
 * 
  * @param subcommand The parsed command written on the command line
 * @return char* The value entered by the user (third argument) 
 */
char * get_third_argument(struct list_head *subcommand) {
  argument *entry = list_entry(subcommand->next->next->next, argument, list); 
  return entry->contents; 
}

/**
 * @brief Handles the setenv internal command. The set environment takes a name 
 * and sets the given value to that name. 
 * 
 * @param subcommand A parsed command from the commandline
 * @return int If an error occured, output is -1 else output is 0
 */
static int handle_setenv(struct list_head *subcommand) {
  //checks that the setenv command is valid
  int num_args = getListLength(subcommand); 
  if (num_args != 3 + 1) { //plus 1 since we store a NULL at the end
    fprintf(stderr, ERROR_SETENV_ARG); 
    return -1; 
  }

  //if the command is value set the apropiate environment variable 
  char *name = get_second_argument(subcommand); 
  char *value = get_third_argument(subcommand); 

  //removes the dollar sign on the variable name 
  *name++; 

  int status = setenv(name, value, 1);  //Right now I have it so that this will overwrite an existing value  

  //If the setenv command failed to execute
  check_status(status, "setenv"); 
  return 0; 
}

/**
 * @brief Handles the getenv internal command. The getenv command is used to 
 * either get a specified environment, given by a name. Or to print the entire environment. 
 * 
 * @param subcommand A parsed command from the commandline 
 * @return int If an error occured, output is -1 else output is 0
 */
static int handle_getenv(struct list_head *subcommand) {
  int num_args = getListLength(subcommand); 
  if (num_args == 1 + 1) { //subcommand: getenv
    //returns the entire environment and prints it to std out
    //TODO: print the entire environment, may need to use commands from environ.c
  } else if (num_args == 2 + 1) { //subcommmand: getenv $NAME
    char *name = get_second_argument(subcommand); 
    *name++;
    char *env = getenv(name); 

    //error check
    if (env == NULL) {
      fprintf(stderr, ERROR_GETENV_INVALID, name); 
      return -1; 
    }

    printf("%s\n", env); 
  } else { //error: there ere not enough arguments or too many 
    fprintf(stderr, ERROR_GETENV_ARG);
    return -1; 
  }
  return 0;
}

/**
 * @brief Handles the unsetenv command. The unsetenv command is used to delete the 
 * given name from the environment. 
 * 
 * @param subcommand A parsed command from the commandline
 * @return int If an error occured, output is -1 else output is 0
 */
static int handle_unsetenv(struct list_head *subcommand) {
  int num_args = getListLength(subcommand); 
  if (num_args != 2 + 1) { //plus 1 since we store a NULL at the end
    fprintf(stderr, ERROR_UNSETENV_ARG); 
    return -1; 
  }
  char *name = get_second_argument(subcommand); 
  *name++; 
  int status = unsetenv(name); 
  check_status(status, "unsetenv"); 
  return 0;
}

/**
 * @brief Handles the change directory internal command. The change directory command
 * either sets the current directory to the HOME directory, or changes to a specific 
 * directory given a path. 
 * 
 * @param subcommand A parsed command from the commandline
 * @return int If an error occured, output is -1 else output is 0
 */
static int handle_cd(struct list_head *subcommand) {
  int num_args = getListLength(subcommand); 
  if (num_args == 1 + 1) { //subcommand: cd
    char *home = getenv("HOME"); //get home env variable 
    int status = chdir(home); 

    //check for error
    //TODO: should this check actually be applied to the return of getenv?
    if (status == -1) {
      fprintf(stderr, ERROR_CD_NOHOME); 
      return -1; 
    }
  } else if (num_args == 2 + 1) { //subcommand: cd pathname
    char *path = get_second_argument(subcommand); 
    int status = chdir(path); 

    check_status(status, "cd"); 

  } else { //too many args 
    fprintf(stderr, ERROR_CD_ARG); 
    return -1;    
  }

    return 0;

}

/**
 * @brief Handles the print working directory internal command. The print working direcotry
 * command calls getcwd() system call and is used to print the current directory
 * that the user is in. 
 * 
 * @param subcommand A parsed command from the commandline
 * @return int If an error occured, output is -1 else output is 0
 */
static int handle_pwd(struct list_head *subcommand) {
  //#define ERROR_PWD_ARG "Error - pwd takes no arguments\n"
  int num_args = getListLength(subcommand); 
  if (num_args != 1 + 1) { //subcommand is NOT: pwd
    fprintf(stderr, ERROR_PWD_ARG); 
    return -1; 
  } 

  char buf[BUFFER_SIZE]; 
  char *status = getcwd(buf, sizeof(buf)); 

  if (status == NULL) {
    check_status(-1, "pwd"); 
  }

  return 0;
}
 
/**
 * @brief Handles the exit internal command. Not only does the command exit the
 * shell, but it also frees any malloced data to ensure there are no memory leaks. 
 * 
 * @param subcommand A parsed command from the commandline
 * @return int If an error occured, output is -1 else output is 0 
 */
static int handle_exit(struct list_head *subcommand) {
  //TODO: free all memory that has been allocated up to this point
  //need to wait until all the functions have been made 
  //will need to free argument, commmandline, lists being used, etc
  exit(0); 
}

// Declaring a table of internal commands that will be crossreferenced to when processing a command 
internal_t internal_cmds[] = {
  { .name = "setenv" , .handler = handle_setenv }, 
  { .name = "getenv" , .handler = handle_getenv },
  { .name = "unsetenv" , .handler = handle_unsetenv },
  { .name = "cd", .handler = handle_cd }, 
  { .name = "pwd", .handler = handle_pwd }, 
  { .name = "exit", .handler = handle_exit}, 
  0
};

/**
 * @brief Given the command on the command line, this function determines
 * what command needs to be handled and calls the respective function.
 * 
 * @param subcommand A parsed command from the commandline
 * @return int If an error occured, output is -1 else output is 0
 */
int handle_internal(struct list_head *subcommand) {
  int i = 0; 
  struct argument *entry;
  while(internal_cmds[i].name != 0) {
    char *command_name = get_internal_command(subcommand); 
    if (!strcmp(internal_cmds[i].name, command_name)) {
      return internal_cmds[i].handler(subcommand); 
    }
    i++;
  }
  return 1; 
