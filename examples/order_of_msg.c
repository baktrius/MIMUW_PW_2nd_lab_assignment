#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "../mimpi.h"
#include "mimpi_err.h"

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

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    int const tag = 2137;

    char number = 0;
    if (world_rank == 0)
    {
        for (; number <= 10; number++)
        {
            msleep(100);
            ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, 1, tag));
        }
        // printf("SENDING %d from process 0\n", number);
    }
    else if (world_rank == 1)
    {
        for (char e = 0; e <= 10; e++)
        {
            if (e % 5 == 0)
                msleep(500);

            ASSERT_MIMPI_OK(MIMPI_Recv(&number, 1, 0, tag));
            printf("Process 1 received number %d from process 0\n", number);
        }
    }

    MIMPI_Finalize();
    return 0;
}
