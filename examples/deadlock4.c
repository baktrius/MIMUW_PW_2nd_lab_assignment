#include <assert.h>
#include <stdbool.h>

#include "test.h"
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    char number = 42;

    if (world_rank == 0) {
        ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, 1, 1), MIMPI_ERROR_DEADLOCK_DETECTED);
        ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, 2, 1), MIMPI_ERROR_DEADLOCK_DETECTED);
    } else {
        ASSERT_MIMPI_RETCODE(MIMPI_Recv(&number, 1, 0, 1), MIMPI_ERROR_DEADLOCK_DETECTED);
    }
    MIMPI_Finalize();
    return test_success();
}
