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

#define ASSERT_MIMPI_OK(expr)                                                                      \
    do {                                                                                           \
        MIMPI_Retcode ret = expr;                                                                  \
        if (ret != MIMPI_SUCCESS) {                                                                \
            fprintf(                                                                               \
                stderr,                                                                            \
                "MIMPI command failed: %s\n\tIn function %s() in %s line %d.\n\t Code: %i - %s\n", \
                #expr, __func__, __FILE__, __LINE__, ret, print_mimpi_error(ret)                   \
            );                                                                                     \
            exit(ret);                                                                             \
        }                                                                                          \
    } while(0)



#endif // MIMPI_ERR_H