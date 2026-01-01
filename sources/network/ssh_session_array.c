#include "ssh_session_array.h"
#include <stdlib.h>

void ssh_array_init(ssh_session_array_t *array) {
    if (!array) {
        return;
    }
    array->data = nullptr;
    array->size = 0;
    array->capacity = 0;    
}

ssh_session *ssh_array_add(ssh_session_array_t *array, ssh_session session) {
    if (!array) {
        return nullptr;
    }

    if (array->size >= array->capacity) {
        const size_t new_capacity = array->capacity ? array->capacity * 2 : 1;

        ssh_session *temp = realloc(array->data, new_capacity * sizeof(*temp));

        if (!temp) {
            return nullptr;
        }

        array->data = temp;
        array->capacity = new_capacity;
    }

    array->data[array->size++] = session;

    return &array->data[array->size - 1];       
}

void ssh_array_free(ssh_session_array_t *array) {
    if (!array) {
        return;
    }
    if (array->data) {
        for (size_t i=0; i<array->size; ++i) {
            ssh_end_session(array->data[i]);
        }
    } 
    free(array->data);
    ssh_array_init(array);
}   
