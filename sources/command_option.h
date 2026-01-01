#ifndef COMMAND_OPTION_H
#define COMMAND_OPTION_H

#include "config_file.h"
#include "option.h"

typedef struct flag_t {
    bool exec_local : 1;
    bool config : 1;
    bool server : 1;
    bool has_opt : 1;
    bool dry_run : 1;
} flag_t;

/**
 * @brief Récupère les option données avec le lancement du programme pour les gérer
 *
 * @param argc Nombre d'argument
 * @param argv Tableau des différents arguments donnés
 * @param flag Structure dans laquel est stocké différent flag permettant de savoir quoi lancer plus tard
 * @param sessions Sessions SSH des différentes machines connectées au programme
 * @param cfg_file Strucure qui va contenir les inforamtions de connexion de toutes les machines distantes récupérées depuis le fichier de configuration
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t command_run(int argc, char *argv[], flag_t *flag, ssh_session_array_t *sessions, config_file_t *cfg_file);

#endif 
