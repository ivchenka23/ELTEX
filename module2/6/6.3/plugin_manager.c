#include <dlfcn.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "calculator.h"

int loadPlugins(const char* dir, Command commands[], int* count, void* handles[]) {
    DIR* d = opendir(dir);
    if (!d) {
        perror("opendir");
        *count = 0;
        return -1;
    }

    struct dirent* entry;
    int idx = 0;

    while ((entry = readdir(d)) != NULL && idx < MAX_COMMANDS) {
        size_t len = strlen(entry->d_name);
        if (len < 3 || strcmp(entry->d_name + len - 3, ".so") != 0) {
            continue;
        }

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

        void* handle = dlopen(path, RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "dlopen %s: %s\n", path, dlerror());
            continue;
        }

        GetCommandFunc get_cmd = (GetCommandFunc)dlsym(handle, "get_command");
        if (!get_cmd) {
            fprintf(stderr, "dlsym %s: %s\n", path, dlerror());
            dlclose(handle);
            continue;
        }

        if (get_cmd(&commands[idx]) != 0) {
            dlclose(handle);
            continue;
        }

        handles[idx] = handle;
        idx++;
    }
    closedir(d);
    *count = idx;
    return 0;
}

void unloadPlugins(void* handles[], int count) {
    for (int i = 0; i < count; i++) {
        if (handles[i]) {
            dlclose(handles[i]);
        }
    }
}
