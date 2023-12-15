#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include "../mimpi.h"
#include "mimpi_err.h"

#define max_fd 1024

int main(int argc, char **argv)
{

    // Nom nom nom, take all possible file descriptors
    for (int i = 3; i < 20; i++)
    {
        assert(dup2(0, i) != -1);
        assert(close(i) != -1);
    }
    for (int i = 1024; i < max_fd; i++)
    {
        assert(dup2(0, i) != -1);
        assert(close(i) != -1);
    }

    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    int const tag = 17;
    for (int i = 3; i < 20; i++)
    {
        assert(dup2(0, i) != -1);
        assert(close(i) != -1);
    }
    for (int i = 1024; i < max_fd; i++)
    {
        assert(dup2(0, i) != -1);
        assert(close(i) != -1);
    }
    char number;
    if (world_rank == 0)
    {
        number = 42;
        ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, 1, tag));
    }
    else if (world_rank == 1)
    {
        ASSERT_MIMPI_OK(MIMPI_Recv(&number, 1, 0, tag));
        printf("Process 1 received number %d from process 0\n", number);
    }

    MIMPI_Finalize();
    return 0;
}
