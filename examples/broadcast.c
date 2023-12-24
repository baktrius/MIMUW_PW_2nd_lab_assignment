#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "../mimpi.h"
#include "mimpi_err.h"

#define WRITE_VAR "CHANNELS_WRITE_DELAY"

int main(int argc, char **argv)
{
    MIMPI_Init(false);
    int const world_rank = MIMPI_World_rank();

    const char *delay = getenv("DELAY");
    if (delay)
        assert(setenv(WRITE_VAR, delay, true) == 0);

    char number = 0;
    if (world_rank == 0)
        number = 42;
    ASSERT_MIMPI_OK(MIMPI_Bcast(&number, 1, 0));
    printf("Number: %d\n", number);
    fflush(stdout);

    assert(unsetenv(WRITE_VAR) == 0);

    MIMPI_Finalize();
    return 0;
}
