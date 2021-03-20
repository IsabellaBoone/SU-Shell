//May need to remove when we remove the executor stuff 
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"


void makeArgumentList(struct list_head *list, char **args, int len);
void free_exec_arg_list(char **args, int len);
void handleChildInExecutor(char *command, char *const *args);
void handleParentInExecutor(pid_t pid, int option);
void execute(char *command, char *const *args);
void run_command(int len, struct list_head *list_args);
