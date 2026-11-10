// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef ROUTES_H
#define ROUTES_H

#include "civetweb.h" // CivetWeb's server context is used by routes_register

// Set up the API routes, including the catch-all handler
void routes_register(struct mg_context *ctx);

#endif