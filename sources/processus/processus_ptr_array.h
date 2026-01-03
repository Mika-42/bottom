#ifndef PROCESSUS_PTR_ARRAY_H
#define PROCESSUS_PTR_ARRAY_H

#include "error.h"
#include <time.h>

// Forward declaration (prevent circular include)
typedef struct processus_t processus_t;
/**
 * @brief Represente un tableau de processus par un pointeur vers un tableau
 */
typedef struct processus_ptr_array_t {
    processus_t **data;		/**< Pointeur vers les données du tableau */
    size_t      size;		/**< Nombre d'éléments dans le tableau */
    size_t      capacity;	/**< Capacité maximum du tableau (mémoire allouée) */
} processus_ptr_array_t;

/**
 * @brief Inisialise un tableau de processus par pointeur
 *
 * @param array Un tableau de processus par pointeur
 */
void        proc_ptr_array_init(processus_ptr_array_t *array);

/**
 * @brief Remise à zéro d'un tableau de processus par pointeur
 *
 * @param array Un tableau de processus par pointeur
 */
void        proc_ptr_array_reset(processus_ptr_array_t *array);*

/**
 * @brief Augmente la mémoire allouée à un tableau de processus par pointeur si besoin et ajoute un processus au tableau
 *
 * @param array Un tableau de processus par pointeur
 * @param proc Un processus à ajouter au tableau
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t proc_ptr_array_emplace_back(processus_ptr_array_t *array, processus_t *proc);

/**
 * @brief Libère la mémoire allouée à un tableau de processus par pointeur
 *
 * @param array Un tableau de processus par pointeur
 */
void        proc_ptr_array_free(processus_ptr_array_t *array);

#endif //PROCESSUS_PTR_ARRAY_H
