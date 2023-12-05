/*
This file provides declaration of
communication functions to be used by MIMPI implementation.
Overview rules to apply:
- replace `pipe` with `channel`
- replace `write` (or any other file writing function) with `chsend` when used on channel's fd
- replace `read` (or any other file reading function) with `chrecv` when used on channel's fd
*/
#ifndef CHANNEL_H
#define CHANNEL_H
#include <stddef.h>

/*
This is required to be called in MIMPI_Init.
*/
void channels_init();

/*
This is required to be called in MIMPI_Finalize.
*/
void channels_finalize();

/*
Works similarly to `pipe`, but possibly takes more time to finish.
*/
int channel(int pipefd[2]);
/*
Works similarly to `write`, but possibly takes more time to finish.
*/
int chsend(int __fd, const void *__buf, size_t __n);
/*
Works similarly to `read`, but possibly takes more time to finish.
*/
int chrecv(int __fd, void *__buf, size_t __nbytes);

#endif /* CHANNEL_H */
