// Archie McVey (Discord: stathri, Roblox: stathri)

#include <string.h>

#include "config.h"
#include "versioning.h"

// Match /v1 as a complete prefix, not something like /v10
int versioning_is_supported_path(const char *path)
{
    size_t prefix_length = strlen(API_VERSION_PREFIX); // We'll use this length for the prefix check below

    return path != NULL && strncmp(path, API_VERSION_PREFIX, prefix_length) == 0 && (path[prefix_length] == '\0' || path[prefix_length] == '/');
}

// Spot version-style paths such as /v2 or /v3
int versioning_is_unsupported_version_path(const char *path)
{
    if (path == NULL || path[0] != '/' || path[1] != 'v') // Anything without the /v pattern isn't an API version path
        return 0;

    // The character after /v needs to be a digit for this check to apply
    return path[2] >= '0' && path[2] <= '9' && !versioning_is_supported_path(path);
}