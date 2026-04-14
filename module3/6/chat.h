#ifndef CHAT_H
#define CHAT_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define IPC_KEY_PATH "/tmp"
#define IPC_KEY_ID   0x55  
#define SERVER_TYPE  10    
#define CLIENT_TYPE(n) (10 + (n) * 10)
#define MAX_TEXT     256
#define MAX_CLIENTS  32

typedef struct {
    long mtype;                  
    int  sender;                 
    char text[MAX_TEXT];         
} Message;

#endif 

