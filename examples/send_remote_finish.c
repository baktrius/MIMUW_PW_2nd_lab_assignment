#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "test.h"

#define MESSAGE "Litwo! Ojczyzno moja! ty jestes jak zdrowie.\nIle cie trzeba cenic, ten tylko sie dowie,\nKto cie stracil..."

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    int const tag = 17;

    if (world_rank == 0)
    {
        sleep(1);
        for (int i = 0; i < 500; ++i)
        {
            // Unmatched sends.
            // None of those calls should block indefinitely,
            // but they can either succeed or return MIMPI_ERROR_REMOTE_FINISH.
            MIMPI_Send(MESSAGE, sizeof(MESSAGE), 1, tag);
        }
    }

    MIMPI_Finalize();
    return test_success();
}
