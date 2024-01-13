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


int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const process_rank = MIMPI_World_rank();
    int const size_of_cluster = MIMPI_World_size();

    // heurystyczne założenie że i-ty proces wypsisze swoje argumenty zanim zacznie to robić i+1.
    msleep((500 * process_rank) + 1);

    printf("Hello World from process %d of %d.\n", process_rank, size_of_cluster);
    printf("My argc %d.\n", argc);
    for(int i=0; i<argc; i++){
        printf("%dth arg: %s\n", i, argv[i]);
    }
    fflush(stdout);

    MIMPI_Finalize();
    return test_success();
}
