#include <assert.h>
#include <stdbool.h>

#include "../mimpi.h"

int main(int argc, char **argv)
{
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    char number;

    if (world_rank == 0) {
        assert(MIMPI_Recv(&number, 1, 1, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
        assert(MIMPI_Recv(&number, 1, 2, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    } else {
        assert(MIMPI_Recv(&number, 1, 0, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    }
    MIMPI_Finalize();
    return 0;
}
