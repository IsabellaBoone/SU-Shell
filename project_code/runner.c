#include <stdio.h> // for expert debugging
#include <stdlib.h> // for memory allocation
#include <sys/stat.h> //for stat system call
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

/**
 * Checks to ensure that the SUSHHOME environment variable has been
 * set.
 * If set, return 1, else return 0.
*/
int sushhome_exists(struct list_head *list_env){
    char* sushhome = get_env(list_env, "SUSHHOME");

    if(sushhome != NULL){
        return 1;
    }

    return 0;
}

void run_rc_file(struct list_head *list_commands, struct list_head *list_env, struct list_head *list_args, commandline cmdline, char *input) {

    // set_env(list_env, "SUSHHOME", "input.txt");

    if(sushhome_exists(list_env)){
        struct stat sb;
        char* sushhome = get_env_value(list_env, "SUSHHOME");
        int stat_status = stat(sushhome, &sb);
        if ((sb.st_mode & S_IRUSR) || (sb.st_mode & S_IXUSR)) { //if true file is valid, read from file
            FILE *file = fopen(sushhome, "r");   //open .suhrc and read from it
            //read from file and execute commands 
            while (fgets(input, INPUT_LENGTH-1, file)!=NULL) {
                
                int len = strlen(input); 
                input[len-1] = '\0';
                cmdline.num = find_num_subcommands(input, len);
                //creates an array of pointers, in proportion to the number of subcommands
                cmdline.subcommand = malloc(cmdline.num *  sizeof(char *)); 
                copy_subcommands(input, cmdline.num, cmdline.subcommand);
                parse_commandline(list_args, &cmdline, list_commands); 

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
            int flcose_status = fclose(file); 
            //should probably check the status 
        }
    }
}

void run_user_input(struct list_head *list_commands, struct list_head *list_env, struct list_head *list_args, commandline cmdline, char *input, int argc) {
    //while(1) {
        
        
        fgets(input, INPUT_LENGTH, stdin);
        if(input[0] != '\n'){
            //printf("%s", get_env(list_env, "PS1")); 
            printf("%s", get_env_value(list_env, "PS1")); 
            fflush(stdout);
            int len = strlen(input); 
            input[len-1] = '\0';

            cmdline.num = find_num_subcommands(input, len);
            
            //creates an array of pointers, in proportion to the number of subcommands
            cmdline.subcommand = malloc(cmdline.num *  sizeof(char *)); 
            copy_subcommands(input, cmdline.num, cmdline.subcommand);
            parse_commandline(list_args, &cmdline, list_commands);
            
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

            //printf("%s", get_env(list_env, "PS1")); 
            printf("%s", get_env_value(list_env, "PS1")); 
            fflush(stdout);
        }

    //}
}

//void run_file_input();