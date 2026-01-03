#ifndef UI_FORMAT_H
#define UI_FORMAT_H

#include <stddef.h>

/** 
 * @brief Permet de récupérer le nom complet de l'état d'un processus à partir de la lettre correspondant à l'état
 *
 * @param state L'état du processus en une lettre comme écrit dans /proc/<pid>/state
 *
 * @return Une chaîne de caractère étant l'état complet d'un processus
 */
[[nodiscard]] const char *ui_format_state(const char state);

/**
 * @brief Permet d'obtenir l'unité de mémoire à utiliser pour une certaine quantité de mémoire en divisant la mémoire pour avoir des plus petite valeur
 *
 * @param rss_bytes Quantité de mémoire à transformer
 * @param value Valeur qui deviendra la quantité de mémoire avec l'unité de sortie final
 *
 * @return L'unité de mémoire à utiliser
 */
[[nodiscard]] const char *ui_format_ram(const unsigned long long rss_bytes, double *value);

/**
 * @brief Convertie un nombre de tick processus en seconde, minute, heure et jour
 *
 * @param ticks Nombre de tick à convertir
 * @param buf Un buffer dans lequel sera écrit le temps sous format "jour:heure:minute:seconde"
 * @param bufsize Taille du buffer
 */
void ui_format_time(unsigned long ticks, char *buf, const size_t bufsize);

#endif //UI_FORMAT_H
