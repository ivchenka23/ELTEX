#ifndef CONTACTS_H
#define CONTACTS_H

#include <stddef.h>

typedef struct {
    char lastName[50];
    char firstName[50];
    char work[100];
    char position[100];
    char phones[100];
    char emails[100];
    char social[200];
    char messengers[200];
} Contact;

typedef struct ContactNode {
    Contact data;
    struct ContactNode *prev;
    struct ContactNode *next;
} ContactNode;

extern ContactNode *head;

void addContact();
void showContacts();
void editContact();
void deleteContact();
ContactNode* searchContact();

#endif
