// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "middleware.h"

#define RATE_LIMIT_CLIENTS 256U

// Store a small fixed set of per-IP counters; suitable for this local demo, not distributed production use
struct rate_limit_entry
{
    char client_ip[48];
    time_t window_started;
    unsigned int request_count;
    int occupied;
};

static struct rate_limit_entry entries[RATE_LIMIT_CLIENTS];

// Print request metadata to stdout to make local testing and demos observable
void middleware_log_request(const struct mg_connection *conn)
{
    const struct mg_request_info *request = mg_get_request_info(conn);

    if (request == NULL)
    {
        fprintf(stderr, "[request] Request metadata unavailable\n");
        return;
    }

    printf("[request] %s %s (client=%s)\n",
           request->request_method != NULL ? request->request_method : "UNKNOWN",
           request->local_uri != NULL ? request->local_uri : "/",
           request->remote_addr);
    fflush(stdout);
}

// Apply a simple fixed-window limit for each remote IP address
int middleware_rate_limit_allow(const struct mg_connection *conn)
{
    const struct mg_request_info *request = mg_get_request_info(conn);
    const time_t now = time(NULL);
    struct rate_limit_entry *available = NULL;
    unsigned int i;

    if (request == NULL)
        return 1; // Do not reject a request solely because metadata is unavailable

    // Find the existing client's entry, or remember the first unused/expired slot
    for (i = 0; i < RATE_LIMIT_CLIENTS; ++i)
    {
        struct rate_limit_entry *entry = &entries[i];

        if (entry->occupied && strcmp(entry->client_ip, request->remote_addr) == 0)
        {
            available = entry;
            break;
        }

        if (!entry->occupied || now - entry->window_started >= (time_t)API_RATE_LIMIT_WINDOW_SECONDS)
        {
            if (available == NULL)
            {
                available = entry;
            }
        }
    }

    // If the table is full, reuse slot zero; this is intentionally bounded for a demo
    if (available == NULL)
    {
        available = &entries[0];
    }

    // Start a fresh time window for new clients or expired windows
    if (!available->occupied || strcmp(available->client_ip, request->remote_addr) != 0 ||
        now - available->window_started >= (time_t)API_RATE_LIMIT_WINDOW_SECONDS)
    {
        (void)snprintf(available->client_ip, sizeof(available->client_ip), "%s", request->remote_addr);

        available->window_started = now;
        available->request_count = 0U;
        available->occupied = 1;
    }

    // Count this request and reject requests after the configured threshold
    ++available->request_count;

    return available->request_count <= API_RATE_LIMIT_MAX_REQUESTS;
}