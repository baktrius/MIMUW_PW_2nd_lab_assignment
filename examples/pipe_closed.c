#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "../mimpi.h"
#include "test.h"

#define NS_PER_1_MS 1 ## 000 ## 000

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    int const tag = 17;

    char number = 42;
    if (world_rank == 0) {
        struct timespec ts = (struct timespec){.tv_sec = 0, .tv_nsec = NS_PER_1_MS};
        int res;
        do {
            res = nanosleep(&ts, &ts);
        } while (res && errno == EINTR);
    } else if (world_rank == 1) {
        test_assert(MIMPI_Recv(&number, 1, 0, tag) == MIMPI_ERROR_REMOTE_FINISHED);
        test_assert(MIMPI_Send(&number, 1, 0, tag) == MIMPI_ERROR_REMOTE_FINISHED);
        printf("Process 1 received number %d from process 0\n", number);
    }

    // Process with rank 0 finishes before rank 1 gets its message.
    MIMPI_Finalize();
    return test_success();
}
