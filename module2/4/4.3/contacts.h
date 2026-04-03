#ifndef CONTACT_H
#define CONTACT_H

#define MAX_CONTACTS 100
#define BALANCE_THRESHOLD 5

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

typedef struct TreeNode {
    Contact data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

extern TreeNode *root;
extern int contactCount;

void addContact();
void showContacts();
void editContact();
void deleteContact();
int searchContact();
void balanceTree();

#endif
