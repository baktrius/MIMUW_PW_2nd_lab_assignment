#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "test.h"

int main()
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();
    int const world_size = MIMPI_World_size();

    char number = '1';
    int const tag = 1;

    test_assert(MIMPI_ERROR_NO_SUCH_RANK == MIMPI_Send(&number, sizeof(number), world_size + world_rank, tag));
    test_assert(MIMPI_ERROR_NO_SUCH_RANK == MIMPI_Recv(&number, sizeof(number), world_size + world_rank, tag));

    MIMPI_Finalize();
    return test_success();
}
