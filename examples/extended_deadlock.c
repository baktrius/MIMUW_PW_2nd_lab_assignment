#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "../mimpi.h"
#include "test.h"

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

int main(int argc, char **argv)
{
    // Open MPI block with deadlock detection on
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    char c = '6';
    if (world_rank == 0)
    {
        int res = setenv(WRITE_VAR, "100", true);
        int res2 = setenv(READ_VAR, "100", true);
        assert(res2 == 0);
        assert(res == 0);

        assert(MIMPI_Send(&c, 1, 1, 1) == MIMPI_SUCCESS);
        assert(MIMPI_Recv(&c, 1, 1, 1) == MIMPI_SUCCESS);
        assert(MIMPI_Recv(&c, 1, 1, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
        assert(MIMPI_Recv(&c, 1, 1, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    }
    else if (world_rank == 1)
    {
        int res = setenv(WRITE_VAR, "100", true);
        int res2 = setenv(READ_VAR, "100", true);
        assert(res2 == 0);
        assert(res == 0);

        assert(MIMPI_Recv(&c, 1, 0, 1) == MIMPI_SUCCESS);
        assert(MIMPI_Send(&c, 1, 0, 1) == MIMPI_SUCCESS);
        assert(MIMPI_Recv(&c, 1, 0, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
        assert(MIMPI_Recv(&c, 1, 0, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    }
    else
    {
        for (int i = 0; i < 200; i++)
        {
            if (i % 2)
                msleep(20);
            assert(MIMPI_Send(&c, 1, world_rank % 2, 1234) == MIMPI_SUCCESS);
        }
    }

    assert(MIMPI_Barrier() == MIMPI_SUCCESS);
    MIMPI_Finalize();
    printf("Done\n");
    return test_success();
}
