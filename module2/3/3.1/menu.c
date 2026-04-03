#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "permissions.h"

int main() {
    int choice;
    char input[256];
    char filename[MAX_PATH];
    int mode = -1;
    Permissions perms;

    while (1) {
        printf("\nМаска прав доступа\n");
        printf("1. Ввести права (буквенное/цифровое)\n");
        printf("2. Получить права файла\n");
        printf("3. Изменить права (chmod-команда)\n");
        printf("4. Выход\n");
        printf("Выбор: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Введите права (например, 755 или rwxr-xr--): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                if (strlen(input) == 9) {
                    mode = parseLetterMode(input);
                } else {
                    mode = parseNumericMode(input);
                }
                if (mode < 0) {
                    printf("Ошибка: неверный формат прав\n");
                    mode = -1;
                } else {
                    initPermissions(&perms, mode);
                    printf("\n");
                    printPermissions(&perms);
                    printMode(mode);
                    printBinary(mode);
                }
                break;
            case 2:
                printf("Введите имя файла: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;
                mode = showFilePermissions(filename);
                break;
            case 3:
                if (mode == -1) {
                    printf("Сначала установите права (пункт 1 или 2)\n");
                } else {
                    printf("Текущие права: %o\n", mode);
                    printf("Введите команду (например, u+x, g-w, a=r): ");
                    fgets(input, sizeof(input), stdin);
                    input[strcspn(input, "\n")] = 0;
                    int new_mode = modifyPermissions(mode, input);
                    if (new_mode < 0) {
                        printf("Ошибка: неверная команда\n");
                    } else {
                        mode = new_mode;
                        initPermissions(&perms, mode);
                        printf("\n");
                        printPermissions(&perms);
                        printMode(mode);
                        printBinary(mode);
                    }
                }
                break;
            case 4:
                printf("Выход из программы\n");
                return 0;
            default:
                printf("Неверный выбор\n");
        }
    }
}
