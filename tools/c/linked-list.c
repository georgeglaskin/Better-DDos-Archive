#include <stdio.h>

typedef struct list_t {
    int val;

    struct list_t *prev, *next;
} List;

List *list_head = NULL;

void append_list(int val) {
    List *last = list_head, *node = calloc(1, sizeof(List));

    node->val = val; 
    node->next = NULL; //most recent structure so we dont need a next

    if(list_head == NULL) { //if there are no items
        node->prev = NULL; //if this is the first structure we dont have a previous
        list_head = node; //set header to our node because its the first

        return;
    }

    while(last->next != NULL) //go through the linked list to find the most recent structure added
        last = last->next;
    
    last->next = node;  //set the next one from our most recent to our new node
    node->prev = last;  //set the set our new nodes previous to the old most recent structure
}

List *remove_list(List *del) {
    List *ret;

    if(list_head == NULL || del == NULL) //dont wanna fuck with null shit
        return NULL;
    
    if(del == list_head) { //if we are deleting our header structure
        list_head = list_head->next; //set our header structure to the structure after header

        list_head->prev = NULL; //set the new header to null
    }
    else
        del->prev->next = del->next; //move the deleted nodes next one to the structure after
    
    free(del);

    ret = del->next; //return the new current node

    return ret;
}

static char search_list(int val) {
    Locker *node = list_head;

    while (node != NULL) {
        if (val == node->pid)
            return 1;

        node = node->next;
    }

    return 0;
}
