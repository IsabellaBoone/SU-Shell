#include <stdio.h> // for expert debugging
#include <stdlib.h> // for memory allocation

// Imports from our files
#include "runner.h"

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

void free_commandline_struct(commandline cmdline) {
    for(int i=0;i<cmdline.num; i++){
        free(cmdline.subcommand[i]);
    }
    free(cmdline.subcommand);
}

void freeing_on_exit(struct list_head *list_commands, struct list_head *list_env, commandline cmdline) {
    clear_list_command(list_commands); 
    clear_list_env(list_env);
    free_commandline_struct(cmdline);  
}

void run_user_input(struct list_head *list_commands, struct list_head *list_env, struct list_head *list_args, commandline cmdline, char *input, int argc) {
    while(1) {
        
        printf("%s", get_env(list_env, "PS1")); 
        fflush(stdout);
         
        fgets(input, INPUT_LENGTH, stdin); //scan for user input
        
        if(input[0] != '\n'){
            if(argc > 0){
                int len = strlen(input); 
                input[len-1] = '\0';

                cmdline.num = find_num_subcommands(input, len);
                
                //creates an array of pointers, in proportion to the number of subcommands
                cmdline.subcommand = malloc(cmdline.num *  sizeof(char *)); 
                copy_subcommands(input, cmdline.num, cmdline.subcommand);
                parse_commandline(list_args, &cmdline, list_commands);
            }

            //Checks if an internal command, if it is then it is run, else a normal command is run
            int internal_code = handle_internal(list_commands, list_env);
            if(internal_code == 1) {
                char **new_envp = make_env_array(list_env); 
                run_command(cmdline.num, list_commands, new_envp);
                int list_len = getListLength(list_env); 
                free_env_array(new_envp, list_len);  
            } else if (internal_code == 6) { //TODO: is this alright?
                freeing_on_exit(list_commands, list_env, cmdline);
                exit(0);
            }

            free_commandline_struct(cmdline);   
            clear_list_command(list_commands); 
        }
    }
}

//void run_file_input();