#ifndef UI_PAGE_H
#define UI_PAGE_H

#include <ncurses.h>
#include "processus_array.h"
#include "ui_constant.h"
#include "ui.h"
#include "error.h"

/**
 * @brief Affiche un tableau de chaîne de caractère dans une fenêtre
 *
 * @param win Fenêtre dans laquelle afficher le tableau
 * @param array Tableau de chaîne de caractère à afficher
 */
void ui_show_array(WINDOW *win, const char (*array)[ui_max_width]);

/**
 * @brief Affiche l'en-tête sur l'interface utilisateur et la met à jour
 *
 * @param header_selected Colonne actuellement selectionnée
 * @param ui Structure de l'interface utilisateur
 * @param asc Un booléen qui est vrai si le tri doit être ascendant
 */
void ui_show_header(const size_t header_selected, ui_t *ui, const bool asc);

/**
 * @brief Affiche la fenêtre d'aide
 *
 * @param ui Structure de l'interface utilisateur
 */
void show_help_page(ui_t *ui);

/**
 * @brief Affiche un tableau de processus dans la fenêtre principal du programme
 *
 * @param array Un tableau de processus
 * @param ui Structure de l'interface utilisateur
 * @param s Selection de l'utilisateur dans l'interface
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t ui_show_proc(const processus_array_t *array, ui_t *ui, user_selection_t *s);

#endif /* UI_PAGE_H */
