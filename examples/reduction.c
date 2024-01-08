#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../mimpi.h"
#include "mimpi_err.h"
#include "test.h"

static unsigned factorial(unsigned n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        unsigned result = 1;
        for (unsigned i = 2; i <= n; i++) {
            result *= i;
        }
        return result;
    }
}

#define DATA_LEN 2137

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();
    int const world_size = MIMPI_World_size();

    MIMPI_Op const ops[] = {MIMPI_PROD, MIMPI_SUM, MIMPI_MIN, MIMPI_MAX};
    uint8_t const results[] = {factorial(world_size), (world_size + 1) * world_size / 2, 1, world_size};

    uint8_t send_data[DATA_LEN];
    memset(send_data, world_rank + 1, DATA_LEN);
    uint8_t recv_data[DATA_LEN];

    for (int i = 0; i < sizeof(ops) / sizeof(MIMPI_Op); ++i) {
        MIMPI_Op const op = ops[i];
        int const root = 1;
        ASSERT_MIMPI_OK(MIMPI_Reduce(send_data, recv_data, DATA_LEN, op, root));
        if (world_rank == root) {
            uint8_t const result = results[i];
            for (int k = 0; k < DATA_LEN; ++k) {
                // if (recv_data[k] != result) {
                //     fprintf(stderr, "MISMATCH: op_idx %i, root %i, k=%i: expected %i, got %i.\n", i, root, k, result, recv_data[k]);
                // }
                test_assert(recv_data[k] == result);
            }
        }
    }

    MIMPI_Finalize();
    return test_success();
}
