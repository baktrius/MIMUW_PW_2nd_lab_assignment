#include <assert.h>
#include <stdbool.h>
#include "../mimpi.h"
#include "mimpi_err.h"
#include "test.h"

int main(int argc, char **argv)
{
    // Open MPI block with deadlock detection on
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    // Silently assumes even number of processes
    int partner_rank = (world_rank / 2 * 2) + 1 - world_rank % 2;

    char number;
    // First deadlock
    ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, partner_rank, 1), MIMPI_ERROR_DEADLOCK_DETECTED);
    // Second deadlock
    ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, partner_rank, 1), MIMPI_ERROR_DEADLOCK_DETECTED);

    MIMPI_Finalize();
    return test_success();
}
