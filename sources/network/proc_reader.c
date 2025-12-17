#include "../processus/processus.h"
#include "../error.h"

int read_processus(char *buffer, processus_t *p) {
    if (buffer == NULL || p == NULL) {
        return NULLPTR_PARAMETER_ERROR; 
    }
    
}

int parse_stat(char *buffer, processus_t *p) {
    if (buffer == NULL || p == NULL) {
        return NULLPTR_PARAMETER_ERROR; 
    }
    int element=sscanf(buffer, "%d (%[^)]) %c %d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %*ld %*ld %*ld %*ld %*ld %*ld %lu",
                     &p->pid,
                     p->name,
                     &p->state,
                     &p->ppid,
                     &p->utime,
                     &p->stime,
                     &p->start_time);
    ///on mets les elements dans la struct et si ils sont pas tous mis ca renvoie l'erreur

    return (element == 7) ? 0 : NULLPTR_PARAMETER_ERROR; 
}

int parse_status(char *buffer, processus_t *p) {
    if (buffer == NULL || p == NULL) {
        return NULLPTR_PARAMETER_ERROR; 
    }
    char *ptr_ram = strstr(buffer, "VmRSS:");
    if (ptr_ram != NULL) {
        sscanf(ptr_ram, "VmRSS: %llu", &p->ram);
    } 

    char *ptr_uid = strstr(buffer, "Uid:");
    if (ptr_uid != NULL) {
        sscanf(ptr_uid, "Uid: %*d", &p->user);
    }
    return 0; 
}

int parse_cmdline(char *buffer, processus_t *p) {
    if (buffer == NULL || p == NULL) {
        return NULLPTR_PARAMETER_ERROR; 
    }
    
}

