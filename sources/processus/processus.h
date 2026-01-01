#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <time.h>
#include <sys/types.h>
#include "error.h"

constexpr size_t PROC_CMD_COUNT = 256;
constexpr size_t PROC_CMD_LEN = 1024;
constexpr size_t PROC_NAME_SIZE = 256;
constexpr size_t PROC_USERNAME_SIZE = 32;
constexpr size_t PROC_PATH_SIZE = 4096;
constexpr size_t buf_max_size = 4096;

/**
 * @brief Represente un processus avec toutes les informations dont on a besoin.
 */
typedef struct processus_t {  
    pid_t       pid;                /**< ID du processus */
    pid_t       ppid;               /**< ID du processus parent */
    char        name[PROC_NAME_SIZE];       /**< Nom du processus */
    char        state;              /**< Etat du processus */
    unsigned long long ram;             /**< Mémoire utilisé en octets par le processus */
    char        user[PROC_USERNAME_SIZE];   /**< Nom de l'utilisateur */
    unsigned long   utime;              /**< Temps CPU utilisateur */
    unsigned long   stime;              /**< Temps CPU système */
    unsigned long   start_time;         /**< Durée de lancement */
    double      cpu_usage;          /**< Pourcentage du CPU utilisé */

    char        executable[PROC_PATH_SIZE];
    char        env[PROC_CMD_COUNT][PROC_CMD_LEN];
    char        cmdline[PROC_CMD_COUNT][PROC_CMD_LEN];
} processus_t;

[[nodiscard]] bool str_is_numeric(const char *str);
[[nodiscard]] bool proc_is_valid_pid(const char *pid);
[[nodiscard]] error_code_t proc_get_user(processus_t *proc);
[[nodiscard]] error_code_t proc_get_stat(processus_t *proc);
[[nodiscard]] error_code_t proc_get_global_stat(long *cpu_total, time_t *boot_time);
[[nodiscard]] error_code_t proc_get_all_infos(const pid_t pid, processus_t *proc);
void proc_get_exe(processus_t *proc);

[[nodiscard]] error_code_t proc_read_null_separated(pid_t pid, const char *proc_file, char out[][PROC_CMD_LEN]);
[[nodiscard]] error_code_t proc_get_cmdline(processus_t *p);
[[nodiscard]] error_code_t proc_get_env(processus_t *p);

#endif //PROCESSUS_H
