/**
 * @file commandlist.h
 * @author Isabella Boone 
 * @author John Gable 
 * @author Hannah Moats
 * 
 * @brief For internal commands table
 * 
 * @version 0.1
 * @date 2021-03-16
 * 
 */

/**
 * @brief Hold a command line argument
 * 
 */
typedef struct commandline {
  char* command; 
  char** args;
} commandline_t;

// Struct for commands
typedef struct internal {
  const char *name; 
  int (*handler)(commandline_t *cmd); 
} internal_t;  

// Static commands
static int handle_setenv(commandline_t *cmd); 
static int handle_getenv(commandline_t *cmd); 
static int handle_unsetenv(commandline_t *cmd); 

// Declare table
internal_t internal_cmds[] = {
  { .name = "setenv" , .handler = handle_setenv },
  { .name = "getenv" , .handler = handle_getenv },
  { .name = "unsetenv" , .handler = handle_unsetenv }, 
  0
};

int handle_internal(commandline_t *cmd) {
  int i = 0; 
  while(internal_cmds[i].name != 0) {
    if (!strcmp(internal_cmds[i].name, cmd->command)) {
      return internal_cmds[i].handler(cmd); 
    }
    i++;
  }
  return 0; 
}

