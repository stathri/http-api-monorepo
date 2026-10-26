// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stddef.h>
#include <string.h>

#include "config.h"
#include "responses.h"
#include "validation.h"

// Check the media type without accepting values such as "application/jsonp"
static int is_json_content_type(const char *content_type)
{
    static const char expected[] = "application/json";
    size_t i;

    if (content_type == NULL)
        return 0;

    // Compare the required media type case-insensitively for ASCII letters
    for (i = 0U; i < sizeof(expected) - 1U; ++i)
    {
        char actual = content_type[i];
        char wanted = expected[i];

        if (actual >= 'A' && actual <= 'Z')
            actual = (char)(actual - 'A' + 'a');
        if (wanted >= 'A' && wanted <= 'Z')
            wanted = (char)(wanted - 'A' + 'a');

        if (actual != wanted)
            return 0;
    }

    // A media type may be followed by parameters such as "; charset=utf-8"
    return content_type[sizeof(expected) - 1U] == '\0' ||
           content_type[sizeof(expected) - 1U] == ';' ||
           content_type[sizeof(expected) - 1U] == ' ';
}

// Read only the declared, bounded request body into caller-owned storage
static int read_request_body(struct mg_connection *conn, long content_length,
                             char *body, size_t capacity)
{
    long remaining = content_length;
    size_t received = 0U;

    // Reserve one byte for the terminating NUL character
    if (capacity == 0U || content_length < 0 ||
        (unsigned long)content_length >= (unsigned long)capacity)
        return 0;

    // CivetWeb may return fewer bytes than requested, so keep reading until complete
    while (remaining > 0 && received < capacity - 1U)
    {
        int count = mg_read(conn, body + received, (size_t)remaining);

        if (count <= 0)
            return 0;

        received += (size_t)count;
        remaining -= count;
    }

    // Do not treat a partial read as a complete request body
    if (remaining != 0 || received == 0U)
        return 0;

    body[received] = '\0';
    return 1;
}

// Validate headers and body size before allowing the route to process the payload
int validation_read_json_body(struct mg_connection *conn, char *body, size_t capacity)
{
    const struct mg_request_info *request = mg_get_request_info(conn);
    const char *content_type = mg_get_header(conn, "Content-Type");

    if (request == NULL)
    {
        response_error(conn, 400, "Bad Request", "VALIDATION_ERROR",
                       "Request metadata is unavailable");
        return 0;
    }

    // Require a declared non-empty body to avoid ambiguous empty submissions
    if (request->content_length <= 0)
    {
        response_error(conn, 400, "Bad Request", "VALIDATION_ERROR",
                       "A non-empty JSON request body is required");
        return 0;
    }

    // Enforce the configured limit before reading into the fixed buffer
    if (request->content_length > (long)API_MAX_BODY_BYTES ||
        (unsigned long)request->content_length >= (unsigned long)capacity)
    {
        response_error(conn, 413, "Content Too Large", "BODY_TOO_LARGE",
                       "Request body exceeds the allowed size");
        return 0;
    }

    // Check media type before consuming the body
    if (!is_json_content_type(content_type))
    {
        response_error(conn, 415, "Unsupported Media Type", "UNSUPPORTED_MEDIA_TYPE",
                       "Content-Type must be application/json");
        return 0;
    }

    // Reject truncated or unreadable bodies with a stable validation error
    if (!read_request_body(conn, request->content_length, body, capacity))
    {
        response_error(conn, 400, "Bad Request", "VALIDATION_ERROR",
                       "Request body could not be read completely");
        return 0;
    }

    return 1;
}