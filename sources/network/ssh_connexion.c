#include "ssh_connexion.h"
#include "processus.h"

#include <libssh/libssh.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void ssh_end_session(ssh_session session) {
	ssh_disconnect(session);
	ssh_free(session);
}

ssh_session ssh_connexion_init(const char *host, int port, const char *user, const char *password) {

	if (!host || !user || !password) {
		return nullptr;
	}

	ssh_session session = ssh_new();
	if (session == nullptr) {
		return nullptr;
	}

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

error_code_t ssh_cmd_exec(ssh_session session, char *buffer, size_t buffer_size, const char *cmd) {
	if(!session || !cmd) return NULLPTR_PARAMETER_ERROR;

	ssh_channel channel = ssh_channel_new(session);
	if (!channel) {
		return MEMORY_ALLOCATION_FAILED;
	}

	if (ssh_channel_open_session(channel) != SSH_OK) {
		ssh_channel_free(channel);
		return SSH_OPEN_FAILED;
	}

	if (ssh_channel_request_exec(channel, cmd) != SSH_OK) {
		ssh_channel_close(channel);
		ssh_channel_free(channel);
		return SSH_REQUEST_FAILED;
	}

	int n = 0;
	size_t size = 0;
	
	if (buffer && buffer_size > 0) {
	
		while ((n = ssh_channel_read(channel, buffer + size, buffer_size - size - 1, 0)) > 0) {
			size += n;
			if (size >= buffer_size - 1) {
				size = buffer_size - 1;
				break;
			}
		}
 
		if (n < 0) {
			ssh_channel_send_eof(channel);
			ssh_channel_close(channel);
			ssh_channel_free(channel);
			return SSH_READ_FAILED;
		}
 		
		if (size > 0 && buffer[size - 1] == '\n')
			size--;
		buffer[size] = '\0';
	}

	ssh_channel_send_eof(channel);

	ssh_channel_close(channel);
	ssh_channel_free(channel);

	return SUCCESS;
}

error_code_t ssh_dry_run(ssh_session session) {
	char output[4096];
	return ssh_cmd_exec(session, output, sizeof(output), "ls");
}

error_code_t ssh_kill_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -KILL %d", pid);
	return ssh_cmd_exec(session, nullptr, 0, cmd);
}

error_code_t ssh_term_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -TERM %d", pid);
	return ssh_cmd_exec(session, nullptr, 0, cmd);
}

error_code_t ssh_stop_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -STOP %d", pid);
	return ssh_cmd_exec(session, nullptr, 0, cmd);
}

error_code_t ssh_cont_processus(ssh_session session, int pid) {
	char cmd[64];
	snprintf(cmd, sizeof(cmd), "kill -CONT %d", pid);
	return ssh_cmd_exec(session, nullptr, 0, cmd);
}

error_code_t ssh_restart_processus(ssh_session session, processus_t *p) {
	char cmd[256];

	snprintf(cmd, sizeof(cmd),"PID=%d; CMDLINE=\"$(tr '\\0' ' ' </proc/$PID/cmdline)\"; CWD=\"$(readlink /proc/$PID/cwd)\"; kill -TERM \"$PID\"; (cd \"$CWD\" || exit 1; exec $CMDLINE) &", p->pid);
	return ssh_cmd_exec(session, nullptr, 0, cmd);
}
