#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"

struct subcommand_new {
    char *stdin; 
    char *stdout; 
    enum Token output_type; 
}; 

void makeArgumentList(struct list_head *list_args, char **args, int len);
void free_exec_arg_list(char **args, int len);
void handleChildInExecutor(char *command, char *const *args);
void handleParentInExecutor(pid_t pid, int option);
void execute(char *command, char *const *args, struct subcommand_new *subcmd);
void run_command(int len, struct list_head *list_args);
#endif