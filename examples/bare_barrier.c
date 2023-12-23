#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../mimpi.h"
#include "mimpi_err.h"

#define WRITE_VAR "CHANNELS_WRITE_DELAY"

int main(int argc, char **argv)
{
    MIMPI_Init(false);
    printf("before\n");
    fflush(stdout);
    setenv(WRITE_VAR, getenv("DELAY"), true);
    ASSERT_MIMPI_OK(MIMPI_Barrier());
    unsetenv(WRITE_VAR);
    printf("after\n");
    MIMPI_Finalize();
    return 0;
}
