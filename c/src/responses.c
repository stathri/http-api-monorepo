// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stdio.h>
#include <string.h>

#include "responses.h"

// Send a JSON response directly over the CivetWeb connection
int response_json(struct mg_connection *conn, int status_code,
                  const char *status_text, const char *json_body)
{
    size_t body_length = strlen(json_body); // strlen gives us the payload length, not including the terminating NUL

    // Tell the client how long the JSON body is before sending it
    mg_printf(conn,
              "HTTP/1.1 %d %s\r\n"
              "Content-Type: application/json; charset=utf-8\r\n"
              "Cache-Control: no-store\r\n"
              "X-Content-Type-Options: nosniff\r\n"
              "Content-Length: %lu\r\n"
              "Connection: close\r\n\r\n",
              status_code, status_text, (unsigned long)body_length);
    mg_write(conn, json_body, body_length); // Headers are out, so write the actual JSON now

    // Tell CivetWeb this handler has taken care of the request
    return 1;
}

// Keep errors in one shape so clients don't have to handle several formats
int response_error(struct mg_connection *conn, int status_code,
                   const char *status_text, const char *code,
                   const char *message)
{
    char body[512]; // Build the small error object here before sending it

    // Don't let an error message overrun this buffer
    (void)snprintf(body, sizeof(body),
                   "{\"error\":{\"code\":\"%s\",\"message\":\"%s\"}}",
                   code, message);

    return response_json(conn, status_code, status_text, body);
}