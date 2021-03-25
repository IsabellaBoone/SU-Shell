#ifndef ENVIRON_H
#define ENVIRON_H

#include "list.h"

struct environment {
    char *name; //the name of the environment variable (ex. NAME)
    char *contents; //the contents of the envp[i]
    struct list_head list; 
};

void display_env_array(char **envp); 
void show_env(char **envp); 
void display_env_list(struct list_head *list); 
char ** make_env_array(struct list_head *list); 
void make_env_list(struct list_head *list, char **envp);  
void free_env_list(struct list_head *list);
void free_env_array(char **envp, int len); 
char * get_env(struct list_head *list, char *name); 
int set_env(struct list_head *list, char *name, char *value); 

#endif