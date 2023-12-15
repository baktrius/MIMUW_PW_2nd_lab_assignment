#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "mimpi_err.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    assert(argc >= 4);

    int const tag = 17;
    int size = atoi(argv[1]);
    int from = atoi(argv[2]), to = atoi(argv[3]);

    assert(size>0);

    if (from == world_rank)
    {
        char *data = (char *)malloc((size_t)size * (size_t) sizeof(char));
        for(int i=0;i<size; i++){
            data[i] = (char)((i ^ 123456) % 117);
        }
        ASSERT_MIMPI_OK(MIMPI_Send(data, size, to, tag));
        free(data);
    }
    else if (to == world_rank)
    {
        char *data = (char *)malloc((size_t)size * (size_t) sizeof(char));
        ASSERT_MIMPI_OK(MIMPI_Recv(data, size, from, tag));
        
        for(int i=0;i<size; i++){
            assert(data[i] == (char)((i ^ 123456) % 117));
        }

        free(data);
        printf("DONE! recived %d bytes.\n", size);
    }

    MIMPI_Finalize();
    return 0;
}
