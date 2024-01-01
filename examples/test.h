#ifndef TEST_H
#define TEST_H
#include <unistd.h>
#include <assert.h>

#define SUCCESS_MARKER "<<success>>\n"

int test_success()
{
    // below unchecked syscall return code
    write(STDERR_FILENO, SUCCESS_MARKER, sizeof(SUCCESS_MARKER));
    return 0;
}

#define test_assert(expr) assert(expr && "<<error>>")

#endif // TEST_H
