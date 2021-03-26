#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "environ.h"
#include "datastructures.h"

void test_display_env_array(char ** envp) {
    display_env_array(envp); 
}

void test_make_env_list(struct list_head *list, char **envp) {
    make_env_list(list, envp); 
}

void test_display_env_list(struct list_head *envp_list) {
    display_env_list(envp_list); 
}

char ** test_make_env_array(struct list_head *list) {
    char **envp = make_env_array(list); 
    display_env_array(envp); 
    return envp; 
}

void test_free_env_array(char **envp, struct list_head *list) {
    int len = getListLength(list); ;
    free_env_array(envp, len); 
}

void test_free_env_list(struct list_head *list) {
    free_env_list(list); 
} 

void test_get_env(struct list_head *list) {
    char *env; 
    char *og; 

    og = getenv("NAME"); 
    printf("getenv NAME: %s\n", og); 

    env = get_env(list, "NAME"); 
    printf("get_env NAME: %s\n", env);  
}

void test_set_env(struct list_head *list) {
    char *env; 
    set_env(list, "NAME", "batman"); 
    env = getenv("NAME"); 
    
}

struct subcommand_new {
    char **execs; 
    char *input; 
    char *output; 
    enum Token type; 
    struct list_head list; 
}; 

void test_print_list(struct list_head *list_commands) {
   struct list_head *curr; 
   curr = list_commands->next; 
   struct subcommand_new *sub = list_entry(curr, struct subcommand_new, list); 
   printf("%s\n", sub->execs[0]); 
   printf("%s\n", sub->execs[1]); 
   //printf("%s\n", sub->execs[2]); 

    pid_t pid = fork(); 

    if (pid == 0) {
        //concat /bin/ls 
        execvp("/bin/ls", (char *const *)sub->execs); 
        exit(1);  
    } else {
        int status; 
        waitpid(pid, &status, 0); 
    }

}



/*
int main (int argc, char **argv, char **envp) {
    
    LIST_HEAD(list_envp); 

    printf("Test 1: \n"); 
    test_display_env_array(envp);

    printf("\nTest 2: \n");
    test_make_env_list(&list_envp, envp); 
    test_display_env_list(&list_envp); 
    //free_env_list(&list_envp); 

    printf("\nTest 3: \n");
    char **envp_test = test_make_env_array(&list_envp);

    printf("\nTest 4: \n"); 
    test_get_env(&list_envp);

    printf("\nTest 5: \n");  
    test_set_env(&list_envp);

    printf("\nTest next to last: \n"); 
    test_free_env_array(envp_test, &list_envp); 
    printf("Freed??? %s\n", envp_test[0]); //yay it works

    printf("\nTest last: \n"); 
    test_free_env_list(&list_envp); 
    display_env_list(&list_envp); //yay it works 
    
   LIST_HEAD(list_args); 
   LIST_HEAD(list_commands); 

   char *args[] = {"ls", "-l", ">", "output.txt", NULL}; 


   struct subcommand_new *subcmd = malloc(sizeof(struct subcommand_new)); 
   //scan list_arg for redirect, mark the input or output as either a file or stdin or stdout
   //if there is more than one command then you know its pipes
   //for the first command, its either stdin or stdout or file input
   //for the last command its either stdin or stdout or file output 
   //for the middle commands its always stdin or stdout
   //assign the toke type to the token type of the subcommand
    subcmd->input = strdup("stdin"); 
    subcmd->output = strdup(args[3]); 
    subcmd->type = REDIRECT_OUTPUT_TRUNCATE; 
    //remove, file names and redirects from list


   int num_args = 2 + 1; //for the NULL
   subcmd->execs = malloc(num_args * sizeof(char *)); 
   subcmd->execs[0] = args[0]; 
   subcmd->execs[1] = args[1]; 
   subcmd->execs[2] = NULL; 
   printf("%s\n", subcmd->execs[0]); 
   printf("%s\n", subcmd->execs[1]); 
   //printf("%s\n", subcmd->execs[2]); 
   //delte all nodes up until the NULL
   //repeat 

   list_add_tail(&subcmd->list, &list_commands); 

   test_print_list(&list_commands); 
    return 0; 
}
*/
