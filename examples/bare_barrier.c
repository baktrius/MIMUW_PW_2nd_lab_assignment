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
    {
        int res = setenv(WRITE_VAR, delay, true);
        assert(res == 0);
    }
    ASSERT_MIMPI_OK(MIMPI_Barrier());
    int res = unsetenv(WRITE_VAR);
    assert(res == 0);
    printf("after\n");
    MIMPI_Finalize();
    return 0;
}
