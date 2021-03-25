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

#define INPUT_LENGTH 4094 // Max input length for strings

/**
 * @brief Project 2: Shell Project 
 * 
 * @return int 
 */
int main(int argc, char **argv) {
    commandline cmdline;
    LIST_HEAD(list_args); 
    char input[INPUT_LENGTH]; 
    setenv("PS1", ">", 1); 

    while(1){
        printf("%s", getenv("PS1"));
        fflush(stdout); 
        fgets(input, INPUT_LENGTH, stdin); 

        //printf("Before: %d", input);
        if(argc>0){
            int len = strlen(input); 
            input[len-1] = '\0';

            cmdline.num = find_num_sentences(input, len);
            
            //creates an array of pointers, in proportion to the number of subcommand
            cmdline.subcommand = malloc(cmdline.num *  sizeof(char *)); 

            copy_subcommands(input, cmdline.num, cmdline.subcommand);

            //printing information 
            //print_num_sentences(sentence_info.num);
            print_subcommands(cmdline.num, cmdline.subcommand); 

            parse_commandline(&list_args, &cmdline);

            display_list(&list_args); 
        }

        //Testing for the internal commands: 
        int internal_code = handle_internal(&list_args);
        printf("%d\n", internal_code);
        if(internal_code == 1){
            //finds the length of the list, used to allocate space for the array of character pointers 
            int list_len = getListLength(&list_args); 
            // displayList(&list_args);
            run_command(list_len, cmdline.num, &list_args); 
        }else if( internal_code == -1){
            printf("Error has occured");
        }
        

        //TODO: Make this a seperate function in a diff file
        for(int i=0;i<cmdline.num; i++){
            free(cmdline.subcommand[i]);
        }
        free(cmdline.subcommand);

        //Free list_args' and contents from memory
        clear_list(&list_args); 
        
        

    }

}

