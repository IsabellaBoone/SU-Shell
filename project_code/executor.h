#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"

// struct subcommand {
//     char *stdin; 
//     char *stdout; 
//     enum Token output_type; 
// }; 

// void makeArgumentList(struct list_head *list_args, char **args, int len);
// void free_exec_arg_list(char **args, int len);
void handleChildInExecutor(char *command, char *const *args, char **env);
void handleParentInExecutor(pid_t pid, int option);
void execute(char *command, char *const *args, struct subcommand *subcmd, char **env);
void run_command(int len, int subcommand_count, struct list_head *list_commands, char **env);
#endif