// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef VERSIONING_H
#define VERSIONING_H

// Identify a version-prefixed path and return 1 when its version is supported
int versioning_is_supported_path(const char *path);

// Return 1 if a path looks version-prefixed but does not use the current version
int versioning_is_unsupported_version_path(const char *path);

#endif