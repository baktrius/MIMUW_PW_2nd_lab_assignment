#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../mimpi.h"
#include "test.h"

int main(int argc, char **argv) {
    int to_break = atoi(argv[1]);
    MIMPI_Init(false);
    if (MIMPI_World_rank() != to_break) {
        assert(MIMPI_Barrier() == MIMPI_ERROR_REMOTE_FINISHED);
    } else {
        sleep(1);
    }
    MIMPI_Finalize();
    return test_success();
}