#ifndef PERMISSIONS_H
#define PERMISSIONS_H
#define MAX_PATH 512
typedef struct {
    int owner_read;
    int owner_write;
    int owner_exec;
    int group_read;
    int group_write;
    int group_exec;
    int other_read;
    int other_write;
    int other_exec;
} Permissions;
void initPermissions(Permissions *perms, int mode);
int permissionsToMode(Permissions *perms);
void printPermissions(Permissions *perms);
void printMode(int mode);
void printBinary(int mode);
int parseNumericMode(const char *str);
int parseLetterMode(const char *str);
int modifyPermissions(int current_mode, const char *command);
int getFilePermissions(const char *filename, int *mode);
int showFilePermissions(const char *filename);
#endif
