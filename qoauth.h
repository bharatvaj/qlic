#ifndef __QLIC_OAUTH_H
#define __QLIC_OAUTH_H

#include <oauth2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <nxjson.h>

#include <qtypes.h>

// TODO Choose between DB and text files for saving this information
// If using text, choose between formats, yaml or json or other format, which is more suckless
// If DB, sliqte3 is a good choice, but don't
// Going to need a json parser and writer, might as well make the config files as json as well
/* const char* saved_grant_token = NULL; */
/* const char* saved_access_token = NULL; */
/* const char* saved_grant_token = ""; */

// TODO rethink this decision later, char* or const char* ?
// Have to free the returning string at the end, who does that?
char *json_access_code_transformer(char* str);

/* qstr write_to_json() { */
/* 	qstr jsonstate = NULL; */
/* 	qstrsprintf(jsonstate, "{ " */
/* 			"}", ); */
/* 	return jsonstate; */
/* } */

char* start_oauth_server(QlicConfig* config);
#endif
