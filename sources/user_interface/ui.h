#ifndef UI_H
#define UI_H

#include "processus_array.h"
#include <ncurses.h>
#include "ui_constant.h"
#include "ui_index_array.h"
#include <pthread.h>


/**
 * @brief Structure qui représentant l'état de sélection et d'interaction utilisateur
 */
typedef struct user_selection_t {
    size_t			selected;
    size_t			machine_selected;
    size_t			header_selected;
    sort_type_t     sort;   // proc_task read-only
    user_mode_t     mode;   // proc_task read-only
    proc_event_t    event;  // proc_task read-write
    size_t			max_machine;
    char			input[256];
    size_t			input_length;
    index_array_t   indices;
    pthread_mutex_t lock;
} user_selection_t;


/**
 * @brief Structure regroupant les éléments de l'interface utilisateur
 */
typedef struct ui_t {
    WINDOW *pad;		/**< Fenêtre principal */
    WINDOW *footer;		/**< Fenêtre de pied de page */
    WINDOW *header;		/**< Fenêtre d'en-tête de page */
    int ui_scroll_x;	/**< Décalage horizontal du défilement */
    int ui_scroll_y;	/**< Décalage vertical du défilement */
} ui_t;

/**
 * @brief Initialise une interface utilisateur
 *
 * @param ui Une interface utilisateur
 */
void ui_init(ui_t *ui);

/**
 * @brief Déinitialise une interface utilisateur
 *
 * @param ui Une interface utilisateur
 */
void ui_deinit(ui_t *ui);

/**
 * @brief Met à jour une interface utilisateur à partir de la taille du terminal
 *
 * @param ui Un interface utilisateur
 * @param size Nombre de données affichées dans la fenêtre principale
 */
void ui_update(ui_t *ui, const size_t size);

/**
 * @brief Met à jour le défilement de l'interface
 *
 * @param ui Une interface utilisateur
 * @param selected L'élément actuellement sélectionné
 */
void ui_scroll(ui_t *ui, const int dx, const size_t selected);

/**
 * @brief Met à jour la selection courante dans la fenêtre principale
 *
 * @param ui Une interface utilisateur
 * @param s Etat courant de l'interface
 */
void ui_select(ui_t *ui, user_selection_t *s);

#endif // UI_H
