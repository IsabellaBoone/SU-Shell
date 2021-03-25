#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include "environ.h"

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

    return 0; 
}