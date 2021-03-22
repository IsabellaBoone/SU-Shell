#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#include "list.h" 

struct environment {
    char *contents; //the contents of the envp[i]
    struct list_head list; //
};

/**
 * @brief Takes the array of environment variables from main and makes a linked list
 * 
 * @param list The list that the environment variables are added to 
 * @param envp The environment variable array that is being made into a list
 */
void make_env_list(struct list_head *list, char **envp) {
    int i = 0; 
    while (envp[i] != NULL) {
        struct environment *env = malloc(sizeof(struct environment)); 
        env->contents = strdup(envp[i]); 
        list_add(&env->list, list); 
    }
}

void show_env(char **envp) {
    int i = 0; 
    while (envp[i] != NULL) {
        printf("%s\n", envp[i]); 
        i++; 
    }
}

/**
 * @brief 
 * 
 * @param list 
 * @return char** 
 */
char ** make_env(struct list_head *list) {
    int list_len = getListLength(list); 
    struct list_head *curr; 
    struct environment *entry; 
    int i = list_len - 1; 

    //TODO: Don't foget to free the memory that is stored in the 
    char **envp = calloc(list_len, sizeof(char *)); //may need to + 1 to be able to store a NULL character 
    for (curr = list->next; curr != list; curr = curr->next) {
        entry = list_entry(curr, struct environment, list);             
        envp[i] = strdup(entry->contents); 
        i--; 
    }
    
    //need to manually set the last argument to NULL, even though it is in linked list
    //may want to just not add null on the linked list 
    envp[list_len - 1] = NULL; 

    return envp; 
}