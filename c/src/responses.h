// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef RESPONSES_H
#define RESPONSES_H

#include "civetweb.h"

// Send a JSON response; the caller supplies a trusted status phrase and JSON body
int response_json(struct mg_connection *conn, int status_code,
                  const char *status_text, const char *json_body);

// Send a consistent JSON error envelope for all API errors
int response_error(struct mg_connection *conn, int status_code,
                   const char *status_text, const char *code,
                   const char *message);

#endif