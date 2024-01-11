#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "../mimpi.h"
#include "mimpi_err.h"
#include "test.h"

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();

    assert(argc >= 3);

    int size = atoi(argv[1]);
    int from = atoi(argv[2]);

    assert(size > 0);

    if (from != world_rank)
    {
        char *data = (char *)malloc((size_t)size * (size_t)sizeof(char));
        for (int i = 0; i < size; i++)
        {
            data[i] = (uint8_t)((i ^ 123456) % 117);
        }
        ASSERT_MIMPI_OK(MIMPI_Reduce(data, NULL, size, MIMPI_SUM, from));
        free(data);
    }
    else
    {
        char *data_s = (char *)malloc((size_t)size * (size_t)sizeof(char));
        for (int i = 0; i < size; i++)
        {
            data_s[i] = (uint8_t)((i ^ 123456) % 117);
        }

        char *data = (char *)malloc((size_t)size * (size_t)sizeof(char));
        ASSERT_MIMPI_OK(MIMPI_Reduce(data_s, data, size, MIMPI_SUM, from));

        for (int i = 0; i < size; i++)
        {
            uint8_t s = (uint8_t)((i ^ 123456) % 117), sum = 0;
            for (int j = 0; j < MIMPI_World_size(); j++)
                sum += s;
            assert((uint8_t)data[i] == sum);
        }
        free(data_s);
        free(data);
        printf("DONE! received %d bytes.\n", size);
    }

    MIMPI_Finalize();
    return test_success();
}
