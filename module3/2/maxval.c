#include <stdio.h>
#include <stdlib.h>
#include <float.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Использование: %s <число1> [число2] ...\n", argv[0]);
        return 1;
    }

    double max = -DBL_MAX;
    int found = 0;

    for (int i = 1; i < argc; i++) {
        char *end;
        double val = strtod(argv[i], &end);
        if (*end != '\0') {
            fprintf(stderr, "Не является числом: %s\n", argv[i]);
            return 1;
        }
        if (!found || val > max) {
            max = val;
            found = 1;
        }
    }

    if (max == (long long)max)
        printf("Максимум: %lld\n", (long long)max);
    else
        printf("Максимум: %.6g\n", max);

    return 0;
}

