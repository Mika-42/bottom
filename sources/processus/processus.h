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
    pid_t       pid;                			/**< ID du processus */
    pid_t       ppid;               			/**< ID du processus parent */
    char        name[PROC_NAME_SIZE];       		/**< Nom du processus */
    char        state;              			/**< Etat du processus */
    unsigned long long ram;             		/**< Mémoire utilisé en octets par le processus */
    char        user[PROC_USERNAME_SIZE];   		/**< Nom de l'utilisateur */
    unsigned long   utime;              		/**< Temps CPU utilisateur */
    unsigned long   stime;              		/**< Temps CPU système */
    unsigned long   start_time;         		/**< Durée de lancement */
    double      cpu_usage;          			/**< Pourcentage du CPU utilisé */

    char        executable[PROC_PATH_SIZE];		/**< Chemain de l'executable */
    char        env[PROC_CMD_COUNT][PROC_CMD_LEN];	/**< Variables d'environnements */
    char        cmdline[PROC_CMD_COUNT][PROC_CMD_LEN];	/**< Options de lancement*/
} processus_t;

/**
 * @brief Vérifie qu'une chaîne de caractère soit une nombre
 *
 * @param str La chaîne de caractère à vérifier
 *
 * @return Un booléen
 */
[[nodiscard]] bool str_is_numeric(const char *str);

/**
 * @brief Vérifie qu'une chaîne de caractère soit un pid valide
 *
 * @param pid Chaîne de caractère à vérifier
 *
 * @return Un booléen
 */
[[nodiscard]] bool proc_is_valid_pid(const char *pid);

/**
 * @brief Récupère l'utilisateur à qui appartient un processus
 *
 * @param proc Processus pour lequel récupérer l'utilisateur
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_get_user(processus_t *proc);

/**
 * @brief Récupère le contenue d'un fichier /proc/<pid>/stat dans la structure du processus
 *
 * @param proc Structure du processus pour lequel récupérer le contenue du fichier
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_get_stat(processus_t *proc);

/**
 * @brief Récupère le contenue du fichier /proc/stat
 *
 * @param cpu_total Entier dans lequel sera inscrit la consommation cpu total
 * @param boot_time Valeur dans laquelle sera inscrit le temps depuis lequel le système est en marche
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_get_global_stat(long *cpu_total, time_t *boot_time);

/**
 * @brief Récupère toute les informations que l'on à besoin dans la structure d'un processus
 *
 * @param pid Pid du processus
 * @param proc Structure processus_t du processus
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_get_all_infos(const pid_t pid, processus_t *proc);

/**
 * @brief Récupère l'executable d'un processus dans sa structure
 *
 * @param proc Structure du processus
 *
 * @return Un code d'erreur défini dans error.h
 */
void proc_get_exe(processus_t *proc);

/**
 * @brief Récupère les options de lancement dans la structure d'un processus
 *
 * @param p La structure du processus
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_get_cmdline(processus_t *p);

/**
 * @brief Récupère les variable d'environement dans la structure d'un processus
 *
 * @param p Structure du processus
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_get_env(processus_t *p);

#endif //PROCESSUS_H
