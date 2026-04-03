#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_SIZE 1024
#define MAX_MSG_LEN 128

typedef struct {
    int priority;
    char message[MAX_MSG_LEN];
    int id;
} QueueItem;

typedef struct {
    QueueItem items[MAX_QUEUE_SIZE];
    int size;
    int next_id;
} PriorityQueue;

#define EXTRACT_FIRST 0
#define EXTRACT_EXACT 1
#define EXTRACT_MIN   2

void queue_init(PriorityQueue *q);
int queue_push(PriorityQueue *q, int priority, const char *msg);
int queue_extract(PriorityQueue *q, int mode, int target, QueueItem *out);
void queue_print(const PriorityQueue *q);

#endif
