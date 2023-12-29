#include <assert.h>
#include <stdbool.h>

#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    int partner_rank = (world_rank / 2 * 2) + 1 - world_rank % 2;

    char number;
    ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, partner_rank, 2));
    ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, partner_rank, 1));
    assert(MIMPI_Recv(&number, 1, partner_rank, 1) == MIMPI_SUCCESS);
    assert(MIMPI_Recv(&number, 1, partner_rank, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    MIMPI_Finalize();
    return 0;
}
