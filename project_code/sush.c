/**
 * @file sush.c
 * @author Isabella Boone 
 * @author John Gable 
 * @author Hannah Moats
 * @brief 
 * @version 0.1
 * @date 2021-03-16
 */
// Imports
#include <stdio.h> // for expert debugging
#include <stdlib.h> // for memory allocation

// Imports from our files
#include "list.h"
#include "datastructures.h"
#include "executor.h"
#include "internal.h" 
#include "environ.h"
#include "clearlist.h"

#define INPUT_LENGTH 4094 // Max input length for strings

/**
 * @brief Project 2: Shell Project 
 * 
 * @return int 
 */
int main(int argc, char **argv, char **envp) {
    commandline cmdline;
    LIST_HEAD(list_args); //TODO should probably put this in parser.c and have it visible only there 
    LIST_HEAD(list_commands); //a list of subcommand structs, represents the comamndline
    LIST_HEAD(list_env); 
    char input[INPUT_LENGTH]; 
    //setenv("PS1", ">", 1); 
    int num=0;

    make_env_list(&list_env, envp);
    //display_env_list(&list_env);

    while(1){
        
        printf("%s", get_env(&list_env, "PS1")); //TODO: need to change this, not a viable solution 
        fflush(stdout);
         
        fgets(input, INPUT_LENGTH, stdin);
        
        if(input[0] != '\n'){
            // printf("%s", get_env("PS1")); //TODO: need to change this, not a viable solution 
            // fflush(stdout); 
            //printf("Before: %d", input);
            if(argc > 0){
                int len = strlen(input); 
                input[len-1] = '\0';

                cmdline.num = find_num_subcommands(input, len);
                
                //creates an array of pointers, in proportion to the number of subcommand
                cmdline.subcommand = malloc(cmdline.num *  sizeof(char *)); 

                copy_subcommands(input, cmdline.num, cmdline.subcommand);

                //printing information 
                //print_num_sentences(sentence_info.num);
                //print_subcommands(cmdline.num, cmdline.subcommand); 

                parse_commandline(&list_args, &cmdline, &list_commands); //CAN'T USE LIST ARGS AFTER THIS
                //display_list(&list_commands);

            }

            
            //TODO will need to pass a list_commands, instead of list_args
            //Testing for the internal commands: 
            int internal_code = handle_internal(&list_commands, &list_env);
            if(internal_code == 1) {
                // finds the length of the list, used to allocate space for the array of character pointers 
                int list_len = getListLength(&list_commands); 
                char **new_envp = make_env_array(&list_env); 
                run_command(list_len, cmdline.num, &list_commands, new_envp);
                list_len = getListLength(&list_env); 
                free_env_array(new_envp, list_len);  
            } else if (internal_code == 6) { //TODO: is this alright?
                clear_list_command(&list_commands); 
                exit(0);
            }
        
            //Freeing Malloced Stuff
            //TODO: Make this a seperate function in a diff file
            for(int i=0;i<cmdline.num; i++){
                free(cmdline.subcommand[i]);
            }
            free(cmdline.subcommand);
        
            clear_list_argument(&list_args);  
            clear_list_command(&list_commands); 
        }
    }
    clear_list_env(&list_env); 
}

