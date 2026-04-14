#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "utils.h"

int is_number(const char *str) {
    if (str == NULL || *str == '\0')
        return 0;

    char *endptr = NULL;
    errno = 0;
    strtod(str, &endptr);

    return (errno == 0 && endptr != str && *endptr == '\0');
}

void print_arg(const char *arg) {
    if (is_number(arg)) {
        double val = strtod(arg, NULL);
        if (val == (long long)val && strchr(arg, '.') == NULL && strchr(arg, 'e') == NULL) {
            printf("  [%s] -> %lld, %lld\n", arg, (long long)val, (long long)val * 2);
        } else {
            printf("  [%s] -> %.6g, %.6g\n", arg, val, val * 2);
        }
    } else {
        printf("  [%s]\n", arg);
    }
}

