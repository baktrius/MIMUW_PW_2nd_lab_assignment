#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "test.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    char number = '1';
    int const tag = 1;

    test_assert(MIMPI_ERROR_ATTEMPTED_SELF_OP == MIMPI_Send(&number, sizeof(number), world_rank, tag));
    test_assert(MIMPI_ERROR_ATTEMPTED_SELF_OP == MIMPI_Recv(&number, sizeof(number), world_rank, tag));

    MIMPI_Finalize();
    return test_success();
}
