#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#include "../mimpi.h"
#include "mimpi_err.h"
#include "test.h"

#define WRITE_VAR "CHANNELS_WRITE_DELAY"
#define READ_VAR "CHANNELS_READ_DELAY"

void handler(int signal_number)
{
}

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

void *sig(void *)
{
    struct sigaction action;
    sigaction(SIGUSR1, NULL, &action);
    action.sa_handler = handler;
    sigaction(SIGUSR1, &action, NULL);
    while (true)
    {
        msleep(5);
        raise(SIGUSR1);
    }
}

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    assert(argc >= 4);
    pthread_t sig_thread;
    pthread_create(&sig_thread, NULL, sig, NULL);

    int res = setenv(WRITE_VAR, "50", true);
    int res2 = setenv(READ_VAR, "50", true);
    assert(res2 == 0);
    assert(res == 0);

    int const tag = 17;
    int size = atoi(argv[1]);
    int from = atoi(argv[2]), to = atoi(argv[3]);

    assert(size > 0);

    if (from == world_rank)
    {
        char *data = (char *)malloc((size_t)size * (size_t)sizeof(char));
        for (int i = 0; i < size; i++)
        {
            data[i] = (char)((i ^ 123456) % 117);
        }
        ASSERT_MIMPI_OK(MIMPI_Send(data, size, to, tag));
        free(data);
    }
    else if (to == world_rank)
    {
        char *data = (char *)malloc((size_t)size * (size_t)sizeof(char));
        ASSERT_MIMPI_OK(MIMPI_Recv(data, size, from, tag));

        for (int i = 0; i < size; i++)
        {
            assert(data[i] == (char)((i ^ 123456) % 117));
        }

        free(data);
        printf("DONE! recived %d bytes.\n", size);
    }

    MIMPI_Finalize();
    pthread_cancel(sig_thread);
    pthread_join(sig_thread, NULL);
    return test_success();
}
