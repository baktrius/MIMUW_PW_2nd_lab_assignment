#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "test.h"
#include "../mimpi.h"

int main(int argc, char **argv)
{
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    int partner_rank = (world_rank / 2 * 2) + 1 - world_rank % 2;
    int data[1000000];
    memset(data, 42, sizeof(data));

    if (world_rank == 0) {
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
    }
    assert(MIMPI_Recv(data, sizeof(int) * 1000000, partner_rank, 1) == MIMPI_ERROR_DEADLOCK_DETECTED);
    MIMPI_Finalize();
    return test_success();
}
