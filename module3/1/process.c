#include <stdio.h>
#include "utils.h"
#include "process.h"

void handle_args(int argc, char *argv[], int start_idx, int end_idx, const char *label) {
    (void)argc;
    if (start_idx >= end_idx)
        return;

    printf("\n%s процесс (аргументы %d-%d)\n", label, start_idx + 1, end_idx);

    for (int i = start_idx; i < end_idx; i++) {
        print_arg(argv[i + 1]);
    }
}

