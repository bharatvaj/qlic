#ifndef __QLIC_OAUTH_H
#define __QLIC_OAUTH_H

#include <oauth2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <nxjson.h>

#include <qtypes.h>

char *json_access_code_transformer(char* str);

char* start_oauth_server(qlicconfig_t* config);
#endif
