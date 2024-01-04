#include <stdio.h>

#include "../mimpi.h"

int main() {
    MIMPI_Init(0);

    if (MIMPI_World_rank() != 0) {
        int a[10];
        for (int i = 0; i < 10000; i++) {
            MIMPI_Send(a, 10 * sizeof(int), 0, MIMPI_ANY_TAG);
        }
    }

    MIMPI_Barrier();
    printf("Done!\n");
    MIMPI_Finalize();
}