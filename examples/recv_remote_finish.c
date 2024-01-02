#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "test.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    int const tag = 17;

    char number;
    if (world_rank == 1)
    {
        // Unmatched Recv.
        // Should exit with MIMPI_ERROR_REMOTE_FINISHED
        // during or after MIMPI_Finalize call in process 0.
        MIMPI_Retcode ret = MIMPI_Recv(&number, 1, 0, tag);
        test_assert(ret == MIMPI_ERROR_REMOTE_FINISHED);
    }

    MIMPI_Finalize();
    return test_success();
}
