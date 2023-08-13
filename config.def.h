#ifndef __CLIQ_CONFIG_H
#define __CLIQ_CONFIG_H

#include <qtypes.h>

/* Constants */
const int CONFIG_FILE = 0;
const int STATE_FILE = 1;
const int QLIC_FP_READ_SIZE = 256;

const char* const qlic_env_vars[] = {
	"QLIC_CONFIG_FILE",
	"QLIC_STATE_FILE",
};
const char* qlic_env_default_vars[] = {
	".config/qlic/config.json",
	".cache/qlic_state",
};
const char* const qlic_file_types[] = {
	"config",
	"state",
};
const char* const qlic_file_flags[] = {
	"r",
	"rw",
};

#define CLIQ_AUTH_ENDPOINT "https://accounts.zoho.com/oauth/v2/auth"
#define CLIQ_TOKEN_ENDPOINT "https://accounts.zoho.com/oauth/v2/token"

/* TODO Make redirect URI non sensical, so it doesn't actually redirect anywhere */
/* Or delegate this url to the user */
/* Search for any no-op ports */
#define CLIQ_REDIRECT_URI "https://127.0.0.1:8443/hello"
#define CLIQ_SCOPE "ZohoCliq.Chats.READ,ZohoCliq.Messages.READ,ZohoCliq.Webhooks.CREATE"

#endif
