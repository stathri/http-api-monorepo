// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef VERSIONING_H
#define VERSIONING_H

// Check whether a path uses the API version configured in config.h
int versioning_is_supported_path(const char *path);

// Flag version-looking paths that aren't using the version we currently support
int versioning_is_unsupported_version_path(const char *path);

#endif