#ifndef MIMPI_H
#define MIMPI_H

#include <stdbool.h>

#define MIMPI_ANY_TAG 0

/// Return code of MIMPI operations.
typedef enum {
    MIMPI_SUCCESS = 0, /// operation ended successfully
    MIMPI_ERROR_ATTEMPTED_SELF_OP = 1, /// process attempted to send/recv to itself
    MIMPI_ERROR_NO_SUCH_RANK = 2, /// no process with requested rank exists in the world
    MIMPI_ERROR_REMOTE_FINISHED = 3, /// the remote process involved in communication has finished
    MIMPI_ERROR_DEADLOCK_DETECTED = 4, /// a deadlock has been detected
} MIMPI_Retcode;

/// @brief Reduction operation kind.
///
/// Type of operation performed in @ref MIMPI_Reduce().
typedef enum {
    MIMPI_MAX,
    MIMPI_MIN,
    MIMPI_SUM,
    MIMPI_PROD,
} MIMPI_Op;

/// @brief Initialises MIMPI framework in MIMPI programs.
///
/// Opens an _MPI block_, permitting use of other MIMPI procedures.
/// @param enable_deadlock_detection - a flag whether deadlock detection
///        should be enabled or not. Note that this adds a considerable
///        overhead, so should only be used when needed.
///
void MIMPI_Init(bool enable_deadlock_detection);

/// @brief Finalises MIMPI framework in MIMPI programs.
///
/// Closes an _MPI block_, freeing all MIMPI-related resources.
/// After a process has called this function, all MIMPI interaction with it
/// (e.g. sending data to it) should return `MIMPI_ERROR_REMOTE_FINISHED`.
///
void MIMPI_Finalize();

/// @brief Returns the number of processes launched by `mimpirun`.
int MIMPI_World_size();

/// @brief Returns the identifier of this process.
///
/// The identifier is unique processes launched by `mimpirun`.
/// Identifiers should be consecutive natural numbers.
///
int MIMPI_World_rank();

/// @brief Sends data to the specified process.
///
/// Sends @ref count bytes of @ref data to the process with rank @ref destination.
/// Data is tagged with @ref tag.
///
/// @param data - data to be sent.
/// @param count - number of bytes of data to be sent.
/// @param destination - rank of the process who is to receive the data. 
/// @param tag - a discriminant of the data, which can be used
///              to distinguish between messages.
/// @return MIMPI return code:
///         - `MIMPI_SUCCESS` if operation ended successfully.
///         - `MIMPI_ERROR_ATTEMPTED_SELF_OP` if process attempted to send to itself
///         - `MIMPI_ERROR_NO_SUCH_RANK` if there is no process with rank
///           @ref destination in the world.
///         - `MIMPI_ERROR_REMOTE_FINISHED` if the process with rank
///         - @ref destination has already escaped _MPI block_.
///
MIMPI_Retcode MIMPI_Send(
    void const *data,
    int count,
    int destination,
    int tag
);

/// @brief Receives data from the specified process.
///
/// Blocks until @ref count bytes of @ref data tagged with @ref tag arrives
/// from the process with rank @ref destination. Then the data is put in @ref data.
///
/// @param data - place where received data is to be put.
/// @param count - number of bytes of data to be received.
/// @param source - rank of the process for data from we are waiting. 
/// @param tag - a discriminant of the data, which can be used
///              to distinguish between messages.
/// @return MIMPI return code:
///         - `MIMPI_SUCCESS` if operation ended successfully.
///         - `MIMPI_ERROR_ATTEMPTED_SELF_OP` if process attempted to receive from itself
///         - `MIMPI_ERROR_NO_SUCH_RANK` if there is no process with rank
///           @ref source in the world.
///         - `MIMPI_ERROR_REMOTE_FINISHED` if the process with rank
///         - @ref source has already escaped _MPI block_.
///         - `MIMPI_ERROR_DEADLOCK_DETECTED` if a deadlock has been detected
///           and therefore this call would else never return.
///
MIMPI_Retcode MIMPI_Recv(
    void *data,
    int count,
    int source,
    int tag
);

/// @brief Synchronises all processes.
///
/// Blocks execution of the calling process until all processes execute
/// this function. In particular, every process executes all instructions
/// preceding the call before any process executes any instruction
/// following the call. 
///
/// @return MIMPI return code:
///         - `MIMPI_SUCCESS` if operation ended successfully.
///         - `MIMPI_ERROR_REMOTE_FINISHED` if any process in the world
///            has already escaped _MPI block_.
///         - `MIMPI_ERROR_DEADLOCK_DETECTED` if a deadlock has been detected
///           and therefore this call would else never return.
///
MIMPI_Retcode MIMPI_Barrier();

/// @brief Broadcasts data to all processes.
///
/// Makes @ref count bytes of data at address @ref data in process @ref root
/// available among all processes at address @ref data.
/// Additionally, is a synchronisation point similarly to @ref MIMPI_Barrier.
///
/// @param data - for @ref root, data to be broadcast; for other processes,
///               place where data are to be put.
/// @param count - number of bytes of data to be broadcast.
/// @param root - rank of the process whose data are to be broadcast.
///
/// @return MIMPI return code:
///         - `MIMPI_SUCCESS` if operation ended successfully.
///         - `MIMPI_ERROR_NO_SUCH_RANK` if there is no process with rank
///           @ref root in the world.
///         - `MIMPI_ERROR_REMOTE_FINISHED` if any process in the world
///            has already escaped _MPI block_.
///         - `MIMPI_ERROR_DEADLOCK_DETECTED` if a deadlock has been detected
///           and therefore this call would else never return.
///
MIMPI_Retcode MIMPI_Bcast(
    void *data,
    int count,
    int root
);

/// @brief Reduces data from all processes to one.
///
/// Performs reduction of kind @ref op over @ref count bytes of data
/// stored at address @ref send_data in every process. The reduction's result
/// is put at @ref recv_data *ONLY* in the process with rank @ref root.
/// Additionally, is a synchronisation point similarly to @ref MIMPI_Barrier.
///
/// @param send_data - data to be reduced.
/// @param recv_data - place where reduction's result is to be put.
/// @param count - number of bytes of data to be reduced.
/// @param op - a particular operation to be performed for reduction.
/// @param root - rank of the process who is to hold the result of reduction.
///
/// @return MIMPI return code:
///         - `MIMPI_SUCCESS` if operation ended successfully.
///         - `MIMPI_ERROR_NO_SUCH_RANK` if there is no process with rank
///           @ref root in the world.
///         - `MIMPI_ERROR_REMOTE_FINISHED` if any process in the world
///            has already escaped _MPI block_.
///         - `MIMPI_ERROR_DEADLOCK_DETECTED` if a deadlock has been detected
///           and therefore this call would else never return.
///
MIMPI_Retcode MIMPI_Reduce(
    void const *send_data,
    void *recv_data,
    int count,
    MIMPI_Op op,
    int root
);

#endif /* MIMPI_H */
