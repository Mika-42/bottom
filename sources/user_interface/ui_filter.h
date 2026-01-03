#ifndef UI_FILTER_H
#define UI_FILTER_H

#include "processus_array.h"
#include "error.h"
#include "ui.h"

/**
 * @brief Filtre le tableau de processus par rapport à une entré utilisateur
 *
 * @param array Un tableau de processus
 * @param s La selection de l'utilisateur dont notemment le buffer de recherche, sa longueur et le tableau d'indice filtré de l'utilisateur
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t ui_filter_proc(const processus_array_t *array, user_selection_t *s);

#endif /* UI_FILTER_H */
