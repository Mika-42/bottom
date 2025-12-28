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
	if(!session || !buffer || !cmd) return NULLPTR_PARAMETER_ERROR;

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
				buffer[size - 1] = '\0';
				goto cleanup;
			}
		}
 
		if (n < 0) {
			goto cleanup;
		}
 		
		if (size > 0 && buffer[size - 1] == '\n')
			size--;
		buffer[size] = '\0';
	}

cleanup:

	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);

	return (n < 0 || (buffer && size >= buffer_size - 1)) ? SSH_READ_FAILED : SUCCESS;
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
	char cmd[16384];
	size_t off = 0;

	off += snprintf(cmd + off, sizeof(cmd) - off, "pkill -TERM -P %d; kill -TERM %d; sleep 1; pkill -KILL -P %d; kill -KILL %d; ", p->pid, p->pid, p->pid, p->pid);

	off += snprintf(cmd + off, sizeof(cmd) - off, "env -i ");
	
	for (size_t i=0; i<PROC_CMD_COUNT && p->env[i][0]; ++i) {
		off += snprintf(cmd + off, sizeof(cmd) - off, "%s ", p->env[i]);
	}

	off += snprintf(cmd + off, sizeof(cmd) - off, "%s ", p->executable);

	for (size_t i=0; i<PROC_CMD_COUNT && p->cmdline[i][0]; ++i) {
		off += snprintf(cmd + off, sizeof(cmd) - off, "%s ", p->cmdline[i]);
	}
	
	off += snprintf(cmd + off, sizeof(cmd) - off, "&");

	return ssh_cmd_exec(session, nullptr, 0, cmd);
}

/**
 * @brief Met le contenue du fichier /proc/file dans le buffer
 *
 * @param session Session ssh
 * @param buffer Adresse de la chaine de caractère dans laquel sera écrit le contenue du fichier, à liberer par l'appelant
 * @param file Fichier du dossier /proc/ qui sera lu
 * @return Code d'erreur correspondant de libssh
 */
error_code_t ssh_get_file(ssh_session session, char *buffer, size_t buffer_size, const char *file) {
	for (const char *p = file; *p; ++p) {
    		if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '/' || *p == '_')) {
			return INVALID_ARGUMENT;
		}
	}

	char cmd[128];
	snprintf(cmd, sizeof(cmd), "cat /proc/%s", file);
	
	return ssh_cmd_exec(session, buffer, buffer_size, cmd);
}

error_code_t ssh_get_exe(ssh_session session, char *buffer, size_t buffer_size, processus_t *p) {
	char cmd[128];
	snprintf(cmd, sizeof(cmd), "readlink /proc/%d/exe", p->pid);
	
	return ssh_cmd_exec(session, buffer, buffer_size, cmd);
}

error_code_t ssh_get_pid_list(ssh_session session, pid_t *pid_list, size_t size_list, size_t *count) {
	
	if (!pid_list || !count) return NULLPTR_PARAMETER_ERROR;
	
	char ls_output[8192];

	error_code_t err = ssh_cmd_exec(session, ls_output, sizeof(ls_output), "ls /proc/");
	
	if (err != SUCCESS) {
		return err;
	}

	*count = 0;
	char *ptr = ls_output;
	
	while (*ptr && *count < size_list) {
		
		while (*ptr && isspace((unsigned char)*ptr)) 
			ptr++;

		if (!*ptr) {
			break;
		}
		
		char *start = ptr;

		while (*ptr && !isspace((unsigned char)*ptr))
			ptr++;

		char saved = *ptr;
		*ptr = '\0';

		if(proc_is_valid_pid(start)) {
			long val = strtol(start, nullptr, 10);
			pid_list[*count] = (pid_t)val;
            (*count)++;
		}
		*ptr = saved;
		if (*ptr) ptr++;
	}

	return SUCCESS;
}
