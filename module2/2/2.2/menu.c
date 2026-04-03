#include <stdio.h>
#include "calculator.h"

int main() {
    int choice;
    double a, b, result;
    
    while (1) {
        printf("\n=== Калькулятор ===\n");
        printf("1. Сложение (+)\n");
        printf("2. Вычитание (-)\n");
        printf("3. Умножение (*)\n");
        printf("4. Деление (/)\n");
        printf("5. Возведение в степень\n");
        printf("6. Квадратный корень\n");
        printf("7. Выход\n");
        printf("Выбор: ");
        scanf("%d", &choice);
        
        if (choice == 7) {
            printf("Выход из программы\n");
            break;
        }
        
        if (choice >= 1 && choice <= 5) {
            printf("Введите первый аргумент: ");
            scanf("%lf", &a);
            printf("Введите второй аргумент: ");
            scanf("%lf", &b);
            
            switch (choice) {
                case 1:
                    result = add(a, b);
                    printf("Результат: %.2lf + %.2lf = %.2lf\n", a, b, result);
                    break;
                case 2:
                    result = subtract(a, b);
                    printf("Результат: %.2lf - %.2lf = %.2lf\n", a, b, result);
                    break;
                case 3:
                    result = multiply(a, b);
                    printf("Результат: %.2lf * %.2lf = %.2lf\n", a, b, result);
                    break;
                case 4:
                    result = divide(a, b);
                    printf("Результат: %.2lf / %.2lf = %.2lf\n", a, b, result);
                    break;
                case 5:
                    result = power(a, b);
                    printf("Результат: %.2lf ^ %.2lf = %.2lf\n", a, b, result);
                    break;
            }
        } else if (choice == 6) {
            printf("Введите число: ");
            scanf("%lf", &a);
            result = squareRoot(a);
            printf("Результат: √%.2lf = %.2lf\n", a, result);
        } else {
            printf("Неверный выбор\n");
        }
    }
    
    return 0;
}
