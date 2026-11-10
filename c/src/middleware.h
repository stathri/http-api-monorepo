// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "civetweb.h" // CivetWeb defines the connection type used below

// Write a short line to stdout for each incoming request
void middleware_log_request(const struct mg_connection *conn);

// Returns 1 while the client is within its request allowance
int middleware_rate_limit_allow(const struct mg_connection *conn);

#endif