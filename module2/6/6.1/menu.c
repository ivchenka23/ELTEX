#include <stdio.h>
#include "contacts.h"

static void clear_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    initList();
    int choice;
    while (1) {
        printf("\n1. Добавить контакт\n");
        printf("2. Показать контакты\n");
        printf("3. Редактировать контакт\n");
        printf("4. Удалить контакт\n");
        printf("5. Поиск контакта\n");
        printf("6. Выход\n");
        printf("Выбор: ");
        
        if (scanf("%d", &choice) != 1) {
            clear_stdin();
            printf("Неверный ввод. Введите число от 1 до 6.\n");
            continue;
        }
        clear_stdin();

        switch (choice) {
        case 1: addContact(); break;
        case 2: showContacts(); break;
        case 3: editContact(); break;
        case 4: deleteContact(); break;
        case 5: searchContact(); break;
        case 6:
            freeList();
            printf("Выход...\n");
            return 0;
        default: printf("Неверный выбор\n");
        }
    }
}
