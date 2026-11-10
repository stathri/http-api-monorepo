// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stddef.h>
#include "config.h"

const char **config_server_options(void)
{
    static const char *options[] = {                                          // CivetWeb wants these as alternating option names and values
                                    "listening_ports", API_HOST ":" API_PORT, // This is the address the API will listen on
                                    "num_threads", API_THREAD_COUNT,          // Keep the worker count in config rather than hardcoding it here
                                    "enable_keep_alive", "yes",               // No need to make clients open a fresh connection for every request
                                    NULL};                                    // The NULL is important here, CivetWeb uses it to spot the end of the list

    return options; // Hand the options back to main so it can start the server
}