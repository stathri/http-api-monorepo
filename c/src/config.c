// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stddef.h>
#include "config.h"

const char **config_server_options(void)
{
    static const char *options[] = {
        "listening_ports", API_HOST ":" API_PORT,
        "num_threads", API_THREAD_COUNT,
        "enable_keep_alive", "yes",
        NULL};

    return options;
}