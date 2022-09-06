#ifndef __CLIQ_API_H
#define __CLIQ_API_H

#define CLIQ_AUTH_ENDPOINT "https://accounts.zoho.com/oauth/v2/auth"
#define CLIQ_TOKEN_ENDPOINT "https://accounts.zoho.com/oauth/v2/token"
/* TODO Make redirect URI non sensical, so it doesn't actually redirect anywhere */
/* Or delegate this url to the user, search for any no-op ports */
#define CLIQ_REDIRECT_URI "https://127.0.0.1:8443/hello"
#define CLIQ_SCOPE "ZohoCliq.Chats.READ,ZohoCliq.Messages.READ,ZohoCliq.Webhooks.CREATE"

#include <qstr.h>

qstr qlic_send_message_str(qstr chat_id);

#endif
