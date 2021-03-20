/**
 * @file sush.c
 * @author Isabella Boone 
 * @author John Gable 
 * @author Hannah Moats
 * @brief 
 * @version 0.1
 * @date 2021-03-16
 */

#include <stdio.h> 
// #include "Commands/file.h"
// #include "Commands/commandlist.h"
#include "list.h"
#include "parser.h"
#include "executor.h"
#define INPUT_LENGTH 4094


/**
 * @brief Project 2: Shell Project 
 * 
 * @return int 
 */
int main(int argc, char **argv) {
    LIST_HEAD(list_args); 
    char input[INPUT_LENGTH]; 

    while(1){
        fgets(input, INPUT_LENGTH, stdin); 

        //printf("Before: %d", input);
        if(argc>0){
            int len = strlen(input); 
            input[len-1] = '\0';
            sentence_info.num = find_num_sentences(input, len);
            
            //creates an array of pointers, in proportion to the number of sentences
            sentence_info.sentences = malloc(sentence_info.num *  sizeof(char *)); 

            copy_sentences(input, sentence_info.num, sentence_info.sentences);

            //printing information 
            //print_num_sentences(sentence_info.num);
            //print_sentences(sentence_info.num, sentence_info.sentences); 

            for(int i=0; i<sentence_info.num; i++){
                stringExtract(&list_args, sentence_info.sentences[i], strlen(sentence_info.sentences[i]));
            }

            //displayList(&list_args);
        }

        //finds the length of the list, used to allocate space for the array of character pointers 
        int list_len = getListLength(&list_args); 
        run_command(list_len, &list_args); 

        //Free list_args' and contents from memory
        clear_list(&list_args); 
    }

}

