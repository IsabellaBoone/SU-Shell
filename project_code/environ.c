/**
 * @file environ.c
 * @author Hannah Moats
 * @brief Handles the shells internal environment. 
 * @version 0.1
 * @date 2021-03-25
 * 
 * @copyright Copyright (c) 2021
 */
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#include "environ.h"

/**
 * @brief Returns the environment variable name. 
 * 
 * @param contents A string with an environment variable in it (NAME=...). 
 * @return char* The environment variable name. 
 */
static char * get_env_variable_name(char const *contents) {
    int len = strlen(contents) + 1; 

    char *new_contents = malloc(len * sizeof(char)); 
    
    strncpy(new_contents, contents, len); 

    char *name; 
    name = strtok(new_contents, "=");
    return name; 
}

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
        char *name = get_env_variable_name(env->contents);
        env->name = name; 
        list_add_tail(&env->list, list); 
        i++; 
    }
}

/**
 * @brief Takes in the envp array and displays it on the screen. 
 * 
 * @param envp The environment variable array that is displayed. 
 */
void display_env_array(char **envp) {
    int i = 0; 
    while (envp[i] != NULL) {
        printf("%s\n", envp[i]); 
        i++; 
    }
}

/**
 * @brief Deletes the list.
 * 
 * @param list The list being deleted. 
 */
void free_env_list(struct list_head *list) {
  struct environment *entry; //Current entry  during traversal

  while (!list_empty(list))
  {
    entry = list_entry(list->next, struct environment, list);
    list_del(&entry->list);
    free(entry->contents);
    free(entry->name); 
    free(entry);
  }
} 

/**
 * @brief Given a list head for the list that contains the environment variable it returns the 
 * whole environment variable. 
 * 
 * @param list The list that contains the environment variables.
 * @param name The name of the environment variable searched for in the shell's internal environment.
 * @return char* The contents of the envirnment variable or NULL if the environment variable is not there 
 */
char * get_env(struct list_head *list, char *name) {
    struct environment *env; 
    struct list_head *curr; 

    curr = list->next; 
    while (curr != list) {
        env = list_entry(curr, struct environment, list); 
        curr = curr->next; 
        if (!strcmp(env->name, name)) { 
           return env->contents; 
        }
    }
    return NULL; 
}

/**
 * @brief Set the env object
 * 
 * @param list 
 * @param name 
 * @param value 
 * @return int Returns 0 upon success, returns -1 if unsuccessful/error ocurred
 */
int set_env(struct list_head *list, char *name, char *value) {
 struct environment *env; 
 struct list_head *curr; 
/*
    //search list for variable
    printf("broke\n");
    curr = list->next; //<-- breaking here???
    printf("broke");
    while (curr != list) {
        printf("broke");
        env = list_entry(curr, struct environment, list); 
        curr = curr->next; 
        if (strcmp(env->name, name) == 0) { //item is in list 
            //set contents 
            printf("broke"); 
            char *temp = name; 
            temp = strcat(temp, value); 
             printf("broke");
            env->contents = strdup(temp); 
             printf("broke");
            return 0; 
        }
    }
    */
    
    // struct environment *env = malloc(sizeof(struct environment)); 
    //     env->contents = strdup(envp[i]); 
    //     char *name = get_env_variable_name(env->contents);
    //     env->name = name; 
    //     list_add_tail(&env->list, list); 
    //     i++; 

    //variable not in list so add variable 
    // struct environment *environment = malloc(sizeof(struct environment));
    // char *temp = name; 
    // temp = strcat(temp, value); 
    // env->contents = temp;
    // environment->contents = temp; 
    // environment->name = name; 
    // list_add_tail(environment, list); 
    return 0;  
}

/**
 * @brief Frees the environment array. 
 * 
 * @param envp The array that is being freed. 
 * @param len The length of the array being freed. 
 */
void free_env_array(char **envp, int len) {
    int i; 
    for (i = 0; i < len; i++) {
        free(envp[i]); 
    }
    free(envp); 

}

//TODO: free environment list

/**
 * @brief Take in the envp list and displays it on the screen.
 * 
 * @param list_envp The list_head of the list that is being displayed
 */
void display_env_list(struct list_head *envp_list) {
  struct list_head *start = envp_list->next; //Start at the first node after the head
  struct list_head *curr;                    //Tracks current node during traversal
  struct environment *entry;                    //Current nodes struct with contents

  for (curr = envp_list->next; curr->next != start; curr = curr->next)
  {
    entry = list_entry(curr, struct environment, list);
    printf("%s\n", entry->contents);
  }
}

/**
 * @brief 
 * 
 * @param list 
 * @return char** 
 */
char ** make_env_array(struct list_head *list) {
    int list_len = getListLength(list); 
    struct list_head *curr; 
    struct environment *entry; 
    int i = 0; 

    //TODO: Don't foget to free the memory that is stored in the 
    char **envp = calloc(list_len + 1, sizeof(char *)); 
    for (curr = list->next; curr != list; curr = curr->next) {
        entry = list_entry(curr, struct environment, list);             
        envp[i] = strdup(entry->contents);  
        i++; 
    }
    envp[list_len] = NULL; 

    return envp; 
}