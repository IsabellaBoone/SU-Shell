/**
 * @file internal.c
 * @author Isabella Boone 
 * @author Hannah Moats
 * @author John Gable
 * @brief Handle Internal Commands
 * @date 2021-03-22
 */

// Include Statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "parser.h"

// Struct for commands
typedef struct internal {
  const char *name; 
  int (*handler)(struct argument *arg); 
} internal_t;  

// Declare table
internal_t internal_cmds[] = {
  { .name = "setenv" , .handler = handle_setenv },
  { .name = "getenv" , .handler = handle_getenv },
  { .name = "unsetenv" , .handler = handle_unsetenv }, 
  { .name = "cd", .handler = handle_cd }, 
  { .name = "pwd", .handler = handle_pwd }, 
  { .name = "exit", .handler = handle_exit}, 
  0
};


// Static commands
static int handle_setenv(struct argument *arg); 
static int handle_getenv(struct argument *arg); 
static int handle_unsetenv(struct argument *arg); 
static int handle_cd(struct argument *arg); 
static int handle_pwd(struct argument *arg); 
static int handle_exit(struct argument *arg); 

/**
 * @brief Find what internal command matches and return its handler. 
 * 
 * @param arg Argument to find match of
 * @return int handler
 */
int handle_internal(struct argument *arg) {
  int i = 0; 
  while(internal_cmds[i].name != 0) {
    if (!strcmp(internal_cmds[i].name, arg->contents)) {
      return internal_cmds[i].handler(arg); 
    }
    i++;
  }
  return 0; 
}
