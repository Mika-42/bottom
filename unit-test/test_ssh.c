#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libssh/libssh.h>

#include "ssh_connexion.h"

/* Compteurs globaux */
static int tests_total = 0;
static int tests_failed = 0;

/* Fonction pour afficher le résultat d'un test */
void run_test(const char *test_name, int condition, const char *error_message) {
    tests_total++;
    if (condition) {
        printf("[ OK ] %s\n", test_name);
    } else {
        printf("[FAIL] %s -> %s\n", test_name, error_message);
        tests_failed++;
    }
}

int main(void) {
    ssh_session session = NULL;

    /* ⚠️ À adapter à ton serveur SSH */
    char *host = "127.0.0.1";
    int port = 22;
    char *user = "testuser";
    char *password = "testpwd";

    printf("=== Début des tests ssh_connexion ===\n\n");

    /* Test 1 : création de la session */
    session = ssh_connexion_init(host, port, user, password);
    run_test("ssh_connexion_init retourne une session valide",
             session != NULL,
             "Impossible de se connecter. Vérifie host/user/password ou le serveur SSH.");

    /* Test 2 : dry run */
    if (session != NULL) {
        int ret = ssh_dry_run(session);
        run_test("ssh_dry_run retourne EXIT_SUCCESS",
                 ret == EXIT_SUCCESS,
                 "La commande 'ps' n'a pas pu être exécutée sur le serveur.");
    } else {
        run_test("ssh_dry_run", 0, "Session invalide, test non exécuté.");
    }

    /* Test 3 : fermeture de session */
    if (session != NULL) {
        ssh_end_session(session);
        run_test("ssh_end_session exécuté sans crash", 1, "");
    }

    /* Résumé final */
    printf("\n=== Résumé des tests ===\n");
    printf("Tests exécutés : %d\n", tests_total);
    printf("Tests réussis  : %d\n", tests_total - tests_failed);
    printf("Tests échoués  : %d\n", tests_failed);

    if (tests_failed == 0) {
        printf("\n✅ TOUS LES TESTS SONT PASSÉS\n");
        return EXIT_SUCCESS;
    } else {
        printf("\n❌ CERTAINS TESTS ONT ÉCHOUÉ\n");
        return EXIT_FAILURE;
    }
}
