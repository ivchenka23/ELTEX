#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contacts.h"

ContactNode *head = NULL;

static void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void readString(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\n")] = 0;
    }
}

ContactNode* searchContact() {
    if (head == NULL) {
        printf("Контактов нет\n");
        return NULL;
    }
    int field;
    char search[256];
    printf("Поиск по:\n1. Фамилия\n2. Имя\n3. Телефон\n4. Email\n5. Соцсети\n6. Мессенджер\nВыбор: ");
    if (scanf("%d", &field) != 1) {
        clearInputBuffer();
        return NULL;
    }
    clearInputBuffer();
    printf("Введите значение для поиска: ");
    fgets(search, sizeof(search), stdin);
    search[strcspn(search, "\n")] = 0;
    if (search[0] == '\0') return NULL;

    ContactNode *curr = head;
    int index = 0;
    while (curr != NULL) {
        int found = 0;
        const char *target = NULL;
        switch (field) {
            case 1: target = curr->data.lastName; break;
            case 2: target = curr->data.firstName; break;
            case 3: target = curr->data.phones; break;
            case 4: target = curr->data.emails; break;
            case 5: target = curr->data.social; break;
            case 6: target = curr->data.messengers; break;
            default: return NULL;
        }
        if (strstr(target, search) != NULL) found = 1;
        if (found) {
            printf("Найден контакт:\n[%d] %s %s\n", index, curr->data.lastName, curr->data.firstName);
            printf("    Место работы: %s\n", curr->data.work[0] ? curr->data.work : "не указано");
            printf("    Должность: %s\n", curr->data.position[0] ? curr->data.position : "не указано");
            printf("    Телефоны: %s\n", curr->data.phones[0] ? curr->data.phones : "не указано");
            printf("    Email: %s\n", curr->data.emails[0] ? curr->data.emails : "не указано");
            printf("    Соцсети: %s\n", curr->data.social[0] ? curr->data.social : "не указано");
            printf("    Мессенджеры: %s\n", curr->data.messengers[0] ? curr->data.messengers : "не указано");
            return curr;
        }
        curr = curr->next;
        index++;
    }
    printf("Контакт не найден\n");
    return NULL;
}

void addContact() {
    ContactNode *newNode = (ContactNode*)malloc(sizeof(ContactNode));
    if (!newNode) { printf("Ошибка выделения памяти\n"); return; }
    memset(&newNode->data, 0, sizeof(Contact));
    newNode->prev = NULL;
    newNode->next = NULL;

    readString("Фамилия: ", newNode->data.lastName, sizeof(newNode->data.lastName));
    readString("Имя: ", newNode->data.firstName, sizeof(newNode->data.firstName));
    readString("Место работы: ", newNode->data.work, sizeof(newNode->data.work));
    readString("Должность: ", newNode->data.position, sizeof(newNode->data.position));
    readString("Телефоны: ", newNode->data.phones, sizeof(newNode->data.phones));
    readString("Email: ", newNode->data.emails, sizeof(newNode->data.emails));
    readString("Соцсети: ", newNode->data.social, sizeof(newNode->data.social));
    readString("Мессенджеры: ", newNode->data.messengers, sizeof(newNode->data.messengers));

    ContactNode *curr = head;
    while (curr && (strcmp(curr->data.lastName, newNode->data.lastName) < 0 || (strcmp(curr->data.lastName, newNode->data.lastName) == 0 && strcmp(curr->data.firstName, newNode->data.firstName) < 0))) {
        curr = curr->next;
    }
    newNode->next = curr;
    newNode->prev = (curr ? curr->prev : NULL);
    if (newNode->prev) newNode->prev->next = newNode;
    else head = newNode;
    if (newNode->next) newNode->next->prev = newNode;
    printf("Контакт добавлен!\n");
}

void showContacts() {
    if (head == NULL) {
        printf("Контактов нет\n");
        return;
    }
    printf("Список контактов:\n");
    ContactNode *curr = head;
    int index = 0;
    while (curr != NULL) {
        printf("[%d] %s %s\n", index, curr->data.lastName, curr->data.firstName);
        printf("    Место работы: %s\n", curr->data.work[0] ? curr->data.work : "не указано");
        printf("    Должность: %s\n", curr->data.position[0] ? curr->data.position : "не указано");
        printf("    Телефоны: %s\n", curr->data.phones[0] ? curr->data.phones : "не указано");
        printf("    Email: %s\n", curr->data.emails[0] ? curr->data.emails : "не указано");
        printf("    Соцсети: %s\n", curr->data.social[0] ? curr->data.social : "не указано");
        printf("    Мессенджеры: %s\n", curr->data.messengers[0] ? curr->data.messengers : "не указано");
        curr = curr->next;
        index++;
    }
}

void editContact() {
    ContactNode *node = searchContact();
    if (!node) return;
    char buf[256];

    printf("Фамилия (текущее: %s): ", node->data.lastName);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.lastName, buf, sizeof(node->data.lastName)-1); node->data.lastName[sizeof(node->data.lastName)-1] = '\0'; }
    printf("Имя (текущее: %s): ", node->data.firstName);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.firstName, buf, sizeof(node->data.firstName)-1); node->data.firstName[sizeof(node->data.firstName)-1] = '\0'; }
    printf("Место работы (текущее: %s): ", node->data.work);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.work, buf, sizeof(node->data.work)-1); node->data.work[sizeof(node->data.work)-1] = '\0'; }
    printf("Должность (текущее: %s): ", node->data.position);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.position, buf, sizeof(node->data.position)-1); node->data.position[sizeof(node->data.position)-1] = '\0'; }
    printf("Телефоны (текущее: %s): ", node->data.phones);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.phones, buf, sizeof(node->data.phones)-1); node->data.phones[sizeof(node->data.phones)-1] = '\0'; }
    printf("Email (текущее: %s): ", node->data.emails);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.emails, buf, sizeof(node->data.emails)-1); node->data.emails[sizeof(node->data.emails)-1] = '\0'; }
    printf("Соцсети (текущее: %s): ", node->data.social);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.social, buf, sizeof(node->data.social)-1); node->data.social[sizeof(node->data.social)-1] = '\0'; }
    printf("Мессенджеры (текущее: %s): ", node->data.messengers);
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n') { buf[strcspn(buf, "\n")] = 0; strncpy(node->data.messengers, buf, sizeof(node->data.messengers)-1); node->data.messengers[sizeof(node->data.messengers)-1] = '\0'; }
    printf("Контакт обновлён\n");
}

void deleteContact() {
    ContactNode *node = searchContact();
    if (!node) return;
    char confirm;
    printf("Удалить этот контакт? (y/n): ");
    scanf(" %c", &confirm);
    clearInputBuffer();
    if (confirm != 'y' && confirm != 'Y') {
        printf("Удаление отменено\n");
        return;
    }
    if (node->prev) node->prev->next = node->next;
    else head = node->next;
    if (node->next) node->next->prev = node->prev;
    free(node);
    printf("Контакт удалён\n");
}
