// Archie McVey (Discord: stathri, Roblox: stathri)

#include <string.h>

#include "config.h"
#include "versioning.h"

// Accept only the current version prefix followed by a path boundary
int versioning_is_supported_path(const char *path)
{
    size_t prefix_length = strlen(API_VERSION_PREFIX);

    return path != NULL && strncmp(path, API_VERSION_PREFIX, prefix_length) == 0 &&
           (path[prefix_length] == '\0' || path[prefix_length] == '/');
}

// Treat paths beginning with /v plus a digit as versioned API paths
int versioning_is_unsupported_version_path(const char *path)
{
    if (path == NULL || path[0] != '/' || path[1] != 'v')
    {
        return 0;
    }

    // A digit immediately after 'v' distinguishes a version prefix such as /v2
    return path[2] >= '0' && path[2] <= '9' && !versioning_is_supported_path(path);
}