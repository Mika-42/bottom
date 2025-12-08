#include "ssh_connexion.h"

#include <libssh/libssh.h>
#include <stdlib.h>

void ssh_end_session(ssh_session session) {
	ssh_disconnect(session);
	ssh_free(session);
}

ssh_session ssh_connexion_init(char *host, int port, char *user, char *password) {
	ssh_session session = ssh_new();
	if (session == NULL) return NULL;

	ssh_options_set(session, SSH_OPTIONS_HOST, host);
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);
	ssh_options_set(session, SSH_OPTIONS_USER, user);

	if (ssh_connect(session) != SSH_OK) {
		ssh_free(session);
		return NULL;
	}

	if (ssh_userauth_password(session, NULL, password) != SSH_AUTH_SUCCESS) {
		ssh_end_session(session);
		return NULL;
	}
	return session;
}

int ssh_dry_run(ssh_session session) {
	ssh_channel channel;

	channel = ssh_channel_new(session);
	if (channel == NULL) return EXIT_FAILURE;
	if (ssh_channel_open_session(channel) != SSH_OK) {
		ssh_channel_free(channel);
		return EXIT_FAILURE;
	}

	if (ssh_channel_request_exec(channel, "ps") != SSH_OK) {
		ssh_channel_close(channel);
		ssh_channel_free(channel);
		return EXIT_FAILURE;
	}

	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);

	return EXIT_SUCCESS;
}
