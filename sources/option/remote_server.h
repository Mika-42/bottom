#ifndef REMOTE_SERVER_H
#define REMOTE_SERVER_H

#include <stddef.h>
#include "error.h"

constexpr size_t field_size = 256;
constexpr size_t max_port = 65'535;

typedef enum connexion_type_t {
    SSH, TELNET
} connexion_type_t;

/**
 * @brief Structure contenant toutes les informations de connexion avec une machine distante
 */
typedef struct remote_server_t {
    char name[field_size];			/**< Nom de la machine distante */
    char address[field_size];		/**< Address de la machine distante */
    char username[field_size];		/**< Nom d'utilisateur pour se connecter à la maachine distante */
    char password[field_size];  	/**< Mot de passe de l'utilisateur */
    unsigned short port;			/**< Port à utiliser pour la connexion */
    connexion_type_t connexion;		/**< Type de connexion */
} remote_server_t;

/**
 * @brief Copie une chaîne de caractère ailleurs
 *
 * @param dest Destination de la copie
 * @param src Chaîne de caractère à copier
 *
 * @return Un code d'erreur définin dans error.h
 */
[[nodiscard]] error_code_t srv_str_duplicate(char *dest, char *src);

/**
 * @brief Copie une chaîne de caractère ailleurs en la transformant en entier non signé
 *
 * @param dest Destination de la copie
 * @param src Chaîne de caractère à copier
 *
 * @return Un code d'erreur défini dans error.h 
 */
[[nodiscard]] error_code_t srv_str_to_uint16(unsigned short *dest, char *src);

/**
 * @brief Copie une chaîne de caractère ailleurs en la transformant en type de connexion
 * 
 * @param dest Destination de la copie
 * @param src Chaîne de caractère à copier
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t srv_str_to_connexion(connexion_type_t *dest, char *src);

/**
 * @brief Copie une chaîne de caractère contenant un '@' et copie la partie de droite et de gauche à deux endroit différent
 *
 * @param dest_lhs Destination de la partie de gauche
 * @param dest_rhs Destination de la partie de droite
 * @param src Chaîne de caractère à copier
 *
 * @return Un code d'erreur défini dans error.h
 */
[[nodiscard]] error_code_t srv_str_split_at(char *dest_lhs, char *dest_rhs, char *src);

/**
 * @brief Vérifie si une chaîne de caractère est vide
 *
 * @param str Chaîne de caractère à vérifier
 * 
 * @return Un booléen
 */
[[nodiscard]] bool srv_str_is_empty(char *str);

/**
 * @brief Vérifie si une structure de serveur est valide
 *
 * @param server Structure d'un serveur à vérifier
 *
 * @return Un booléen
 */
[[nodiscard]] bool srv_is_valid(remote_server_t *server);

#endif /* REMOTE_SERVER_H */
