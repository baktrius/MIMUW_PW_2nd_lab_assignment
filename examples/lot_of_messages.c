#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include "mimpi_err.h"

#define steps 1000000
#define tagSpace 12
#define spaceSize 12

unsigned char myRand1(long long state)
{
    state = state * 1664525 + 1013904223;
    return state >> 20;
}

unsigned char myRand2(long long state)
{
    state = state * 25214903917 + 11;
    return state >> 20;
}

unsigned char myRand3(long long state)
{
    state = state * 16843009 + 826366247;
    return state >> 20;
}

char *getData(int tag, int size)
{
    char *res = (char *)malloc(size * sizeof(char));
    assert(res != NULL);

    for (int i = 0; i < size; i++)
    {
        res[i] = (myRand1(i) ^ myRand2(tag) ^ myRand3(size));
    }
    return res;
}

void validateData(int tag, int size, char *data)
{
    for (int i = 0; i < size; i++)
    {
        fflush(stdout);
        assert(data[i] == (char)(myRand1(i) ^ myRand2(tag) ^ myRand3(size)));
    }
}

long long st = 2137;
unsigned char myRand4()
{
    st = st * 214013 + 2531011;
    return st >> 20;
}

int main(int argc, char **argv)
{
    MIMPI_Init(false);

    int const world_rank = MIMPI_World_rank();
    int const world_size = MIMPI_World_size();

    st ^= world_rank;

    for (int i = 0; i < steps; i++)
    {
        fflush(stdout);
        if (myRand4() % 4 || i < 20000)
        {
            int tar = myRand4() % (world_size - 1);
            tar += (tar >= world_rank) ? 1 : 0;

            int size = (myRand4() % spaceSize) + 1;
            int tag = (myRand4() % tagSpace) + 1;

            char *data = getData((tag ^ myRand2(world_rank)), size);
            ASSERT_MIMPI_OK(MIMPI_Send(data, size, tar, tag));

            free(data);
        }
        else
        {
            int rec = myRand4() % (world_size - 1);
            rec += (rec >= world_rank) ? 1 : 0;

            int size = (myRand4() % spaceSize) + 1;
            int tag = myRand4() % (tagSpace + 1);

            char *data = (char *)malloc(size * sizeof(char));
            assert(data != NULL);
            ASSERT_MIMPI_OK(MIMPI_Recv(data, size, rec, tag));

            if (tag != 0)
                validateData((tag ^ myRand2(rec)), size, data);
            free(data);
        }
    }
    MIMPI_Barrier();
    fflush(stdout);
    MIMPI_Finalize();
    return 0;
}
