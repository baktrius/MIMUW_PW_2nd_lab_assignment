#include <assert.h>
#include <stdio.h>

#include "mimpi_err.h"
#include "../mimpi.h"

int main() {
    MIMPI_Init(false);
    int rank = MIMPI_World_rank();
    int a = 1, b = 2;
    if (rank == 0) {
        ASSERT_MIMPI_OK(MIMPI_Send(&a, sizeof(int), 1, 1));
        ASSERT_MIMPI_OK(MIMPI_Send(&b, sizeof(int), 1, 2));

        ASSERT_MIMPI_OK(MIMPI_Send(&a, sizeof(int), 2, 1));
        ASSERT_MIMPI_OK(MIMPI_Send(&b, sizeof(int), 2, 2));

        ASSERT_MIMPI_OK(MIMPI_Send(&a, sizeof(int), 3, 1));
        ASSERT_MIMPI_OK(MIMPI_Send(&b, sizeof(int), 3, 2));
    } if (rank == 1) {
        ASSERT_MIMPI_OK(MIMPI_Recv(&a, sizeof(int), 0, 2));
        assert(a == 2);
    } else if (rank == 2) {
        ASSERT_MIMPI_OK(MIMPI_Recv(&b, sizeof(int), 0, 1));
        assert(b == 1);
    } else if (rank == 3) {
        ASSERT_MIMPI_OK(MIMPI_Recv(&b, sizeof(int), 0, MIMPI_ANY_TAG));
        assert(b == 1);
    }
    printf("Done!\n");
    MIMPI_Finalize();
}