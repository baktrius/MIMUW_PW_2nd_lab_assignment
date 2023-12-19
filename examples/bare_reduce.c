#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    size_t data_size = 1;
    if (argc > 1)
    {
        data_size = atoi(argv[1]);
    }

    MIMPI_Init(false);
    int const world_rank = MIMPI_World_rank();

    uint8_t *data = malloc(data_size);
    assert(data);
    memset(data, 1, data_size);
    if (world_rank == 0)
    {
        uint8_t *recv_data = malloc(data_size);
        assert(recv_data);
        ASSERT_MIMPI_OK(MIMPI_Reduce(data, recv_data, data_size, MIMPI_SUM, 0));
        for (int i = 1; i < data_size; ++i)
            assert(recv_data[i] == recv_data[0]);
        printf("Number: %d\n", recv_data[0]);
        fflush(stdout);
        free(recv_data);
    }
    else
        ASSERT_MIMPI_OK(MIMPI_Reduce(data, NULL, data_size, MIMPI_SUM, 0));
    assert(data[0] == 1);
    for (int i = 1; i < data_size; ++i)
        assert(data[i] == data[0]);
    free(data);

    MIMPI_Finalize();
    return 0;
}
