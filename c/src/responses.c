// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stdio.h>
#include <string.h>

#include "responses.h"

// Write HTTP headers and the JSON payload through CivetWeb's connection API
int response_json(struct mg_connection *conn, int status_code,
                  const char *status_text, const char *json_body)
{
    size_t body_length = strlen(json_body);

    // Include Content-Length so clients know exactly how many body bytes to read
    mg_printf(conn,
              "HTTP/1.1 %d %s\r\n"
              "Content-Type: application/json; charset=utf-8\r\n"
              "Cache-Control: no-store\r\n"
              "X-Content-Type-Options: nosniff\r\n"
              "Content-Length: %lu\r\n"
              "Connection: close\r\n\r\n",
              status_code, status_text, (unsigned long)body_length);
    mg_write(conn, json_body, body_length);

    // CivetWeb treats any nonzero handler result as "request handled"
    return 1;
}

// Format errors consistently; code and message are fixed internal strings, not user input
int response_error(struct mg_connection *conn, int status_code,
                   const char *status_text, const char *code,
                   const char *message)
{
    char body[512];

    // Keep the error payload small and bounded for predictable responses
    (void)snprintf(body, sizeof(body),
                   "{\"error\":{\"code\":\"%s\",\"message\":\"%s\"}}",
                   code, message);
    return response_json(conn, status_code, status_text, body);
}