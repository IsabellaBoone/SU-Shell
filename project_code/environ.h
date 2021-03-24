#ifndef ENVIRON_H
#define ENVIRON_H

void show_env(char **envp); 
char ** make_env(struct list_head *list); 
void make_env_list(struct list_head *list, char **envp); 

#endif