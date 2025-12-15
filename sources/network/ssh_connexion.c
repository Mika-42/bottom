#include "ssh_connexion.h"

#include <libssh/libssh.h>
#include <stdlib.h>

void ssh_end_session(ssh_session session) {
	ssh_disconnect(session);
	ssh_free(session);
}

ssh_session ssh_connexion_init(char *host, int port, char *user, char *password) {
	ssh_session session = ssh_new();
	if (session == nullptr) return nullptr;

	ssh_options_set(session, SSH_OPTIONS_HOST, host);
	ssh_options_set(session, SSH_OPTIONS_PORT, &port);
	ssh_options_set(session, SSH_OPTIONS_USER, user);

	if (ssh_connect(session) != SSH_OK) {
		ssh_free(session);
		return nullptr;
	}

	if (ssh_userauth_password(session, nullptr, password) != SSH_AUTH_SUCCESS) {
		ssh_end_session(session);
		return nullptr;
	}
	return session;
}

int ssh_cmd_exec(ssh_session session, char *cmd, char *output, size_t len_max) {
	ssh_channel channel = ssh_channel_new(session);
	if (channel == nullptr) return EXIT_FAILURE;
	if (ssh_channel_open_session(channel) != SSH_OK) {
		ssh_channel_free(channel);
		return EXIT_FAILURE;
	}

	if (ssh_channel_request_exec(channel, cmd) != SSH_OK) {
		ssh_channel_close(channel);
		ssh_channel_free(channel);
		return EXIT_FAILURE;
	}

	int nbytes = ssh_channel_read(channel, output, len_max - 1, 0);
	if (nbytes >= 0) output[nbytes] = '\0';
	
	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);

	return (nbytes >= 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int ssh_dry_run(ssh_session session) {
	char output[4096];
	return ssh_cmd_exec(session, "ps", output, sizeof(output));
}
