#include "ssh_connexion.h"
#include <libssh/libssh.h>
#include <stdio.h>
#include <stdlib.h>

void ssh_end_session(ssh_session session) {
	ssh_disconnect(session);
	ssh_free(session);
}


ssh_session ssh_connexion_init(const char *host, int port, const char *user, const char *password) {
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

int ssh_cmd_exec(ssh_session session, const char *cmd, char *output, size_t len_max) {
	ssh_channel channel = ssh_channel_new(session);
	if (!channel) return SSH_ERROR;
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
	if (nbytes >= 0 && output != nullptr)  output[nbytes] = '\0';
	
	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);

	return (nbytes >= 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int ssh_dry_run(ssh_session session) {
	char output[4096];
	return ssh_cmd_exec(session, "ps", output, sizeof(output));
}

int ssh_kill_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -KILL %d", pid);
	return ssh_cmd_exec(session, cmd, nullptr, 0);
}

int ssh_term_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -TERM %d", pid);
	return ssh_cmd_exec(session, cmd, nullptr, 0);
}

int ssh_stop_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -STOP %d", pid);
	return ssh_cmd_exec(session, cmd, nullptr, 0);
}

int ssh_cont_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -CONT %d", pid);
	return ssh_cmd_exec(session, cmd, nullptr, 0);
}

/**
 * @brief Met le contenue du fichier /proc/file dans le buffer
 *
 * @param session Session ssh
 * @param buffer Adresse de la chaine de caractère dans laquel sera écrit le contenue du fichier, à liberer par l'appelant
 * @param file Fichier du dossier /proc/ qui sera lu
 * @return Code d'erreur correspondant de libssh
 */
int ssh_get_file(ssh_session session, char **buffer, const char *file) {
	for (const char *p = file; *p; ++p) {
		if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '/' || *p == '_')) {
			return SSH_ERROR;
		}
	}
	
	int n;
	char *buf = nullptr;
	size_t size = 0, capacity = 8192;

	ssh_channel channel = ssh_channel_new(session);
	if (!channel) return SSH_ERROR;
	
	if (ssh_channel_open_session(channel) != SSH_OK) goto error;

	char cmd[128];
	snprintf(cmd, sizeof(cmd), "cat /proc/%s", file);

	if (ssh_channel_request_exec(channel, cmd) != SSH_OK) goto error;
	
	buf = malloc(capacity + 1);
	if (!buf) goto error;
		
	
	while ((n = ssh_channel_read(channel, buf + size, capacity - size, 0)) > 0) {
		size += n;
		if (size == capacity) {
			capacity *= 2;
			char *tmp = realloc(buf, capacity +1);
			if (!tmp) goto error;
			buf = tmp;
		}
	}
 
	if (n < 0) goto error;
 	
	buf[size] = '\0';
	*buffer = buf;
	
	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);
 
	return SSH_OK;

error:
	free(buf);
	if (channel) {
		ssh_channel_send_eof(channel);
		ssh_channel_close(channel);
		ssh_channel_free(channel);
	}
	return SSH_ERROR;
}
