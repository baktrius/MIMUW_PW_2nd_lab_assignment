#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "../mimpi.h"
#include "test.h"
#include "mimpi_err.h"

#define NS_PER_1_MS 1##000##000

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

    int const tag = 17;

    char number = 42;
    if (world_rank == 0)
    {
        msleep(500);
    }
    else if (world_rank == 1)
    {
        ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, 0, tag), MIMPI_ERROR_REMOTE_FINISHED);
        if (MIMPI_World_size() > 6)
            ASSERT_MIMPI_RETCODE(MIMPI_Send(&number, 1, 0, tag), MIMPI_ERROR_REMOTE_FINISHED);
        ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, 0, tag), MIMPI_ERROR_REMOTE_FINISHED);
        assert(number == 42);
    }
    else if (world_rank == 2)
    {
        msleep(1000);
        ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, 0, tag), MIMPI_ERROR_REMOTE_FINISHED);
        assert(number == 42);
        ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, 0, tag), MIMPI_ERROR_REMOTE_FINISHED);
        assert(number == 42);
    }
    else if (world_rank == 3)
    {
        ASSERT_MIMPI_RETCODE(MIMPI_Barrier(), MIMPI_ERROR_REMOTE_FINISHED);
    }
    else if (world_rank == 4)
    {
        // heurystyczne założenie że chwrite zakończy się z błędem "Zamknięty odpbiorca".
        setenv("CHANNELS_WRITE_DELAY", "1000", true);
        ASSERT_MIMPI_RETCODE(MIMPI_Barrier(), MIMPI_ERROR_REMOTE_FINISHED);
    }
    else if (world_rank == 5)
    {
        msleep(1000);
        ASSERT_MIMPI_RETCODE(MIMPI_Barrier(), MIMPI_ERROR_REMOTE_FINISHED);
    }
    else if (world_rank == 6)
    {
        msleep(1000);
        ASSERT_MIMPI_RETCODE(MIMPI_Send(&number, 1, 0, tag), MIMPI_ERROR_REMOTE_FINISHED);
    }

    MIMPI_Finalize();
    printf("Done\n");
    return test_success();
}
