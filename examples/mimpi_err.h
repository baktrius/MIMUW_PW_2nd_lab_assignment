#ifndef MIMPI_ERR_H
#define MIMPI_ERR_H

#include <stdio.h>
#include <stdlib.h>
#include "../mimpi.h"

static char const *const print_mimpi_error(MIMPI_Retcode const ret) {
    // This corresponds to MIMPI_Retcode enum values.
    char const *const retcodename[] = {"SUCCESS", "ERROR_ATTEMPTED_SELF_OP", "ERROR_NO_SUCH_RANK", "ERROR_REMOTE_FINISHED", "ERROR_DEADLOCK_DETECTED"};
    if (ret >= 0 && ret < sizeof(retcodename) / sizeof(*retcodename)) {
        return retcodename[ret];
    } else {
        return NULL;
    }
}

#define ASSERT_MIMPI_OK(expr) ASSERT_MIMPI_RETCODE(expr, MIMPI_SUCCESS)

#define ASSERT_MIMPI_RETCODE(expr, expected)                                                       \
    do {                                                                                           \
        MIMPI_Retcode _ret = expr;                                                                 \
        if (_ret != expected) {                                                                    \
            fprintf(                                                                               \
                stderr,                                                                            \
                "MIMPI command failed: %s\n\tIn function %s() in %s line %d.\n\t Code: %i - %s\n", \
                #expr, __func__, __FILE__, __LINE__, _ret, print_mimpi_error(_ret)                 \
            );                                                                                     \
            exit(_ret);                                                                            \
        }                                                                                          \
    } while(0)



#endif // MIMPI_ERR_H