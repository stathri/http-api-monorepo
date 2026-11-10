// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef RESPONSES_H
#define RESPONSES_H

#include "civetweb.h" // Needed for the CivetWeb connection pointer in these declarations

// Send a JSON body with the status details supplied by the route
int response_json(struct mg_connection *conn, int status_code,
                  const char *status_text, const char *json_body);

// Use the same error object shape across the API
int response_error(struct mg_connection *conn, int status_code,
                   const char *status_text, const char *code,
                   const char *message);

#endif