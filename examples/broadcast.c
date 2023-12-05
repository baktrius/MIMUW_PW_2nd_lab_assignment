#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);
    int const world_rank = MIMPI_World_rank();

    char number = 0;
    if (world_rank == 0)
        number = 42;
    ASSERT_MIMPI_OK(MIMPI_Bcast(&number, 1, 0));
    printf("Number: %d\n", number);
    fflush(stdout);

    MIMPI_Finalize();
    return 0;
}
