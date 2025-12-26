#include <libssh/libssh.h>
#include <stdio.h>
#include <pw.h>
#include "ssh_connexion.h"

int print_intro() {
	char *presentation = "Pour faire ce teste unitaire, il vous faut openssh d'installé sur votre machine, si ce n'est pas le cas: \n"
		"sudo pacman -S openssh               pour le système de package pacman\n"
		"ou\n"
		"sudo apt install openssh-server      pour le système de package apt\n"
		"Ensuite il faut que ssh soit lancé avec la commende:\n"
		"sudo systemctl sshd start            si installé avec pacman\n"
		"ou\n"
		"sudo systemctl ssh start             si installé avec apt\n";
	printf("%s\n", presentation);
}

int main() {
	print_intro();

	char *host = localhost;
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);
	char *user;
	if (!pw) {
		printf("Le programme n'a pas réussi à lire votre nom d'utilisateur, entrez le svp\n");
		scranf("%s", &user);
	} else {
		user = pw->pw_name;
	}
	char *password;
	
	printf("Entrez votre mdp:");
	scanf("%s", &password);
	

	ssh_connexion_init(host, 22, user, password);

	if (!session) {
		printf("Erreur lors de la connextion ssh: %s", ssh_get_error(session));
	} else {
		printf("Connection ssh fait avec succès");
	}

	ssh_end_session(session);

	return 0;
}
