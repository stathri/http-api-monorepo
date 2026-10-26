// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef ROUTES_H
#define ROUTES_H

#include "civetweb.h"

// Register the application's endpoints and the final fallback handler
void routes_register(struct mg_context *ctx);

#endif