#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "ssh_connexion.h"

int main(void) {
    char *host = "127.0.0.1";   // à adapter
    int port = 22;
    char *user = "testuser";   // à adapter
    char *password = "testpwd"; // à adapter

    printf("=== Test ssh_connexion_init ===\n");
    ssh_session session = ssh_connexion_init(host, port, user, password);

    /* Test 1 : la session doit être créée */
    assert(session != NULL);
    printf("OK : session créée\n");

    printf("=== Test ssh_dry_run ===\n");
    int ret = ssh_dry_run(session);

    /* Test 2 : la commande doit réussir */
    assert(ret == EXIT_SUCCESS);
    printf("OK : ssh_dry_run réussi\n");

    printf("=== Test ssh_end_session ===\n");
    ssh_end_session(session);
    printf("OK : session fermée\n");

    printf("=== Tous les tests sont passés avec succès ===\n");
    return EXIT_SUCCESS;
}
