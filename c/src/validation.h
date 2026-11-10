// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef VALIDATION_H
#define VALIDATION_H

#include "civetweb.h" // Needed for the CivetWeb connection passed into the validator

// Read and check a JSON body; returns 1 on success and sends an error on failure
int validation_read_json_body(struct mg_connection *conn, char *body, size_t capacity);

#endif