#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "contacts.h"

Contact contacts[MAX_CONTACTS];
int count = 0;

void loadContacts() {
    int fd = open(FILE_PATH, O_RDONLY);
    if (fd == -1) {
        count = 0;
        return;
    }

    ssize_t bytes = read(fd, &count, sizeof(int));
    if (bytes != sizeof(int)) {
        close(fd);
        count = 0;
        return;
    }

    if (count < 0 || count > MAX_CONTACTS) {
        close(fd);
        count = 0;
        return;
    }

    bytes = read(fd, contacts, count * sizeof(Contact));
    if (bytes != (ssize_t)(count * sizeof(Contact))) {
        count = 0;
    }

    close(fd);
}

void saveContacts() {
    int fd = open(FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Ошибка открытия файла для записи");
        return;
    }

    ssize_t bytes = write(fd, &count, sizeof(int));
    if (bytes != sizeof(int)) {
        close(fd);
        return;
    }

    if (count > 0) {
        bytes = write(fd, contacts, count * sizeof(Contact));
        if (bytes != (ssize_t)(count * sizeof(Contact))) {
            perror("Ошибка записи контактов");
        }
    }

    close(fd);
}

int searchContact() {
    if (count == 0) {
        printf("Контакты пусты\n");
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
    printf("6. Мессенджеры\n");
    printf("Выбор: ");
    scanf("%d", &field);
    getchar();

    printf("Введите строку для поиска: ");
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
            printf("    Должность: %s\n", contacts[i].position[0] ? contacts[i].position : "не указана");
            printf("    Телефон: %s\n", contacts[i].phones[0] ? contacts[i].phones : "не указан");
            printf("    Email: %s\n", contacts[i].emails[0] ? contacts[i].emails : "не указан");
            printf("    Соцсети: %s\n", contacts[i].social[0] ? contacts[i].social : "не указаны");
            printf("    Мессенджеры: %s\n", contacts[i].messengers[0] ? contacts[i].messengers : "не указаны");
            return i;
        }
    }

    printf("Контакт не найден\n");
    return -1;
}

void addContact() {
    if (count >= MAX_CONTACTS) {
        printf("Достигнуто максимальное количество\n");
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
    printf("Телефон: ");
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
        printf("Контакты пусты\n");
        return;
    }

    printf("Список контактов:\n");
    for (int i = 0; i < count; i++) {
        printf("[%d] %s %s\n", i, contacts[i].lastName, contacts[i].firstName);
        printf("    Место работы: %s\n", contacts[i].work[0] ? contacts[i].work : "не указано");
        printf("    Должность: %s\n", contacts[i].position[0] ? contacts[i].position : "не указана");
        printf("    Телефон: %s\n", contacts[i].phones[0] ? contacts[i].phones : "не указан");
        printf("    Email: %s\n", contacts[i].emails[0] ? contacts[i].emails : "не указан");
        printf("    Соцсети: %s\n", contacts[i].social[0] ? contacts[i].social : "не указаны");
        printf("    Мессенджеры: %s\n", contacts[i].messengers[0] ? contacts[i].messengers : "не указаны");
    }
}

void editContact() {
    if (count == 0) {
        printf("Контакты пусты\n");
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

    printf("Телефон (текущее: %s): ", contacts[i].phones);
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

    printf("Контакт изменён\n");
}

void deleteContact() {
    if (count == 0) {
        printf("Контакты пусты\n");
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

