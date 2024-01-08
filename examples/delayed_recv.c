#include <unistd.h>

#include "../mimpi.h"
#include "mimpi_err.h"

int main() {
    MIMPI_Init(false);
    int a[1];
    if (MIMPI_World_rank() == 0) {
        sleep(1);
        ASSERT_MIMPI_OK(MIMPI_Send(a, 1, 1, 1));
    } else {
        ASSERT_MIMPI_OK(MIMPI_Recv(a, 1, 0, 1));
    }
    printf("Done!\n");
    MIMPI_Finalize();
}