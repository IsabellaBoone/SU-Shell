#include "list.h"

//adds the "new" item to the list at the front. The "new" list becomes the head of the list. 
//@new: new entry to be added 
//@head: list head to add it after
void list_add(struct list_head *new, struct list_head *head) {
    struct list_head *prev = head; 
    struct list_head *next = head->next; 
    next->prev = new; 
    new->next = next; 
    new->prev = prev; 
    prev->next = new; 
}

//Removes the item from the list. Updates its next and prev pointers to itself.
//@entry: the item of the list being deleted
void list_del(struct list_head *entry) {
    //Updating the nodes around the entry
    struct list_head *prev = entry->prev; 
    struct list_head *next = entry->next; 
    next->prev = prev; 
    prev->next = next;  

    //updating entry
    entry->prev = entry; 
    entry->next = entry; 
}

//Returns non-zero if the list is empty, or 0 if the list is not empty 
//@head: the head node of the list in question 
int list_empty(struct list_head *head) {
    /*checking to make sure head does not point to itself. 
    If it does, then that means head is the only item in the list. 
    */
    if (head->next == head) {  //compares addresses 
        return 1; 
    }
    return 0; 
} 
