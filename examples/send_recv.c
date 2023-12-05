#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    int const tag = 17;

    char number;
    if (world_rank == 0)
    {
        number = 42;
        ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, 1, tag));
    }
    else if (world_rank == 1)
    {
        ASSERT_MIMPI_OK(MIMPI_Recv(&number, 1, 0, tag));
        printf("Process 1 received number %d from process 0\n", number);
    }

    MIMPI_Finalize();
    return 0;
}
