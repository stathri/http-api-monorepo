// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "civetweb.h"
#include "config.h"
#include "routes.h"

// Initialise CivetWeb, register routes, and stop cleanly when the Enter key is pressed in the terminal
int main(void)
{
    struct mg_callbacks callbacks;
    struct mg_context *ctx;

    // Clear optional callbacks because this application does not override them
    memset(&callbacks, 0, sizeof(callbacks));

    // Start the HTTP server using the central configuration module
    ctx = mg_start(&callbacks, NULL, config_server_options());

    if (ctx == NULL)
    {
        fprintf(stderr, "Failed to start HTTP server at %s:%s.\n", API_HOST, API_PORT);
        return EXIT_FAILURE;
    }

    // Install all application routes after the server has started successfully
    routes_register(ctx);

    printf("C API listening at http://%s:%s\n", API_HOST, API_PORT);
    printf("Press Enter to stop the server.\n");

    (void)getchar();

    // Release server resources before the process exits
    mg_stop(ctx);

    return EXIT_SUCCESS;
}