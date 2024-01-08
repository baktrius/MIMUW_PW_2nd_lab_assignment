/*
An example implementation of channel-related utilities.
Might be changed during testing,
but as stated in the assignment description the provided functions' behaviour
shouldn't observably differ in any way other than execution duration.
*/
#include "channel.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Assert that expression evaluates to zero (otherwise use result as error number, as in pthreads). */
#define ASSERT_ZERO(expr)                                                                    \
    do {                                                                                     \
        int const _errno = (expr);                                                           \
        if (_errno != 0)                                                                     \
            fprintf(                                                                         \
                stderr,                                                                      \
                "ERROR: Failed: %s\n\tIn function %s() in %s line %d.\n\tErrno: (%d; %s)\n", \
                #expr, __func__, __FILE__, __LINE__, _errno, strerror(_errno)                  \
            );                                                                               \
    } while(0)

// based on: https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
/* msleep(): Sleep for the requested number of milliseconds. */
static int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

#define WRITE_VAR "CHANNELS_WRITE_DELAY"
#define READ_VAR "CHANNELS_READ_DELAY"
#define ATOMIC_BLOCK_SIZE 512

pthread_mutex_t mutex;

static void delay(const char *delay_var, const size_t size)
{
    ASSERT_ZERO(pthread_mutex_lock(&mutex));

    const char *delay_str = getenv(delay_var);
    if (delay_str)
    {
        int delay_ms = atoi(delay_str);
        if (delay_ms > 0)
        {
            msleep((size + ATOMIC_BLOCK_SIZE - 1) / ATOMIC_BLOCK_SIZE * delay_ms);
        }
    }
    // Defaults to not wait
    ASSERT_ZERO(pthread_mutex_unlock(&mutex));
}

int channel(int pipefd[2])
{
    return pipe(pipefd);
}

void channels_init() {
    signal(SIGPIPE, SIG_IGN);

    pthread_mutexattr_t attr;
    ASSERT_ZERO(pthread_mutexattr_init(&attr));
    ASSERT_ZERO(pthread_mutex_init(&mutex, &attr));
    ASSERT_ZERO(pthread_mutexattr_destroy(&attr));
}

void channels_finalize() {
    ASSERT_ZERO(pthread_mutex_destroy(&mutex));
}

int chsend(int __fd, const void *__buf, size_t __n)
{
    delay(WRITE_VAR, __n);
    return write(__fd, __buf, __n);
}

int chrecv(int __fd, void *__buf, size_t __nbytes)
{
    ssize_t res = read(__fd, __buf, __nbytes);
    delay(READ_VAR, __nbytes);
    return res;
}
