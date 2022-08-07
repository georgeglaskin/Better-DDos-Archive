#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct port {
    int val, count;

    struct port *prev, *next;
} port_t;

port_t *list_head = NULL;

int *arr;

void swap(int* xp, int* yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void sort_arr(int arr[], int n) {
    int i, j, min_idx;
 
    // One by one move boundary of unsorted subarray
    for (i = 0; i < n - 1; i++) {
 
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;
 
        // Swap the found minimum element
        // with the first element
        swap(&arr[min_idx], &arr[i]);
    }
}

static port_t *append_list(int val) {
    port_t *last = list_head, *node = calloc(1, sizeof(port_t));

    node->val = val; 
    node->next = NULL; //most recent structure so we dont need a next

    if(list_head == NULL) { //if there are no items
        node->prev = NULL; //if this is the first structure we dont have a previous
        list_head = node; //set header to our node because its the first

        return node;
    }

    while(last->next != NULL) //go through the linked list to find the most recent structure added
        last = last->next;
    
    last->next = node;  //set the next one from our most recent to our new node
    node->prev = last;  //set the set our new nodes previous to the old most recent structure

    return node;
}

static port_t *remove_list(port_t *del) {
    port_t *ret;

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

static port_t *search_port(int val) {
    port_t *node = list_head;

    while (node != NULL) {
        if (val == node->val)
            return node;

        node = node->next;
    }

    return NULL;
}

static port_t *search_count(int val) {
    port_t *node = list_head;

    while (node != NULL) {
        if (val == node->count)
            return node;

        node = node->next;
    }

    return NULL;
}

static int list_length(void) {
    int ret = 0;

    port_t *node = list_head;

    while((node = node->next))
        ret++;
    
    return ret + 1;
}

static void file_runner(FILE *fp) {
    char buf[64] = {0}, *buffer;

    port_t *f_node;

    while(fgets(buf, 64, fp)) {
        buffer = strchr(buf, ':');

        *buffer++;

        if((f_node = search_port(atoi(buffer))) == NULL)
            f_node = append_list(atoi(buffer));
        
        f_node->count++;
    }

    fclose(fp);
}

static void arr_append(int val, int loc) {
    arr = realloc(arr, (loc + 1) * sizeof(int));
    arr[loc] = val;
}

static int make_arr(void) {
    int c = 0;
    port_t *head = list_head;

    for(int i = 0; i < list_length(); i++) {
        arr_append(head->val, c++);

        head = head->next;
    }

    sort_arr(arr, c);

    return c;
}

void print_opt(void) {
    int c = make_arr(), i = 1;

    while(c--) {
        port_t *node;
        if((node = search_count(arr[c])) == NULL)
            continue;

        printf("[%d] ", i++);
        printf("[%d] ", node->count);
        printf("[%d]\n", node->val);

        remove_list(node);
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Invalid Usage!\r\n%s [file.txt]\r\n", argv[0]);

        return 0;
    }

    FILE *fp;
    if((fp = fopen(argv[1], "r")) == NULL) {
        perror(argv[1]);

        return -1;
    }

    file_runner(fp);

    print_opt();
}
