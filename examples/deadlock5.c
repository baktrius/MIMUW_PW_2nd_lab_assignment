#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "test.h"
#include "../mimpi.h"
#include "mimpi_err.h"

int data[1000000];
int main(int argc, char **argv)
{
    MIMPI_Init(true);

    int const world_rank = MIMPI_World_rank();
    int partner_rank = (world_rank / 2 * 2) + 1 - world_rank % 2;
    memset(data, 42, sizeof(data));
    
    if (world_rank == 0) {
        MIMPI_Send(data, sizeof(int) * 1000000, partner_rank, 2);
    }
    ASSERT_MIMPI_RETCODE(MIMPI_Recv(data, sizeof(int) * 1000000, partner_rank, 1), MIMPI_ERROR_DEADLOCK_DETECTED);
    MIMPI_Finalize();
    return test_success();
}
