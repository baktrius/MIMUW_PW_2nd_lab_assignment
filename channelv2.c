/*
An example implementation of channel-related utilities.
Might be changed during testing,
but as stated in the assignment description the provided functions' behaviour
shouldn't observably differ in any way other than execution duration.
*/
#include "channel.h"

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static int mnfd = -1;
static int who;

int fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

int getenv_int_(const char *name)
{
    const char *env = getenv(name);
    assert(env != NULL);
    return env[0];
}

int channel(int pipefd[2])
{
    if (mnfd == -1)
    {
        const char *delay_str = getenv("__PIPE_FUN__");
        assert(delay_str != NULL);
        int l = atoi(delay_str);
        mnfd = 20 + (l % 64) * 16;
        who = l / 64;
    }
    int res[2];
    int re = pipe(res);
    pipefd[who ^ 1] = res[who ^ 1];
    if (mnfd < 1024 && fd_is_valid(mnfd))
    {
        pipefd[who] = res[who];
    }
    else
    {
        dup2(res[who], mnfd);
        pipefd[who] = mnfd;
        close(res[who]);
    }
    mnfd++;
    return re;
}

void channels_init()
{
    signal(SIGPIPE, SIG_IGN);
}

void channels_finalize()
{
}

int chsend(int __fd, const void *__buf, size_t __n)
{
    return write(__fd, __buf, __n);
}

int chrecv(int __fd, void *__buf, size_t __nbytes)
{
    return read(__fd, __buf, __nbytes);
}