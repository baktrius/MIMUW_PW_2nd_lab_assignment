#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);
    printf("before\n");
    fflush(stdout);
    ASSERT_MIMPI_OK(MIMPI_Barrier());
    printf("after\n");
    MIMPI_Finalize();
    return 0;
}
