// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "civetweb.h"

// Log the method, URI, and client address for each request
void middleware_log_request(const struct mg_connection *conn);

// Return nonzero when the client's request should be allowed by the local rate limiter
int middleware_rate_limit_allow(const struct mg_connection *conn);

#endif