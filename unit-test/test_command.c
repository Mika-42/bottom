#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>

#ifndef MAX_LENGTH
#define MAX_LENGTH 255
#endif

typedef struct 
{
    bool help;
    bool dry_run;
    char remote_config[MAX_LENGTH];    
    char connexion_type[MAX_LENGTH];
    int port;
    char login[MAX_LENGTH]; 
    char remote_server[MAX_LENGTH]; 
    char username[MAX_LENGTH];
    char password[MAX_LENGTH]; 
    bool all;

} options_prog; 

typedef struct 
{
    char name_server[MAX_LENGTH];
    char adress_server[MAX_LENGTH];
    int port; 
    char username[MAX_LENGTH];
    char password[MAX_LENGTH]; 
    char type_co[7];
} fichier;

int command_run(int argc, char *argv[], options_prog *options);

static bool is_empty(const char *s) {
    return s[0] == '\0';
}

void setup_options(options_prog *options) {
    memset(options, 0, sizeof(options_prog)); 
}

#define ASSERT_EQ_STR(actual, expected) \
    if (strcmp(actual, expected) != 0) { \
        printf("\t❌ Echec: %s. Attendu: '%s', Obtenu: '%s'\n", #actual, expected, actual); \
        return 1; \
    }

#define ASSERT_EQ_INT(actual, expected) \
    if (actual != expected) { \
        printf("\t❌ Echec: %s. Attendu: %d, Obtenu: %d\n", #actual, expected, actual); \
        return 1; \
    }

#define ASSERT_TRUE(actual) \
    if (actual != true) { \
        printf("\t❌ Echec: %s. Attendu: TRUE\n", #actual); \
        return 1; \
    }

#define ASSERT_FALSE(actual) \
    if (actual != false) { \
        printf("\t❌ Echec: %s. Attendu: FALSE\n", #actual); \
        return 1; \
    }

int test_simple_connexion() {
    options_prog options;
    setup_options(&options);
    
    char *argv[] = {"./prog", "-s", "remote.srv", "-u", "user", "-p", "pass123"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    if (command_run(argc, argv, &options) != 0) return 1;

    ASSERT_EQ_STR(options.remote_server, "remote.srv");
    ASSERT_EQ_STR(options.username, "user");
    ASSERT_EQ_STR(options.password, "pass123");
    
    return 0;
}

int test_login_split() {
    options_prog options;
    setup_options(&options);
    
    char *argv[] = {"./prog", "-l", "admin@prod.example.com"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    if (command_run(argc, argv, &options) != 0) return 1;

    ASSERT_EQ_STR(options.login, "admin@prod.example.com");
    ASSERT_EQ_STR(options.username, "admin");
    ASSERT_EQ_STR(options.remote_server, "prod.example.com");
    
    return 0;
}

int test_default_port_ssh() {
    options_prog options;
    setup_options(&options);
    
    char *argv[] = {"./prog", "-s", "remote", "-t", "ssh"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    if (command_run(argc, argv, &options) != 0) return 1;

    ASSERT_EQ_STR(options.connexion_type, "ssh");
    ASSERT_EQ_INT(options.port, 22);
    
    return 0;
}

int test_port_override() {
    options_prog options;
    setup_options(&options);
    
    char *argv[] = {"./prog", "-s", "remote", "-t", "telnet", "-P", "8080"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    if (command_run(argc, argv, &options) != 0) return 1;

    ASSERT_EQ_STR(options.connexion_type, "telnet");
    ASSERT_EQ_INT(options.port, 8080);
    
    return 0;
}

int test_all_option() {
    options_prog options;
    setup_options(&options);
    
    FILE *tmp = fopen("test_config.conf", "w");
    if (tmp) {
        fprintf(tmp, "srv;127.0.0.1;22;user;pass;ssh\n");
        fclose(tmp);
        chmod("test_config.conf", 0600); 
    }

    char *argv[] = {"./prog", "-a", "-c", "test_config.conf"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    int result = command_run(argc, argv, &options);
    
    remove("test_config.conf");

    if (result != 0) return 1;

    ASSERT_EQ_STR(options.remote_config, "test_config.conf");
    ASSERT_TRUE(options.all);
    
    return 0;
}

int main() {
    int failures = 0;
    int total_tests = 0;

    printf("--- Lancement des tests unitaires ---\n");
    
    total_tests++;
    printf("Test 1: Simple Connexion (-s -u -p)... ");
    if (test_simple_connexion() == 0) printf("✅ Succès\n"); else failures++;

    total_tests++;
    printf("Test 2: Split Login (-l)... ");
    if (test_login_split() == 0) printf("✅ Succès\n"); else failures++;
    
    total_tests++;
    printf("Test 3: Port par defaut (ssh)... ");
    if (test_default_port_ssh() == 0) printf("✅ Succès\n"); else failures++;

    total_tests++;
    printf("Test 4: Port specifie (-P)... ");
    if (test_port_override() == 0) printf("✅ Succès\n"); else failures++;

    total_tests++;
    printf("Test 5: Option -a avec -c... ");
    if (test_all_option() == 0) printf("✅ Succès\n"); else failures++;
    
    printf("--------------------------------------\n");
    printf("Total des tests: %d, Echecs: %d\n", total_tests, failures);
    
    return failures;
}