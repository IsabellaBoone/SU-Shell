#include "list.h"

void list_add(struct list_head *new, struct list_head *head){
    struct list_head *next = head->next;

    next->prev = new;
    new->next = next;

    new->prev = head;
    head->next = new;
}

void list_add_tail(struct list_head *new, struct list_head *head){
    struct list_head *next = head->next;
    struct list_head *prev = head->prev;
    prev->next = new;
    new->prev = prev;
    
    new->next = head;
    head->prev = new;
}

void list_del(struct list_head *entry) { 
    struct list_head *next = entry->next;
    struct list_head *prev = entry->prev;

    next->prev = prev;
    prev->next = next;

    //Briggs states to replicate LIST_INIT and set the entries next/prev to itself
    entry->next = entry;
    entry->prev = entry;

}

int list_empty(struct list_head *head){
    int isEmpty = (head->next == head && head->next == head) ? 1 : 0;
    return isEmpty;
}

void list_splice(struct list_head *list, struct list_head *head){
    struct list_head *A = list->next;
    struct list_head *B = list->prev;

    struct list_head *C = head->next;
    struct list_head *D = head->prev;

    //Set the List's head prev (A's prev) to the Head's tail's next (D's next)
    A->prev = D;
    D->next = A; 
    
    //Set the next at the end of List to 
    B->next = C;  //Connect B from List to the C of Head
    C->prev = B;    //Connect C from Head to B from list
}

/**
 * @brief Get the length of the list 
 * 
 * @param list 
 * @return int 
 */
int getListLength(struct list_head *list) {
    int count = 0; 
    struct list_head *curr;
    
    for (curr = list->next; curr != list; curr = curr->next) {
        count++; 
    } 
    return count;
}