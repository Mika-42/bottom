#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <stddef.h>
#include "ui.h"

/**
 * @brief Limite un entier int à une valeur maximum et une valeur minimum
 *
 * @param value La valeur à limité
 * @param min_value La valeur minimal
 * @param max_value La valeur maximal
 */
void ui_utils_clamp_int(int *value, const int min_value, const int max_value);

/**
 * @brief Limite un entier size_t à une valeur maximum et une valeur minimum
 *
 * @param value La valeur à limité
 * @param min_value La valeur minimal
 * @param max_value La valeur maximal
 */
void ui_utils_clamp_size_t(size_t *value, const size_t min_value, const size_t max_value);

/**
 * @brief Réinitialise la selection utilisateur pour la recherche et la saisie de texte
 *
 * @param s Structure de la  selection utilisateur
 */
void ui_utils_reset_input_buffer(user_selection_t *s);

#endif /* UI_UTILS_H */
