#include "shared.h"

int cli_msgid = -1;
int pipe_fds[2];
volatile sig_atomic_t running = 1;

void sig_handler(int sig) { (void)sig; running = 0; }

void *receiver_thread(void *arg) {
    (void)arg;
    Msg msg;
    while (running) {
        if (msgrcv(cli_msgid, &msg, sizeof(msg.mtext), 1, 0) > 0) {
            write(pipe_fds[1], &msg, sizeof(Msg));
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    cli_msgid = msgget(CLI_QUEUE_KEY, IPC_CREAT | 0666);
    if (cli_msgid == -1) { perror("msgget cli"); return 1; }

    if (pipe(pipe_fds) == -1) { perror("pipe"); return 1; }

    pthread_t recv_tid;
    pthread_create(&recv_tid, NULL, receiver_thread, NULL);
    pthread_detach(recv_tid);

    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGCHLD, SIG_IGN);

    int efd = epoll_create1(0);
    if (efd == -1) { perror("epoll_create1"); return 1; }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    ev.data.fd = pipe_fds[0];
    epoll_ctl(efd, EPOLL_CTL_ADD, pipe_fds[0], &ev);

    Driver drivers[MAX_DRIVERS];
    int driver_count = 0;

    printf("Taxi CLI ready. Commands: create_driver, send_task <pid> <secs>, get_status <pid>, get_drivers\n> ");
    fflush(stdout);

    while (running) {
        struct epoll_event events[2];
        int n = epoll_wait(efd, events, 2, -1);
        if (n == -1) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                char line[MAX_MSG_TEXT];
                if (fgets(line, sizeof(line), stdin) == NULL) break;
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line) == 0) { printf("> "); fflush(stdout); continue; }

                char cmd[32], arg1[32], arg2[32];
                arg1[0] = arg2[0] = '\0';
                sscanf(line, "%31s %31s %31s", cmd, arg1, arg2);

                if (strcmp(cmd, "create_driver") == 0) {
                    int d_msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
                    if (d_msgid == -1) { perror("msgget driver"); continue; }
                    pid_t pid = fork();
                    if (pid == 0) {
                        driver_proc(d_msgid);
                        exit(0);
                    } else if (pid > 0) {
                        if (driver_count < MAX_DRIVERS) {
                            drivers[driver_count].pid = pid;
                            drivers[driver_count].msgid = d_msgid;
                            driver_count++;
                            printf("Driver created, PID: %d\n", pid);
                        }
                    }
                } else if (strcmp(cmd, "send_task") == 0) {
                    pid_t target = atoi(arg1);
                    int secs = atoi(arg2);
                    if (secs <= 0) { printf("Invalid time\n"); continue; }
                    int mid = -1;
                    for (int i = 0; i < driver_count; i++) {
                        if (drivers[i].pid == target) { mid = drivers[i].msgid; break; }
                    }
                    if (mid == -1) { printf("Driver %d not found\n", target); continue; }
                    Msg m = {.mtype = 1, .mtext = {0}};
                    snprintf(m.mtext, MAX_MSG_TEXT, "TASK %d", secs);
                    msgsnd(mid, &m, strlen(m.mtext) + 1, 0);
                } else if (strcmp(cmd, "get_status") == 0) {
                    pid_t target = atoi(arg1);
                    int mid = -1;
                    for (int i = 0; i < driver_count; i++) {
                        if (drivers[i].pid == target) { mid = drivers[i].msgid; break; }
                    }
                    if (mid == -1) { printf("Driver %d not found\n", target); continue; }
                    Msg m = {.mtype = 1, .mtext = {0}};
                    snprintf(m.mtext, MAX_MSG_TEXT, "STATUS");
                    msgsnd(mid, &m, strlen(m.mtext) + 1, 0);
                } else if (strcmp(cmd, "get_drivers") == 0) {
                    for (int i = 0; i < driver_count; i++) {
                        Msg m = {.mtype = 1, .mtext = {0}};
                        snprintf(m.mtext, MAX_MSG_TEXT, "STATUS");
                        msgsnd(drivers[i].msgid, &m, strlen(m.mtext) + 1, 0);
                    }
                }
                printf("> ");
                fflush(stdout);
            } else if (events[i].data.fd == pipe_fds[0]) {
                Msg msg;
                ssize_t r = read(pipe_fds[0], &msg, sizeof(Msg));
                if (r > 0) {
                    msg.mtext[MAX_MSG_TEXT - 1] = '\0';
                    printf("\r[Driver]: %s\n> ", msg.mtext);
                    fflush(stdout);
                }
            }
        }
    }

    printf("\nShutting down...\n");
    for (int i = 0; i < driver_count; i++) {
        Msg m = {.mtype = 1, .mtext = {0}};
        snprintf(m.mtext, MAX_MSG_TEXT, "EXIT");
        msgsnd(drivers[i].msgid, &m, strlen(m.mtext) + 1, 0);
    }
    close(efd);
    close(pipe_fds[0]);
    close(pipe_fds[1]);
    msgctl(cli_msgid, IPC_RMID, NULL);
    return 0;
}

static volatile sig_atomic_t drv_running = 1;
static void drv_sig_handler(int sig) { (void)sig; drv_running = 0; }
static void alarm_handler(int sig) { (void)sig; }

void driver_proc(int msgid) {
    struct sigaction sa_sig, sa_alrm;
    sa_sig.sa_handler = drv_sig_handler; sigemptyset(&sa_sig.sa_mask); sa_sig.sa_flags = 0;
    sigaction(SIGINT, &sa_sig, NULL); sigaction(SIGTERM, &sa_sig, NULL);
    sa_alrm.sa_handler = alarm_handler; sigemptyset(&sa_alrm.sa_mask); sa_alrm.sa_flags = 0;
    sigaction(SIGALRM, &sa_alrm, NULL);

    int state = 0;
    time_t busy_until = 0;

    while (drv_running) {
        Msg msg;
        int ret = msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
        if (ret == -1) {
            if (errno == EINTR) {
                time_t now = time(NULL);
                if (state == 1 && now >= busy_until) {
                    state = 0;
                    busy_until = 0;
                }
                continue;
            }
            break;
        }

        char cmd[32];
        int val;
        if (sscanf(msg.mtext, "%31s %d", cmd, &val) < 1) continue;

        time_t now = time(NULL);
        if (state == 1 && now >= busy_until) {
            state = 0;
            busy_until = 0;
        }

        Msg reply = {.mtype = 1, .mtext = {0}};
        if (strcmp(cmd, "TASK") == 0) {
            if (state == 0) {
                state = 1;
                busy_until = now + val;
                alarm(val);
                snprintf(reply.mtext, MAX_MSG_TEXT, "Task accepted");
            } else {
                int rem = busy_until - now;
                if (rem < 0) rem = 0;
                snprintf(reply.mtext, MAX_MSG_TEXT, "Busy %d", rem);
            }
        } else if (strcmp(cmd, "STATUS") == 0) {
            if (state == 0) {
                snprintf(reply.mtext, MAX_MSG_TEXT, "Available");
            } else {
                int rem = busy_until - now;
                if (rem < 0) rem = 0;
                snprintf(reply.mtext, MAX_MSG_TEXT, "Busy %d", rem);
            }
        } else if (strcmp(cmd, "EXIT") == 0) {
            break;
        } else {
            continue;
        }
        msgsnd(cli_msgid, &reply, strlen(reply.mtext) + 1, 0);
    }
    msgctl(msgid, IPC_RMID, NULL);
    exit(0);
}
