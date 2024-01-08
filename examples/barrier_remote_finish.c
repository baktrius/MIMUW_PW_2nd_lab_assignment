#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "test.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    if (world_rank != 1)
    {
        // Unmatched Barrier process 0 skips it.
        // Should exit with MIMPI_ERROR_REMOTE_FINISHED
        // during or after MIMPI_Finalize call in process 0.
        MIMPI_Retcode ret = MIMPI_Barrier();
        test_assert(ret == MIMPI_ERROR_REMOTE_FINISHED);
    }

    MIMPI_Finalize();
    return test_success();
}
