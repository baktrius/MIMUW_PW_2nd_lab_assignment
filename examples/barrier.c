#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);
    int const process_rank = MIMPI_World_rank();
    int const size_of_cluster = MIMPI_World_size();

    for (int i = 0; i < size_of_cluster; i++)
    {
        if (i == process_rank)
        {
            printf("Hello World from process %d of %d\n", process_rank, size_of_cluster);
            fflush(stdout);
        }
        ASSERT_MIMPI_OK(MIMPI_Barrier());
    }

    MIMPI_Finalize();
    return 0;
}
