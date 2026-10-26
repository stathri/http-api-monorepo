// Archie McVey (Discord: stathri, Roblox: stathri)

#ifndef CONFIG_H
#define CONFIG_H

#define API_HOST "127.0.0.1"
#define API_PORT "6767"
#define API_THREAD_COUNT "4"
#define API_VERSION "v1"
#define API_VERSION_PREFIX "/v1"
#define API_RATE_LIMIT_MAX_REQUESTS 60U
#define API_RATE_LIMIT_WINDOW_SECONDS 60U
#define API_MAX_BODY_BYTES 8192U

const char **config_server_options(void);

#endif