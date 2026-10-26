// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef VALIDATION_H
#define VALIDATION_H

#include <stddef.h>
#include "civetweb.h"

// Read a bounded JSON request body. Returns 1 when valid, otherwise sends an error response
int validation_read_json_body(struct mg_connection *conn, char *body, size_t capacity);

#endif