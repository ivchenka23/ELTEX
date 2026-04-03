#include <stdio.h>
#include <string.h>
#include "queue.h"

void queue_init(PriorityQueue *q) {
    q->size = 0;
    q->next_id = 1;
}

int queue_push(PriorityQueue *q, int priority, const char *msg) {
    if (q->size >= MAX_QUEUE_SIZE) return -1;
    if (priority < 0 || priority > 255) return -1;

    QueueItem *item = &q->items[q->size];
    item->priority = priority;
    item->id = q->next_id++;
    strncpy(item->message, msg, MAX_MSG_LEN - 1);
    item->message[MAX_MSG_LEN - 1] = '\0';
    q->size++;
    return 0;
}

int queue_extract(PriorityQueue *q, int mode, int target, QueueItem *out) {
    if (q->size == 0) return -1;

    int idx = -1;

    if (mode == EXTRACT_FIRST) {
        idx = 0;
    } else if (mode == EXTRACT_EXACT) {
        for (int i = 0; i < q->size; i++) {
            if (q->items[i].priority == target) {
                idx = i;
                break;
            }
        }
    } else if (mode == EXTRACT_MIN) {
        for (int i = 0; i < q->size; i++) {
            if (q->items[i].priority >= target) {
                idx = i;
                break;
            }
        }
    }

    if (idx == -1) return 0;

    *out = q->items[idx];
    for (int i = idx; i < q->size - 1; i++) {
        q->items[i] = q->items[i + 1];
    }
    q->size--;
    return 1;
}

void queue_print(const PriorityQueue *q) {
    if (q->size == 0) {
        printf("Очередь пуста\n");
        return;
    }
    printf("Размер: %d\n", q->size);
    for (int i = 0; i < q->size; i++) {
        printf("[%d] P:%3d | ID:%3d | Msg: %s\n", i, q->items[i].priority, q->items[i].id, q->items[i].message);
    }
}
