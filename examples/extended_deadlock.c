#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../mimpi.h"
#define WRITE_VAR "CHANNELS_WRITE_DELAY"

int main(int argc, char **argv)
{
    // Open MPI block with deadlock detection on
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    char c = '6';
    if (world_rank == 0)
    {
        int res = setenv(WRITE_VAR, "500", true);
        assert(res == 0);

        assert(MIMPI_Send(&c, 1, 1, 1234) == MIMPI_SUCCESS);
        assert(MIMPI_Recv(&c, 1, 1, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
        assert(MIMPI_Recv(&c, 1, 1, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    }
    else if (world_rank == 1)
    {
        int res = setenv(WRITE_VAR, "100", true);
        assert(res == 0);
        assert(MIMPI_Recv(&c, 1, 0, 1234) == MIMPI_SUCCESS);
        assert(MIMPI_Recv(&c, 1, 0, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
        assert(MIMPI_Recv(&c, 1, 0, 1234) == MIMPI_ERROR_DEADLOCK_DETECTED);
        assert(MIMPI_Recv(&c, 1, 0, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    }
    else
    {
        for (int i = 0; i < 100; i++)
        {
            assert(MIMPI_Send(&c, 1, world_rank - 2, 1234) == MIMPI_SUCCESS);
        }
    }

    assert(MIMPI_Barrier() == MIMPI_SUCCESS);
    MIMPI_Finalize();
    printf("Done\n");
    return 0;
}
