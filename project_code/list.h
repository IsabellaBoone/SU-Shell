#ifndef LIST_H
#define LIST_H

#include <stddef.h>

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

#define list_entry(ptr, type, member) ({ \
      void *__mptr = (void *)(ptr); \
      ((type *)(__mptr - offsetof(type, member))); \
})


struct list_head {
    struct list_head *next, *prev; 
};

//adds the "new" item to the list at the front. The "new" list becomes the head of the list. 
void list_add(struct list_head *n, struct list_head *head); 

//Adds the "new" item to the list, at the end. 
void list_add_tail(struct list_head *n, struct list_head *head); 

//Removes the item from the list. Updates its next and prev pointers to itself.
void list_del(struct list_head *entry); 

//Returns non-zero if the list is empty, or 0 if the list is not empty 
int list_empty(struct list_head *head); 

//Joins two lists immediately after the head. 
//@list: the list that is spliced after the og head 
//@head: the head of the original list
void list_splice(struct list_head *list, struct list_head *head); 

void print_traverse(struct list_head *node);

#endif