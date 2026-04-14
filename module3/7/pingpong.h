#ifndef PINGPONG_H
#define PINGPONG_H

#include <mqueue.h>
#include <stdlib.h>

#define PING_Q_NAME "/ping_pong_ping_q"
#define PONG_Q_NAME "/ping_pong_pong_q"
#define MAX_MSG_LEN 256
#define MAX_PRIO    31
#define MSG_PRIO    10
#define EXIT_PRIO   0  
#define OFLAGS      (O_RDWR | O_CREAT)
#define Q_PERMS     0644

#endif 

