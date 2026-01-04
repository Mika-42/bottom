#ifndef THREAD_H
#define THREAD_H

#include <stdatomic.h>
#include <pthread.h>
#include "processus_array.h"
#include "ui_constant.h"
#include "ssh_session_array.h"
#include "ui.h"

typedef struct double_buffer_t {
    processus_array_t buffer[2];
    atomic_int active;
} double_buffer_t;

/**
 * @brief Structure contenant les arguments d'un thread
 */
typedef struct thread_args_t {
    //unique
    user_selection_t *selection;	/**< Selection qui définie plusieurs chose propre à chaque machine */

    //shared
    double_buffer_t *array;			/**< Un double buffer */
    ssh_session session;			/**< Session SSH de la machine */
    char machine_name[32];			/**< Nom de la machine */
} thread_args_t;

/**
 * @brief Met à jour en continue une liste de processus en local ou en connexion SSH
 * 
 * @param arg Pointeur générique pointant vers une structure thread_args_t
 */
void *proc_task(void *arg);

/**
 * @brief Met à jour en continue l'affichage des processus et de lire le clavier
 *
 * @param arg Pointeur générique pointant vers une structure thread_args_t
 */
void *ui_task(void *arg);

/**
 * @brief Permet d'agir en continue sur un processus en local ou en connexion SSH à partir d'une selection utilisateur
 *
 * @param arg Pointeur générique pointant vers une structure thread_args_t
 */
void *manager_task(void *arg);

#endif /* THREAD_H */
