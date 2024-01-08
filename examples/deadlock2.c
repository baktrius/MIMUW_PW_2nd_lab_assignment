#include <assert.h>
#include <stdbool.h>

#include "test.h"
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    int partner_rank = (world_rank / 2 * 2) + 1 - world_rank % 2;

    char number = 42;
    MIMPI_Send(&number, 1, partner_rank, 2);
    ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, partner_rank, 1), MIMPI_ERROR_DEADLOCK_DETECTED);
    MIMPI_Finalize();
    return test_success();
}
