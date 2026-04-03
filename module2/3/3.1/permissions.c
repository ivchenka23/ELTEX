#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "permissions.h"

void initPermissions(Permissions *perms, int mode) {
    perms->owner_read = (mode & 0400) ? 1 : 0;
    perms->owner_write = (mode & 0200) ? 1 : 0;
    perms->owner_exec = (mode & 0100) ? 1 : 0;
    perms->group_read = (mode & 0040) ? 1 : 0;
    perms->group_write = (mode & 0020) ? 1 : 0;
    perms->group_exec = (mode & 0010) ? 1 : 0;
    perms->other_read = (mode & 0004) ? 1 : 0;
    perms->other_write = (mode & 0002) ? 1 : 0;
    perms->other_exec = (mode & 0001) ? 1 : 0;
}

int permissionsToMode(Permissions *perms) {
    int mode = 0;
    if (perms->owner_read) mode |= 0400;
    if (perms->owner_write) mode |= 0200;
    if (perms->owner_exec) mode |= 0100;
    if (perms->group_read) mode |= 0040;
    if (perms->group_write) mode |= 0020;
    if (perms->group_exec) mode |= 0010;
    if (perms->other_read) mode |= 0004;
    if (perms->other_write) mode |= 0002;
    if (perms->other_exec) mode |= 0001;
    return mode;
}

void printPermissions(Permissions *perms) {
    printf("Буквенное: ");
    printf("%c%c%c", perms->owner_read ? 'r' : '-', perms->owner_write ? 'w' : '-', perms->owner_exec ? 'x' : '-');
    printf("%c%c%c", perms->group_read ? 'r' : '-', perms->group_write ? 'w' : '-', perms->group_exec ? 'x' : '-');
    printf("%c%c%c", perms->other_read ? 'r' : '-', perms->other_write ? 'w' : '-', perms->other_exec ? 'x' : '-');
    printf("\n");
}

void printMode(int mode) {
    printf("Цифровое: %o\n", mode & 0777);
}

void printBinary(int mode) {
    printf("Битовое: ");
    for (int i = 8; i >= 0; i--) {
        printf("%d", (mode >> i) & 1);
        if (i == 6 || i == 3) printf(" ");
    }
    printf("\n");
}

int parseNumericMode(const char *str) {
    int len = strlen(str);
    if (len == 0 || len > 4) return -1;
    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '7') return -1;
    }
    return strtol(str, NULL, 8) & 0777;
}

int parseLetterMode(const char *str) {
    if (strlen(str) != 9) return -1;
    Permissions p = {0};
    p.owner_read = (str[0] == 'r') ? 1 : (str[0] == '-' ? 0 : -1);
    p.owner_write = (str[1] == 'w') ? 1 : (str[1] == '-' ? 0 : -1);
    p.owner_exec = (str[2] == 'x') ? 1 : (str[2] == '-' ? 0 : -1);
    p.group_read = (str[3] == 'r') ? 1 : (str[3] == '-' ? 0 : -1);
    p.group_write = (str[4] == 'w') ? 1 : (str[4] == '-' ? 0 : -1);
    p.group_exec = (str[5] == 'x') ? 1 : (str[5] == '-' ? 0 : -1);
    p.other_read = (str[6] == 'r') ? 1 : (str[6] == '-' ? 0 : -1);
    p.other_write = (str[7] == 'w') ? 1 : (str[7] == '-' ? 0 : -1);
    p.other_exec = (str[8] == 'x') ? 1 : (str[8] == '-' ? 0 : -1);
    if (p.owner_read == -1 || p.owner_write == -1 || p.owner_exec == -1 ||
        p.group_read == -1 || p.group_write == -1 || p.group_exec == -1 ||
        p.other_read == -1 || p.other_write == -1 || p.other_exec == -1) return -1;
    return permissionsToMode(&p);
}

int modifyPermissions(int current_mode, const char *command) {
    char who[10], op, perms[10];
    if (sscanf(command, "%[^+-=]%c%s", who, &op, perms) < 3) return -1;

    int who_mask = 0;
    for (int i = 0; who[i]; i++) {
        switch (who[i]) {
            case 'u': who_mask |= 0700; break;
            case 'g': who_mask |= 0070; break;
            case 'o': who_mask |= 0007; break;
            case 'a': who_mask |= 0777; break;
            default: return -1;
        }
    }

    int perm_mask = 0;
    for (int i = 0; perms[i]; i++) {
        switch (perms[i]) {
            case 'r': perm_mask |= 0444; break;
            case 'w': perm_mask |= 0222; break;
            case 'x': perm_mask |= 0111; break;
            default: return -1;
        }
    }

    int apply_mask = perm_mask & who_mask;

    switch (op) {
        case '+': return current_mode | apply_mask;
        case '-': return current_mode & ~apply_mask;
        case '=': return (current_mode & ~who_mask) | apply_mask;
        default: return -1;
    }
}

int getFilePermissions(const char *filename, int *mode) {
    struct stat st;
    if (stat(filename, &st) != 0) return -1;
    *mode = st.st_mode & 0777;
    return 0;
}

int showFilePermissions(const char *filename) {
    int mode;
    if (getFilePermissions(filename, &mode) != 0) {
        printf("Ошибка: не удалось получить информацию о файле '%s'\n", filename);
        return -1;
    }
    Permissions perms;
    initPermissions(&perms, mode);
    printf("\nФайл: %s\n", filename);
    printPermissions(&perms);
    printMode(mode);
    printBinary(mode);
    return mode;
}
