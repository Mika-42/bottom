#ifndef STAT_PARSER_H
#define STAT_PARSER_H

#include "error.h"
#include "processus.h"

/**
 * @brief Récupère dans une ligne de /proc/pid/stat, les informations d'un processus
 *
 * @param proc Structure des informations d'un processus
 * @param line Ligne du fichier stat d'un processus
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t stat_stat_parser(processus_t *proc, char *line);

/**
 * @brief Récupère les données de stat globale des processus
 *
 * @param cpu_total Pointeur vers une valeur qui sera l'utilisation du CPU total
 * @param boot_time Pointeur vers une valeur qui sera le temps depuis lequel le système est en marche
 * @param line Ligne du fichier /proc/stat
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t stat_global_stat_parser(long *cpu_total, time_t *boot_time, char *line);

/**
 * @brief Permet de savoir à quel utilisateur appartient un processus
 *
 * @param proc Processus à analyser
 * @param line Ligne de /proc/pid/status
 * 
 * @return Un code d'erreur de error.h
 */
[[nodiscard]] error_code_t stat_user_parser(processus_t *proc, char *line);

/**
 * @brief Permet de transformer un buffer avec des '\0' comme séparateur en tableau de chaîne de caractère
 *
 * @param buffer Le buffer à parser
 * @param size Taille du buffer 
 * @param out Le tableau de chaîne de caractère dans lequel sera écrit les différente chaîne de caractère contenue dans le buffer
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t stat_null_separated_parser(char *buffer, size_t size, char out[][PROC_CMD_LEN]);

#endif /* STAT_PARSER_H */
