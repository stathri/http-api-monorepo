// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef CONFIG_H
#define CONFIG_H

#define API_HOST "127.0.0.1"              // Keep this bound to localhost for now
#define API_PORT "6767"                   // The port the local API runs on
#define API_THREAD_COUNT "4"              // How many requests CivetWeb can handle in parallel
#define API_VERSION "v1"                  // Bump this when a new API version is introduced
#define API_VERSION_PREFIX "/v1"          // Routes for this version sit underneath this path
#define API_RATE_LIMIT_MAX_REQUESTS 60U   // Requests a single client can make before being throttled
#define API_RATE_LIMIT_WINDOW_SECONDS 60U // How long it takes for that client's counter to reset
#define API_MAX_BODY_BYTES 8192U          // Keep incoming JSON bodies under this size

const char **config_server_options(void);

#endif