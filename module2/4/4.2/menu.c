#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "queue.h"

void simulate_messages(PriorityQueue *q, int count) {
    srand(time(NULL));
    const char *templates[] = {"Системное сообщение", "Ошибка подключения", "Уведомление", "Важное обновление", "Лог события"};
    for (int i = 0; i < count; i++) {
        int prio = rand() % 256;
        char msg[64];
        snprintf(msg, sizeof(msg), "%s #%d", templates[rand() % 5], rand() % 1000);
        if (queue_push(q, prio, msg) != 0) {
            printf("Очередь переполнена!\n");
            break;
        }
    }
    printf("Сгенерировано %d сообщений.\n", count);
}

int main() {
    PriorityQueue q;
    queue_init(&q);
    int choice;
    QueueItem out;

    while (1) {
        printf("\nОчередь с приоритетом\n");
        printf("1. Сгенерировать сообщения\n");
        printf("2. Показать очередь\n");
        printf("3. Извлечь первый элемент (FIFO)\n");
        printf("4. Извлечь по точному приоритету\n");
        printf("5. Извлечь с приоритетом >= N\n");
        printf("6. Выход\n");
        printf("Выбор: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int n;
                printf("Количество: ");
                scanf("%d", &n);
                simulate_messages(&q, n);
                break;
            }
            case 2:
                queue_print(&q);
                break;
            case 3: {
                int res = queue_extract(&q, EXTRACT_FIRST, 0, &out);
                if (res == 1) printf("Извлечено: P:%d, ID:%d, Msg:%s\n", out.priority, out.id, out.message);
                else printf("Не найдено или очередь пуста.\n");
                break;
            }
            case 4: {
                int prio;
                printf("Приоритет (0-255): ");
                scanf("%d", &prio);
                int res = queue_extract(&q, EXTRACT_EXACT, prio, &out);
                if (res == 1) printf("Извлечено: P:%d, ID:%d, Msg:%s\n", out.priority, out.id, out.message);
                else printf("Сообщений с таким приоритетом нет.\n");
                break;
            }
            case 5: {
                int prio;
                printf("Минимальный приоритет: ");
                scanf("%d", &prio);
                int res = queue_extract(&q, EXTRACT_MIN, prio, &out);
                if (res == 1) printf("Извлечено: P:%d, ID:%d, Msg:%s\n", out.priority, out.id, out.message);
                else printf("Сообщений с таким или более высоким приоритетом нет.\n");
                break;
            }
            case 6: return 0;
            default: printf("Неверный выбор\n");
        }
    }
}
