#ifndef ERROR_H
#define ERROR_H

typedef enum error_t : int {
        SUCCESS,
        GENERIC_ERROR,
        NULLPTR_PARAMETER_ERROR,
        OPEN_FILE_FAILED,
        READ_FAILED,
        MALFORMED_STATUS_LINE,
        UID_NOT_FOUND,
        USER_NOT_FOUND,
        MEMORY_ALLOCATION_FAILED,
} error_t;

#endif // ERROR_H
