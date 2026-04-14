#include <stdio.h>
#include "contacts.h"

int main() {
    int choice;

    loadContacts();
    printf("Загружено контактов: %d\n", count);

    while (1) {
        printf("\n1. Добавить контакт\n");
        printf("2. Показать контакты\n");
        printf("3. Редактировать контакт\n");
        printf("4. Удалить контакт\n");
        printf("5. Поиск контакта\n");
        printf("6. Выход\n");
        printf("Выбор: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: addContact(); break;
        case 2: showContacts(); break;
        case 3: editContact(); break;
        case 4: deleteContact(); break;
        case 5: searchContact(); break;
        case 6:
            saveContacts();
            printf("Контакты сохранены. Выход.\n");
            return 0;
        default: printf("Неверная команда\n");
        }
    }
}

