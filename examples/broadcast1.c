#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv) {
    int root = atoi(argv[1]);
    MIMPI_Init(false);
    int number = 0;
    if (MIMPI_World_rank() == root) {
        number = 42;
    }
    ASSERT_MIMPI_OK(MIMPI_Bcast(&number, sizeof(int), root));
    assert(number == 42);
    MIMPI_Finalize();
}