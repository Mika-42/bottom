#ifndef ERROR_H
#define ERROR_H

typedef enum proc_err_t : int {
        SUCCESS,
        GENERIC_ERROR,
        NULLPTR_PARAMETER_ERROR,
        OPEN_FILE_FAILED,
        READ_FAILED,
        MALFORMED_STATUS_LINE,
        UID_NOT_FOUND,
        USER_NOT_FOUND,
        MEMORY_ALLOCATION_FAILED,
} proc_err_t;

#endif // ERROR_H
