#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contacts.h"

TreeNode *root = NULL;
int contactCount = 0;
static int opCount = 0;

static void insertNode(TreeNode **node, Contact c) {
    if (*node == NULL) {
        *node = malloc(sizeof(TreeNode));
        if (!*node) { perror("malloc"); exit(1); }
        (*node)->data = c;
        (*node)->left = NULL;
        (*node)->right = NULL;
        return;
    }
    int cmp = strcmp(c.lastName, (*node)->data.lastName);
    if (cmp == 0) cmp = strcmp(c.firstName, (*node)->data.firstName);
    
    if (cmp < 0) insertNode(&(*node)->left, c);
    else         insertNode(&(*node)->right, c);
}

static TreeNode* searchNodeByField(TreeNode* node, int field, const char* value) {
    if (!node) return NULL;
    
    int found = 0;
    switch(field) {
        case 1: found = strstr(node->data.lastName, value) != NULL; break;
        case 2: found = strstr(node->data.firstName, value) != NULL; break;
        case 3: found = strstr(node->data.phones, value) != NULL; break;
        case 4: found = strstr(node->data.emails, value) != NULL; break;
        case 5: found = strstr(node->data.social, value) != NULL; break;
        case 6: found = strstr(node->data.messengers, value) != NULL; break;
        default: return NULL;
    }
    if (found) return node;
    
    TreeNode* res = searchNodeByField(node->left, field, value);
    return res ? res : searchNodeByField(node->right, field, value);
}

static TreeNode* deleteNodeByKey(TreeNode* node, const char* last, const char* first) {
    if (!node) return NULL;
    
    int cmp = strcmp(last, node->data.lastName);
    if (cmp == 0) cmp = strcmp(first, node->data.firstName);
    
    if (cmp < 0) node->left = deleteNodeByKey(node->left, last, first);
    else if (cmp > 0) node->right = deleteNodeByKey(node->right, last, first);
    else {
        if (!node->left) { TreeNode* tmp = node->right; free(node); return tmp; }
        if (!node->right) { TreeNode* tmp = node->left; free(node); return tmp; }
        
        TreeNode* minNode = node->right;
        while (minNode->left) minNode = minNode->left;
        
        node->data = minNode->data;
        node->right = deleteNodeByKey(node->right, minNode->data.lastName, minNode->data.firstName);
    }
    return node;
}

static void showInOrder(TreeNode* node, int* counter) {
    if (!node) return;
    showInOrder(node->left, counter);
    printf("[%d] %s %s\n", (*counter)++, node->data.lastName, node->data.firstName);
    printf("    Место работы: %s\n", node->data.work[0] ? node->data.work : "не указано");
    printf("    Должность: %s\n", node->data.position[0] ? node->data.position : "не указано");
    printf("    Телефоны: %s\n", node->data.phones[0] ? node->data.phones : "не указано");
    printf("    Email: %s\n", node->data.emails[0] ? node->data.emails : "не указано");
    printf("    Соцсети: %s\n", node->data.social[0] ? node->data.social : "не указано");
    printf("    Мессенджеры: %s\n", node->data.messengers[0] ? node->data.messengers : "не указано");
    showInOrder(node->right, counter);
}

static void flatten(TreeNode* node, Contact* arr, int* idx) {
    if (!node) return;
    flatten(node->left, arr, idx);
    arr[(*idx)++] = node->data;
    flatten(node->right, arr, idx);
}

static void freeTree(TreeNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

static TreeNode* buildBalanced(Contact* arr, int start, int end) {
    if (start > end) return NULL;
    int mid = start + (end - start) / 2;
    TreeNode* node = malloc(sizeof(TreeNode));
    if (!node) { perror("malloc"); exit(1); }
    node->data = arr[mid];
    node->left = buildBalanced(arr, start, mid - 1);
    node->right = buildBalanced(arr, mid + 1, end);
    return node;
}

static void checkBalance() {
    opCount++;
    if (opCount >= BALANCE_THRESHOLD) {
        balanceTree();
        opCount = 0;
    }
}

void balanceTree() {
    if (!root || contactCount == 0) return;
    Contact arr[MAX_CONTACTS];
    int idx = 0;
    flatten(root, arr, &idx);
    freeTree(root);
    root = buildBalanced(arr, 0, idx - 1);
    printf(">> Дерево сбалансировано!\n");
}

static TreeNode* promptAndFind() {
    if (!root) return NULL;
    int field;
    char search[200];
    printf("Поиск по:\n1. Фамилия\n2. Имя\n3. Телефон\n4. Email\n5. Соцсети\n6. Мессенджер\nВыбор: ");
    scanf("%d", &field);
    getchar();
    printf("Введите значение для поиска: ");
    fgets(search, sizeof(search), stdin);
    search[strcspn(search, "\n")] = 0;
    return searchNodeByField(root, field, search);
}

int searchContact() {
    if (!root) { printf("Контактов нет\n"); return 0; }
    TreeNode* node = promptAndFind();
    if (node) {
        printf("Найден контакт:\n");
        printf("%s %s\n", node->data.lastName, node->data.firstName);
        printf("    Место работы: %s\n", node->data.work[0] ? node->data.work : "не указано");
        printf("    Должность: %s\n", node->data.position[0] ? node->data.position : "не указано");
        printf("    Телефоны: %s\n", node->data.phones[0] ? node->data.phones : "не указано");
        printf("    Email: %s\n", node->data.emails[0] ? node->data.emails : "не указано");
        printf("    Соцсети: %s\n", node->data.social[0] ? node->data.social : "не указано");
        printf("    Мессенджеры: %s\n", node->data.messengers[0] ? node->data.messengers : "не указано");
        return 1;
    }
    printf("Контакт не найден\n");
    return 0;
}

void addContact() {
    if (contactCount >= MAX_CONTACTS) {
        printf("Максимум контактов достигнут\n");
        return;
    }
    Contact c;
    printf("Фамилия: "); scanf("%49s", c.lastName);
    printf("Имя: "); scanf("%49s", c.firstName);
    printf("Место работы: "); scanf("%99s", c.work);
    printf("Должность: "); scanf("%99s", c.position);
    printf("Телефоны: "); scanf("%99s", c.phones);
    printf("Email: "); scanf("%99s", c.emails);
    printf("Соцсети: "); scanf("%199s", c.social);
    printf("Мессенджеры: "); scanf("%199s", c.messengers);
    
    insertNode(&root, c);
    contactCount++;
    checkBalance();
    printf("Контакт добавлен!\n");
}

void showContacts() {
    if (!root) { printf("Контактов нет\n"); return; }
    printf("Список контактов:\n");
    int idx = 0;
    showInOrder(root, &idx);
}

void editContact() {
    if (!root) { printf("Контактов нет\n"); return; }
    TreeNode* node = promptAndFind();
    if (!node) { printf("Контакт не найден\n"); return; }

    Contact old = node->data;
    Contact newC = node->data;
    char buf[256];

    #define EDIT_FIELD(prompt, field) do { \
        printf("%s (текущее: %s): ", prompt, newC.field); \
        fgets(buf, sizeof(buf), stdin); \
        if (buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strcpy(newC.field, buf); } \
    } while(0)

    EDIT_FIELD("Фамилия", lastName);
    EDIT_FIELD("Имя", firstName);
    EDIT_FIELD("Место работы", work);
    EDIT_FIELD("Должность", position);
    EDIT_FIELD("Телефоны", phones);
    EDIT_FIELD("Email", emails);
    EDIT_FIELD("Соцсети", social);
    EDIT_FIELD("Мессенджеры", messengers);

    root = deleteNodeByKey(root, old.lastName, old.firstName);
    insertNode(&root, newC);
    checkBalance();
    printf("Контакт обновлён\n");
}

void deleteContact() {
    if (!root) { printf("Контактов нет\n"); return; }
    TreeNode* node = promptAndFind();
    if (!node) { printf("Контакт не найден\n"); return; }

    printf("Удалить контакт %s %s? (y/n): ", node->data.lastName, node->data.firstName);
    char confirm;
    scanf(" %c", &confirm);
    if (confirm != 'y' && confirm != 'Y') { printf("Удаление отменено\n"); return; }

    root = deleteNodeByKey(root, node->data.lastName, node->data.firstName);
    contactCount--;
    checkBalance();
    printf("Контакт удалён\n");
}
