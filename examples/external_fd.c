#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../mimpi.h"
#include "mimpi_err.h"
#include "test.h"

#define DESCRIPTOR_FROM_ABOVE 7
#define MSG "Process 1 received number from process 0\n"
#define TAG 1

int main(int argc, char **argv)
{
    // close some fds which are not allowed to be assumed to be left intact
    for (int i = 3; i < DESCRIPTOR_FROM_ABOVE; ++i)
        close(i);
    for (int i = DESCRIPTOR_FROM_ABOVE + 1; i < 20; ++i)
        close(i);
    for (int i = 1024; i < 1099; ++i)
        close(i);

    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    char number;
    if (world_rank == 0)
    {
        number = 42;
        ASSERT_MIMPI_OK(MIMPI_Send(&number, 1, 1, TAG));
    }
    else if (world_rank == 1)
    {
        ASSERT_MIMPI_OK(MIMPI_Recv(&number, 1, 0, TAG));
        write(DESCRIPTOR_FROM_ABOVE, MSG, sizeof(MSG) - 1);
    }

    MIMPI_Finalize();
    return test_success();
}
