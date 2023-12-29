#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv) {
    int root = atoi(argv[1]);
    MIMPI_Init(false);
    int world_size = MIMPI_World_size();
    int rank = MIMPI_World_rank();
    {
        uint8_t data[4] = {rank + 1, rank + 2, 2 * rank, root};
        uint8_t recv_data[4] = {0, 0, 0, 0};
        ASSERT_MIMPI_OK(MIMPI_Reduce(data, recv_data, 4, MIMPI_MAX, root));
        if (rank != root) {
            assert(*(uint32_t *)recv_data == 0);
        } else {
            assert(recv_data[0] == world_size);
            assert(recv_data[1] == world_size + 1);
            assert(recv_data[2] == 2 * world_size - 2);
            assert(recv_data[3] == root);
        }
    }
    {
        uint8_t data[4] = {rank + 1, rank + 2, 2 * rank, 1};
        uint8_t recv_data[4] = {0, 0, 0, 0};
        ASSERT_MIMPI_OK(MIMPI_Reduce(data, recv_data, 4, MIMPI_MIN, root));
        if (rank != root) {
            assert(*(uint32_t *)recv_data == 0);
        } else {
            assert(recv_data[0] == 1);
            assert(recv_data[1] == 2);
            assert(recv_data[2] == 0);
            assert(recv_data[3] == 1);
        }
    }
    {
        uint8_t data[4] = {rank + 1, 1, 2 * rank, 0};
        uint8_t recv_data[4] = {0, 0, 0, 0};
        ASSERT_MIMPI_OK(MIMPI_Reduce(data, recv_data, 4, MIMPI_SUM, root));
        if (rank != root) {
            assert(*(uint32_t *)recv_data == 0);
        } else {
            assert(recv_data[0] == (uint8_t)(((world_size + 1) * (world_size)) / 2));
            assert(recv_data[1] == world_size);
            assert(recv_data[2] == (uint8_t)((1 << world_size) * (((world_size + 1) * (world_size)) / 2) - world_size));
            assert(recv_data[3] == 0);
        }
    }
    MIMPI_Finalize();
}