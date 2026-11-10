// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stdio.h>
#include <stdlib.h>

#include "civetweb.h"
#include "config.h"
#include "routes.h"

// Initialise CivetWeb, register routes, and stop cleanly when the Enter key is pressed in the terminal
int main(void)
{
    struct mg_callbacks callbacks; // CivetWeb's optional hooks; none of them need custom behaviour yet
    struct mg_context *ctx;        // We'll need this handle again when shutting the server down

    // Start with a clean callback struct since we're not overriding any CivetWeb hooks
    memset(&callbacks, 0, sizeof(callbacks));

    // Let the config module supply the server options
    ctx = mg_start(&callbacks, NULL, config_server_options());

    if (ctx == NULL)
    {
        fprintf(stderr, "Failed to start HTTP server at %s:%s.\n", API_HOST, API_PORT);
        return EXIT_FAILURE;
    }

    // Only register routes once CivetWeb has actually started
    routes_register(ctx);

    printf("C API listening at http://%s:%s\n", API_HOST, API_PORT);
    printf("Press Enter to stop the server.\n"); // The server stays up until someone asks to stop it

    (void)getchar(); // Pause here so the server doesn't immediately exit

    // Shut CivetWeb down cleanly before leaving main
    mg_stop(ctx);

    return EXIT_SUCCESS;
}