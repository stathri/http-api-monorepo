// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stddef.h>

#include "config.h"
#include "responses.h"
#include "validation.h"

// Only accept application/json, not lookalikes such as application/jsonp
static int is_json_content_type(const char *content_type)
{
    static const char expected[] = "application/json"; // The exact content type we're looking for
    size_t i;                                          // Current position in the comparison

    if (content_type == NULL)
        return 0;

    // HTTP header values are case-insensitive, so handle uppercase letters too
    for (i = 0U; i < sizeof(expected) - 1U; ++i)
    {
        char actual = content_type[i]; // What the client sent at this position
        char wanted = expected[i];     // What we expect to see here

        if (actual >= 'A' && actual <= 'Z')
            actual = (char)(actual - 'A' + 'a');

        if (wanted >= 'A' && wanted <= 'Z')
            wanted = (char)(wanted - 'A' + 'a');

        if (actual != wanted)
            return 0;
    }

    // Allow normal parameters after the media type, for example a charset
    return content_type[sizeof(expected) - 1U] == '\0' ||
           content_type[sizeof(expected) - 1U] == ';' ||
           content_type[sizeof(expected) - 1U] == ' ';
}

// Read the body into the buffer provided by the caller, without going past its limit
static int read_request_body(struct mg_connection *conn, long content_length, char *body, size_t capacity)
{
    long remaining = content_length; // How much of the declared body is left
    size_t received = 0U;            // Bytes we've successfully read so far

    // The buffer needs a spare byte for '\0'
    if (capacity == 0U || content_length < 0 || (unsigned long)content_length >= (unsigned long)capacity)
        return 0;

    // mg_read can return a partial chunk, so don't assume one call gets everything
    while (remaining > 0 && received < capacity - 1U)
    {
        int count = mg_read(conn, body + received, (size_t)remaining); // Append this chunk where the previous read finished

        if (count <= 0)
            return 0;

        received += (size_t)count; // Advance to the next free spot in the buffer
        remaining -= count;        // Keep the remaining count in sync with the read
    }

    // A short body is still a failed read
    if (remaining != 0 || received == 0U)
        return 0;

    body[received] = '\0'; // Terminate the buffer so the rest of the code can treat it as a string
    return 1;
}

// Check the request before letting the route use its body
int validation_read_json_body(struct mg_connection *conn, char *body, size_t capacity)
{
    const struct mg_request_info *request = mg_get_request_info(conn); // CivetWeb tells us how many bytes the client says it sent
    const char *content_type = mg_get_header(conn, "Content-Type");    // We need this before deciding whether to read the body

    if (request == NULL)
    {
        response_error(conn, 400, "Bad Request", "VALIDATION_ERROR", "Request metadata is unavailable");
        return 0;
    }

    // An empty POST isn't useful for this test endpoint
    if (request->content_length <= 0)
    {
        response_error(conn, 400, "Bad Request", "VALIDATION_ERROR", "A non-empty JSON request body is required");
        return 0;
    }

    // Reject oversized bodies before they get anywhere near the buffer
    if (request->content_length > (long)API_MAX_BODY_BYTES || (unsigned long)request->content_length >= (unsigned long)capacity)
    {
        response_error(conn, 413, "Content Too Large", "BODY_TOO_LARGE", "Request body exceeds the allowed size");
        return 0;
    }

    // Don't bother reading it if the client sent the wrong content type
    if (!is_json_content_type(content_type))
    {
        response_error(conn, 415, "Unsupported Media Type", "UNSUPPORTED_MEDIA_TYPE", "Content-Type must be application/json");
        return 0;
    }

    // If the full body can't be read, return the same validation error
    if (!read_request_body(conn, request->content_length, body, capacity))
    {
        response_error(conn, 400, "Bad Request", "VALIDATION_ERROR", "Request body could not be read completely");
        return 0;
    }

    return 1;
}