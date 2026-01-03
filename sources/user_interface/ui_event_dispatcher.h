#ifndef UI_EVENT_DISPATCHER_H
#define UI_EVENT_DISPATCHER_H

#include "processus_array.h"
#include "ui.h"
#include "error.h"

/**
 * @brief Récupère les touches du clavier entrée dans son mode "normal" et déclanche les événements liées
 *
 * @param array Un tableau de processus
 * @param ch La touche du clavier entrée
 * @param ui Une interface utilisateur
 * @param s L'état actuel de l'interface
 */
void ui_event_dispatcher_normal(const processus_array_t *array, const int ch,ui_t *ui, user_selection_t *s);

/**
 * @brief Récupère les touches du clavier entrée dans son mode "help" et déclanche les événements liées
 *
 * @param ch La touche du clavier entrée
 * @param ui Une interface utilisateur
 * @param s L'état actuel de l'interface
 */
void ui_event_dispatcher_help(const int ch, ui_t *ui, user_selection_t *s);

/**
 * @brief Récupère les touches du clavier entrée dans son mode "recherche" et déclanche les événements liées
 *
 * @param ch La touche du clavier entrée
 * @param ui Une interface utilisateur
 * @param s L'état actuel de l'interface
 */
void ui_event_dispatcher_search(const int ch, ui_t *ui, user_selection_t *s);

/**
 * @brief Récupère les touches du clavier entrée dans son mode "trie" et déclanche les événements liées
 *
 * @param ch La touche du clavier entrée
 * @param s L'état actuel de l'interface
 */
void ui_event_dispatcher_sort(const int ch, user_selection_t *s);

/**
 * @brief Récupère l'entrée des flèches de gauche et de droite
 *
 * @param ch La touche du clavier entrée
 *
 * @return Un entier <0 si gauche et >0 si droite, sinon 0
 */
[[nodiscard]] int ui_event_dispatcher_global(const int ch);

#endif /* UI_EVENT_DISPATCHER_H */
