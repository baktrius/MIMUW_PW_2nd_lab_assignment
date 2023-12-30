#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "../mimpi.h"
#include "mimpi_err.h"

#define NS_PER_1_MS 1 ## 000 ## 000

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

    char number = 0;
    if (world_rank == 0)
    {
        // printf("SENDING %d from process 0\n", number);
        number = 13;
        ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, 1, tag+1));
        char l[2] = {14,15};
        ASSERT_MIMPI_OK(MIMPI_Send(&l, 2, 1, tag));
        number = 42;
        ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, 1, tag));
        ASSERT_MIMPI_OK(MIMPI_Send(&l, 2, 1, tag));
        number = 13;
        ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, 1, tag+1));
    }
    else if (world_rank == 1)
    {
        msleep(1000);   
        ASSERT_MIMPI_OK(MIMPI_Recv(&number, 1, 0, tag));
        printf("Process 1 received number %d from process 0\n", number);
    }

    MIMPI_Finalize();
    return 0;
}
