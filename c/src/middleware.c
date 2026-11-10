// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "middleware.h"

#define RATE_LIMIT_CLIENTS 256U // Don't let the per-client table grow without a limit

// A small in-memory counter for each client IP; fine for this local setup, but it won't be shared between multiple server processes
struct rate_limit_entry
{
    char client_ip[48];         // IP address this slot currently belongs to
    time_t window_started;      // When this client's current minute started
    unsigned int request_count; // How many requests they've made in this window
    int occupied;               // Zero means this slot hasn't been claimed yet
};

static struct rate_limit_entry entries[RATE_LIMIT_CLIENTS]; // All clients' counters live in this table

// Log the basics of each request so it's easier to see what's hitting the API
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
    fflush(stdout); // Flush it now rather than waiting for stdout to catch up
}

// Keep each IP to the configured number of requests per time window
int middleware_rate_limit_allow(const struct mg_connection *conn)
{
    const struct mg_request_info *request = mg_get_request_info(conn);
    const time_t now = time(NULL);             // Use the same time value throughout this check
    struct rate_limit_entry *available = NULL; // Keep track of the first slot that looks reusable
    unsigned int i;                            // Walk through the entries one by one

    if (request == NULL)
        return 1; // Without request details there's no IP to check, so let it through

    // Look for this IP first, but keep an empty or expired slot handy as we go
    for (i = 0; i < RATE_LIMIT_CLIENTS; ++i)
    {
        struct rate_limit_entry *entry = &entries[i]; // Short name for the entry we're looking at

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

    // No free slot left, so recycle the first one rather than growing the table
    if (available == NULL)
    {
        available = &entries[0];
    }

    // Reset the counter if this is a new IP or its minute has run out
    if (!available->occupied || strcmp(available->client_ip, request->remote_addr) != 0 ||
        now - available->window_started >= (time_t)API_RATE_LIMIT_WINDOW_SECONDS)
    {
        (void)snprintf(available->client_ip, sizeof(available->client_ip), "%s", request->remote_addr); // snprintf keeps the address inside the fixed-size field

        available->window_started = now; // Start their next window from this request
        available->request_count = 0U;   // They get a fresh allowance
        available->occupied = 1;         // This entry now belongs to that IP
    }

    // This request counts too, so increment before checking the limit
    ++available->request_count; // Don't forget to count the request that brought us here

    return available->request_count <= API_RATE_LIMIT_MAX_REQUESTS;
}