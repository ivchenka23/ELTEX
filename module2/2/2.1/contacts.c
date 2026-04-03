#include <stdio.h>
#include <string.h>
#include "contacts.h"


Contact contacts[MAX_CONTACTS];
int count = 0;

int searchContact() {
    if (count == 0) {
        printf("Контактов нет\n");
        return -1;
    }

    int field;
    char search[200];

    printf("Поиск по:\n");
    printf("1. Фамилия\n");
    printf("2. Имя\n");
    printf("3. Телефон\n");
    printf("4. Email\n");
    printf("5. Соцсети\n");
    printf("6. Мессенджер\n");
    printf("Выбор: ");
    scanf("%d", &field);
    getchar();

    printf("Введите значение для поиска: ");
    fgets(search, sizeof(search), stdin);
    search[strcspn(search, "\n")] = 0;

    for (int i = 0; i < count; i++) {
        int found = 0;
        switch (field) {
        case 1: found = strstr(contacts[i].lastName, search) != NULL; break;
        case 2: found = strstr(contacts[i].firstName, search) != NULL; break;
        case 3: found = strstr(contacts[i].phones, search) != NULL; break;
        case 4: found = strstr(contacts[i].emails, search) != NULL; break;
        case 5: found = strstr(contacts[i].social, search) != NULL; break;
        case 6: found = strstr(contacts[i].messengers, search) != NULL; break;
        default: return -1;
        }

        if (found) {
            printf("Найден контакт:\n");
            printf("[%d] %s %s\n", i, contacts[i].lastName, contacts[i].firstName);
            printf("    Место работы: %s\n", contacts[i].work[0] ? contacts[i].work : "не указано");
            printf("    Должность: %s\n", contacts[i].position[0] ? contacts[i].position : "не указано");
            printf("    Телефоны: %s\n", contacts[i].phones[0] ? contacts[i].phones : "не указано");
            printf("    Email: %s\n", contacts[i].emails[0] ? contacts[i].emails : "не указано");
            printf("    Соцсети: %s\n", contacts[i].social[0] ? contacts[i].social : "не указано");
            printf("    Мессенджеры: %s\n", contacts[i].messengers[0] ? contacts[i].messengers : "не указано");
            return i;
        }
    }

    printf("Контакт не найден\n");
    return -1;
}

void addContact() {
    if (count >= MAX_CONTACTS) {
        printf("Максимум контактов достигнут\n");
        return;
    }

    printf("Фамилия: ");
    scanf("%s", contacts[count].lastName);
    printf("Имя: ");
    scanf("%s", contacts[count].firstName);
    printf("Место работы: ");
    scanf("%s", contacts[count].work);
    printf("Должность: ");
    scanf("%s", contacts[count].position);
    printf("Телефоны: ");
    scanf("%s", contacts[count].phones);
    printf("Email: ");
    scanf("%s", contacts[count].emails);
    printf("Соцсети: ");
    scanf("%s", contacts[count].social);
    printf("Мессенджеры: ");
    scanf("%s", contacts[count].messengers);

    count++;
    printf("Контакт добавлен!\n");
}

void showContacts() {
    if (count == 0) {
        printf("Контактов нет\n");
        return;
    }

    printf("Список контактов:\n");
    for (int i = 0; i < count; i++) {
        printf("[%d] %s %s\n", i, contacts[i].lastName, contacts[i].firstName);
        printf("    Место работы: %s\n", contacts[i].work[0] ? contacts[i].work : "не указано");
        printf("    Должность: %s\n", contacts[i].position[0] ? contacts[i].position : "не указано");
        printf("    Телефоны: %s\n", contacts[i].phones[0] ? contacts[i].phones : "не указано");
        printf("    Email: %s\n", contacts[i].emails[0] ? contacts[i].emails : "не указано");
        printf("    Соцсети: %s\n", contacts[i].social[0] ? contacts[i].social : "не указано");
        printf("    Мессенджеры: %s\n", contacts[i].messengers[0] ? contacts[i].messengers : "не указано");
    }
}

void editContact() {
    if (count == 0) {
        printf("Контактов нет\n");
        return;
    }

    int i = searchContact();
    if (i == -1) {
        return;
    }

    char buf[256];

    printf("Фамилия (текущее: %s): ", contacts[i].lastName);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].lastName, buf);
    }

    printf("Имя (текущее: %s): ", contacts[i].firstName);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].firstName, buf);
    }

    printf("Место работы (текущее: %s): ", contacts[i].work);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].work, buf);
    }

    printf("Должность (текущее: %s): ", contacts[i].position);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].position, buf);
    }

    printf("Телефоны (текущее: %s): ", contacts[i].phones);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].phones, buf);
    }

    printf("Email (текущее: %s): ", contacts[i].emails);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].emails, buf);
    }

    printf("Соцсети (текущее: %s): ", contacts[i].social);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].social, buf);
    }

    printf("Мессенджеры (текущее: %s): ", contacts[i].messengers);
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] != '\n') {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(contacts[i].messengers, buf);
    }

    printf("Контакт обновлён\n");
}

void deleteContact() {
    if (count == 0) {
        printf("Контактов нет\n");
        return;
    }

    int i = searchContact();
    if (i == -1) {
        return;
    }

    char confirm;
    printf("Удалить этот контакт? (y/n): ");
    scanf("%c", &confirm);

    if (confirm != 'y' && confirm != 'Y') {
        printf("Удаление отменено\n");
        return;
    }

    for (int j = i; j < count - 1; j++) {
        contacts[j] = contacts[j + 1];
    }
    count--;
    printf("Контакт удалён\n");
}
