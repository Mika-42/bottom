#ifndef OPTION_H
#define OPTION_H

#include <stdio.h>
#include "error.h"
#include "config_file.h"
#include "ssh_connexion.h"
#include "ssh_session_array.h"

/**
 * @brief Affiche le help du programme
 */
void opt_print_help();

/**
 * @brief Demande quelque chose à l'utilisateur 
 *
 * @param field_nam Question à poser / message à écrire au début
 * @param dest Destination de ce que va écrire l'utilisateur
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t opt_ask_user(const char *field_name, char *dest);

/**
 * @brief Effectue une connexion à une ou plusieurs machines distantes pour tester la connexion
 *
 * @param file Tableau de serveur sur lesquels effectuer le teste
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] error_code_t opt_dry_run(const config_file_t *file);

/**
 * @brief Initialise la connexion avec une ou plusieurs machine distante et entre les sessions de connexion dans un tableau
 *
 * @param file Tableau de serveur sur lesquels le programme va se connecter
 * @param array Tableau des sessions SSH pour toutes les machines de la liste de serveur
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t opt_connect(const config_file_t *file, ssh_session_array_t *array);

#endif /* OPTION_H */
