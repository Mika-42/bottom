#ifndef UI_INDEX_ARRAY_H
#define UI_INDEX_ARRAY_H

#include <stddef.h>
#include "error.h"

/**
 * @brief Structure représentant un tableau d'index
 */
typedef struct index_array_t {
    size_t *data;	/**< Tableau d'index */
    size_t size;	/**< Nombre d'index dans le tableau */
    size_t capacity;	/**< Capacité maximum du tableau (mémoire allouée) */
} index_array_t;

/**
 * @brief Réinitialise un tableau d'index
 *
 * @param array Tableau d'index à réinitialiser
 */
void ui_index_array_reset(index_array_t *array);

/**
 * @brief Ajoute un index au tableau d'index en augmentant la mémoire allouée si besoin
 *
 * @param array Un tableau d'index
 * @param value Index à ajouté au tableau
 *
 * @return Le dernière élément du tableau 
 */
[[nodiscard]] size_t *ui_index_array_emplace_back(index_array_t *array, size_t value);

/**
 * @brief Libère la mémoire d'un tableau d'index
 *
 * @param array Tableau d'index à libérer
 */
void ui_index_array_free(index_array_t *array);

#endif /* UI_INDEX_ARRAY_H */
