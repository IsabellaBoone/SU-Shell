#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "list.h"

#define INPUT_LENGTH 4094

/**
 * @brief Current state on parser 
 * 
 */
enum State
{
  WHITESPACE,
  CHARACTER,
  QUOTE,
  OUTPUT,
  INPUT
};

/**
 * @brief Token Enum for type of argument
 */
enum Token
{
  REDIRECT_INPUT,
  REDIRECT_OUTPUT_APPEND,
  REDIRECT_OUTPUT_TRUNCATE,
  NORMAL,
  FILENAME
};

/** 
 * Main struct for the linked list.
 * Contains the type of user input(list or dashed), the contents  (the input),
 * and the recursive list.
 */
struct argument {
    char *contents;
    enum Token token; 
    struct list_head list;
};

/**
 * @brief 
 * 
 */
struct subcommand
{
  struct list_head list; // part of a list of subcommands
  // struct argument *args; // a pointer to the head of arg_list
  char **exec_args; // equivalent to lsargs in hw3
  // enum command_type command; // internal environment, job internal, normal
};

/** Full line that is typed
 */
struct commandline_t
{
  int num;           // Number of subcommands
  char **subcommand; // 2d array of all subcommands
};

//Finds the number of subcommand in the input string and returns that value.
int find_num_sentences(char input[], int len)
{
  int i, count = 1;

  for (i = 0; i < len; i++)
  {
    if (input[i] == '|')
    {
      count++;
    }
  }

  return count;
}

//Copies an individual sentence to a pointer
void copy_sentence(char **subcommand, char *sentence, int i)
{
  strcpy(subcommand[i], sentence);
}

//Copies all the subcommand from input into the array of pointers
void copy_sentences(char input[], int num, char **subcommand)
{
  int i, len;
  char *sentence = strtok(input, "|");

  len = strlen(sentence);

  for (i = 0; i < num; i++)
  {
    len = strlen(sentence);
    subcommand[i] = malloc(len + 2);
    copy_sentence(subcommand, sentence, i);

    sentence = strtok(NULL, "|");
  }
}

//Prints the number of subcommand in input
void print_num_sentences(int num)
{
  printf("num: %d\n", num);
}

//Prints the subcommand in the correct format
void print_sentences(int num, char **sen)
{
  int i = 0;
  for (i = 0; i < num; i++)
  {
    printf("%d : (%s)\n", i, sen[i]);
  }
}

/**
 * Receives a linked list of our list_head struct.
 * The function will traverse through our linked list and
 * free it from memory after deleting each node entry.
 */
void clear_list(struct list_head *list)
{
  struct argument *entry; //Current entry  during traversal

  while (!list_empty(list))
  {
    entry = list_entry(list->next, struct argument, list);
    list_del(&entry->list);
    free(entry->contents);
    free(entry);
  }
}

/**
 * Recurses through a given list_head struct's list,
 * and prints the contents to the console.
 */
void displayList(struct list_head *todo_list)
{
  struct list_head *start = todo_list->next; //Start at the first node after the head
  struct list_head *curr;                    //Tracks current node during traversal
  struct argument *entry;                    //Current nodes struct with contents

  for (curr = todo_list->next; curr->next != start; curr = curr->next)
  {
    entry = list_entry(curr, struct argument, list);
    printf("(%s), (%d)\n", entry->contents, entry->token);
  }
}

/**
 * Take in the input string and length of the string as parameters.
 * Returns the amount of subcommand in the input string
**/
void stringExtract(struct list_head *list_args, struct commandline_t *commandline)
{
  int word_count = 0;
  int currentState = CHARACTER;

  char *temp = calloc(100, sizeof(char)); // Temporary word variable
  struct argument *arg;                   // Linked List

  for (int i = 0; i < commandline->num; i++)
  {
    for (int j = 0; j < strlen(commandline->subcommand[i]); j++)
    {
      if ((commandline->subcommand[i][j] != ' ' && commandline->subcommand[i][j] != '\t' && commandline->subcommand[i][j] != '"'))
      {
        arg = malloc(sizeof(struct argument));
        if (commandline->subcommand[i][j] == '>')
        {
          if (commandline->subcommand[i][j + 1] == '>')
          {
            arg->token = REDIRECT_OUTPUT_APPEND;
            strncat(temp, &commandline->subcommand[i][j], 2);
          }
          else
          {
            arg->token = REDIRECT_OUTPUT_TRUNCATE;
            strncat(temp, &commandline->subcommand[i][j], 1);
          }

          arg->contents = strdup(temp);    //store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); //add the argument to the list of args
          memset(temp, 0, 50);

          /**
           * @brief Increment J so we don't run into this again 
           * 
           */
          if (arg->token == REDIRECT_OUTPUT_APPEND)
          {
            j++;
          }
        }
        else if (commandline->subcommand[i][j] == '<')
        {
          arg = malloc(sizeof(struct argument));
          arg->token = REDIRECT_INPUT;
          strncat(temp, &commandline->subcommand[i][j], 1);
          arg->contents = strdup(temp);    //store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); //add the argument to the list of args
          memset(temp, 0, 50);
        }
        else
        {

          // char is a character
          currentState = CHARACTER; // Set current state
          strncat(temp, &commandline->subcommand[i][j], 1);

          // if we found the last word, and it has no space after, add it to the list
          if (j == (strlen(commandline->subcommand[i]) - 1))
          {
            arg = malloc(sizeof(struct argument));
            arg->contents = strdup(temp);
            arg->token = NORMAL;
            list_add_tail(&arg->list, list_args);
            memset(temp, 0, 50);
          }
        }
      }
      else if (commandline->subcommand[i][j] == ' ' || commandline->subcommand[i][j] == '\t')
      {
        // if we see a space or tab
        if (currentState != WHITESPACE)
        {
          currentState = WHITESPACE;
          arg = malloc(sizeof(struct argument));
          arg->token = NORMAL;             // set token to normal
          arg->contents = strdup(temp);    //store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); //add the argument to the list of args
          word_count++;                    //increment which word we are on
          memset(temp, 0, 50);             //reset the temp word variable to blank
        }
      }
      else if (commandline->subcommand[i][j] == '"')
      {
        if (currentState != QUOTE)
        {
          currentState = QUOTE;
          j++;
          while (commandline->subcommand[i][j] != '"')
          {
            strncat(temp, &commandline->subcommand[i][j], 1);
            j++;
          }
          arg->token = NORMAL; // set token to normal
          arg = malloc(sizeof(struct argument));
          arg->contents = strdup(temp);    //store the last full word into contents of an argument
          list_add_tail(&arg->list, list_args); //add the argument to the list of args
          memset(temp, 0, 50);             //reset the temp word variable to blank
          currentState = WHITESPACE;
        }
      }
    }
    //The req specify ending the list of arguements with a NULL for exec
    arg = malloc(sizeof(struct argument));
    arg->contents = strdup("\0");
    arg->token = NORMAL;
    list_add_tail(&arg->list, list_args);
  }


  free(temp);
}
