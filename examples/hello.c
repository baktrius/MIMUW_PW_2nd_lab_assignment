#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "test.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const process_rank = MIMPI_World_rank();
    int const size_of_cluster = MIMPI_World_size();

    printf("Hello World from process %d of %d\n", process_rank, size_of_cluster);

    MIMPI_Finalize();
    return test_success();
}
