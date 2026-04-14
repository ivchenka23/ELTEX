#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <число1> [число2] ...\n", argv[0]);
        return 1;
    }

    double sum = 0;
    for (int i = 1; i < argc; i++) {
        char *end;
        double val = strtod(argv[i], &end);
        if (*end != '\0') {
            fprintf(stderr, "Не является числом: %s\n", argv[i]);
            return 1;
        }
        sum += val;
    }

    if (sum == (long long)sum)
        printf("Сумма: %lld\n", (long long)sum);
    else
        printf("Сумма: %.6g\n", sum);

    return 0;
}

