#include "../processus/processus.h"
#include "../error.h"
#include <libssh2.h>

unsigned long NOUVEAU_TEMPS_CPU = 0; 
unsigned long ANCIEN_TEMPS_CPU = 0;

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

    return (element == 7) ? 0 : PARSING_FAILED; 
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
        sscanf(ptr_uid, "Uid: %s", p->user);
    }
    return 0; 
}

void temps_cpu(char *buffer) {
    unsigned long user, nice, system, idle, iowait, irq, softirq;
    ANCIEN_TEMPS_CPU = NOUVEAU_TEMPS_CPU;

    if (sscanf(buffer, "cpu %lu %lu %lu %lu %lu %lu %lu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq) >= 4) {
        NOUVEAU_TEMPS_CPU = user + nice + system + idle + iowait + irq + softirq;
    } ///lis et additionne tout les temps de cpu pour pouvoir calculer le % du cpu
}

double calcul_cpu(const processus_t *p_now, const processus_t *p_prev) {
    
    unsigned long temps_total_now = p_now->utime + p_now->stime;
    unsigned long temps_total_old = p_prev->utime + p_prev->stime;

    unsigned long delta_proc = temps_total_now - temps_total_old;
    unsigned long delta_machine = NOUVEAU_TEMPS_CPU - ANCIEN_TEMPS_CPU;
    
    if (delta_machine == 0 || ANCIEN_TEMPS_CPU == 0) return 0.0;

    return ((double)delta_proc / (double)delta_machine) * 100.0;
} /// on calcul le % du cpu utilise par le processus


int read_processus(char *buf_stat, char *buf_status, processus_t *p) {
    if (p == NULL) {
        return NULLPTR_PARAMETER_ERROR; 
    }

    parse_stat(buf_stat, p);
    parse_status(buf_status, p);
    return 0;
}
