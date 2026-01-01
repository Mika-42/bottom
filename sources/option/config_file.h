#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <stddef.h>
#include "remote_server.h"

constexpr size_t path_max_size = 4096;
constexpr char cfg_filename[path_max_size] = ".config";

/**
 * @brief Tableau serveur contenue dans un fichier de configuration
 */
typedef struct config_file_t {
    remote_server_t *data;	/**< Tableau de serveur */
    size_t size;		/**< Nombre de serveur dans le tableau */
    size_t capacity;		/**< Capacité maximum d'élément du tableau (taille allouée) */
} config_file_t;

/**
 * @brief Initialise un tableau de serveur
 *
 * @param file Tableau à initialiser 
 */
void cfg_init(config_file_t *file);

/**
 * @brief Libère la mémoire allouée à un tableau de serveur
 *
 * @param file Tableau à libérer
 */
void cfg_free(config_file_t *file);

/**
 * @brief Ajoute un server à un tableau de serveur
 *
 * @param file Tableau de serveur
 * @param server Serveur à ajouter au tableau
 *
 * @return Un code d'erreur défini dans error.h
 */
[[maybe_unused]] remote_server_t *cfg_add_server(config_file_t *file, remote_server_t *server);

/**
 * @brief Vérifie si le fichier donné s'appel ".config"
 *
 * @param path Chemin vers le fichier
 *
 * @return Un booléen
 */
[[nodiscard]] bool cfg_is_dot_config(const char *path);

/**
 * @brief Vérifie que les pérmissions sur le fichier sont bonne
 *
 * @param path Chemin vers le fichier
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t cfg_is_600(const char *path);

/**
 * @brief Récupère les information d'un fichier de configuration pour ajouté un serveur à un tableau de serveur
 *
 * @param file Tableau de serveur dans lequel sera ajouté le serveur si le fichier de configuration est bon
 * @param path Chemin vers un fichier de configuration à ajouter
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t cfg_parse(config_file_t *file, const char *path);

#endif /* CONFIG_FILE_H */
