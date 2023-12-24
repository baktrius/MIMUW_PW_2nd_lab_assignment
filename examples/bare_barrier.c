#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../mimpi.h"
#include "mimpi_err.h"

#define WRITE_VAR "CHANNELS_WRITE_DELAY"

int main(int argc, char **argv)
{
    MIMPI_Init(false);
    printf("before\n");
    fflush(stdout);
    const char *delay = getenv("DELAY");
    if (delay)
        assert(setenv(WRITE_VAR, delay, true) == 0);
    ASSERT_MIMPI_OK(MIMPI_Barrier());
    assert(unsetenv(WRITE_VAR) == 0);
    printf("after\n");
    MIMPI_Finalize();
    return 0;
}
