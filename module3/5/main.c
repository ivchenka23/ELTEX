#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define LOG_FILE "counter.log"

static volatile sig_atomic_t sigint_count = 0;
static volatile sig_atomic_t got_signal = 0;
static volatile int last_signum = 0;

static void handler(int sig) {
    last_signum = sig;
    got_signal = 1;

    if (sig == SIGINT) {
        sigint_count++;
    }
}

static void log_signal(int fd, int sig) {
    char buf[128];
    const char *name;

    switch (sig) {
    case SIGINT:  name = "SIGINT";  break;
    case SIGQUIT: name = "SIGQUIT"; break;
    default:      name = "UNKNOWN"; break;
    }

    int len = snprintf(buf, sizeof(buf),
                       "!!! Получен сигнал %s (обработано, счётчик SIGINT: %d)\n",
                       name, (int)sigint_count);

    write(fd, buf, len);
}

int main(void) {
    int fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Ошибка открытия файла");
        return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    char pid_buf[64];
    int pid_len = snprintf(pid_buf, sizeof(pid_buf),
                           "PID: %d\nОжидание сигналов...\n", (int)getpid());
    write(fd, pid_buf, pid_len);

    int counter = 0;

    while (1) {
        if (got_signal) {
            log_signal(fd, last_signum);
            got_signal = 0;

            if (sigint_count >= 3) {
                char msg[] = "Получено 3 сигнала SIGINT — завершение работы.\n";
                write(fd, msg, sizeof(msg) - 1);
                close(fd);
                return 0;
            }
        }

        char buf[64];
        int len = snprintf(buf, sizeof(buf), "%d\n", ++counter);
        write(fd, buf, len);

        sleep(1);
    }

    close(fd);
    return 0;
}

