#include <stdio.h> 
#include <stdlib.h> 

#include "list.h"
#include "datastructures.h"

/**
 * @brief Clears and frees the list_commands
 * 
 * @param list The list of commands being freed. 
 */
void clear_list_command(struct list_head *list) {
    struct subcommand *entry; 
    while (!list_empty(list)) {
        entry = list_entry(list->next, struct subcommand, list);
        //free 2D array
        int i = 0;
        while (entry->exec_args[i] != NULL) {
            free(entry->exec_args[i]); //free array elements
            i++; 
        }
        free(entry->exec_args); //free array
        free(entry->input); 
        free(entry->output); 
        list_del(&entry->list); 
        free(entry); 
    }
}