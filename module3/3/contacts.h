#ifndef CONTACT_H
#define CONTACT_H

#define MAX_CONTACTS 100
#define FILE_PATH "contacts.dat"

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

extern Contact contacts[MAX_CONTACTS];
extern int count;

void loadContacts();

void saveContacts();

void addContact();
void showContacts();
void editContact();
void deleteContact();
int searchContact();

#endif

