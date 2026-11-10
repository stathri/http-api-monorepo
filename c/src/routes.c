// Archie McVey (Discord: stathri, Roblox: stathri)

#include <string.h>

#include "config.h"
#include "middleware.h"
#include "responses.h"
#include "routes.h"
#include "versioning.h"
#include "validation.h"

// Keep each route strict about which HTTP methods it accepts
static int require_method(struct mg_connection *conn, const char *expected)
{
    const struct mg_request_info *request = mg_get_request_info(conn); // Grab the request details before checking its method

    if (request == NULL || request->request_method == NULL || strcmp(request->request_method, expected) != 0)
    {
        response_error(conn, 405, "Method Not Allowed", "METHOD_NOT_ALLOWED", "HTTP method is not supported for this route"); // Sends 405 JSON error to the browser
        return 0;
    }

    return 1;
}

// Defined below, but route_test needs to call it first
static int route_test_post(struct mg_connection *conn, void *callback_data);

// Both test URLs end up in this handler
static int route_test(struct mg_connection *conn, void *callback_data)
{
    const struct mg_request_info *request = mg_get_request_info(conn); // Grab the request details before checking its method

    // POST on the versioned route has a body to validate, so send it to the POST handler
    if (request != NULL && request->request_method != NULL && strcmp(request->request_method, "POST") == 0 && versioning_is_supported_path(request->local_uri))
        return route_test_post(conn, callback_data);

    (void)callback_data; // Nothing was passed in callback_data that we need here

    middleware_log_request(conn); // Log it before we start rejecting anything

    if (!middleware_rate_limit_allow(conn))                                                                          // Don't run the route if this IP has hit the limit
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED", "Too many requests; try again later"); // Sends 429 JSON error to the browser

    if (!require_method(conn, "GET")) // This one is a GET route, anything else gets a 405
        return 1;

    return response_json(conn, 200, "OK", "{\"message\":\"API is working\"}"); // A simple response is enough to confirm the route is alive
}

// The versioned and unversioned health checks share this handler
static int route_health(struct mg_connection *conn, void *callback_data)
{
    (void)callback_data;          // Nothing was passed in callback_data that we need here
    middleware_log_request(conn); // Log it before we start rejecting anything

    if (!middleware_rate_limit_allow(conn))                                                                          // Don't run the route if this IP has hit the limit
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED", "Too many requests; try again later"); // Sends 429 JSON error to the browser

    if (!require_method(conn, "GET")) // This one is a GET route, anything else gets a 405
        return 1;

    return response_json(conn, 200, "OK", "{\"status\":\"ok\"}"); // Keep the health response deliberately small
}

// Test the body checks without pulling in a separate JSON library
static int route_test_post(struct mg_connection *conn, void *callback_data)
{
    char body[API_MAX_BODY_BYTES + 1U]; // One extra byte is reserved for the string terminator

    (void)callback_data;          // Nothing was passed in callback_data that we need here
    middleware_log_request(conn); // Log it before we start rejecting anything

    if (!middleware_rate_limit_allow(conn))                                                                          // Don't run the route if this IP has hit the limit
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED", "Too many requests; try again later"); // Sends 429 JSON error to the browser

    if (!require_method(conn, "POST")) // Check the method first; there's no point reading a body for the wrong request
        return 1;

    // Let validation handle the content type, size and actual read
    if (!validation_read_json_body(conn, body, sizeof(body)))
        return 1;

    // We only confirm the request was accepted; the supplied body isn't echoed back
    return response_json(conn, 200, "OK", "{\"message\":\"Request accepted\"}"); // Sends 200 JSON response to the browser
}

// Anything that didn't match a route ends up here
static int route_fallback(struct mg_connection *conn, void *callback_data)
{
    const struct mg_request_info *request = mg_get_request_info(conn); // Grab the request details before checking its method
    const char *path = request != NULL ? request->local_uri : NULL;    // The fallback can still run even if CivetWeb gives us no request details

    (void)callback_data; // Nothing was passed in callback_data that we need here

    middleware_log_request(conn); // Log it before we start rejecting anything

    if (middleware_rate_limit_allow(conn) == 0)                                                                      // Unknown paths still count towards the same rate limit
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED", "Too many requests; try again later"); // Sends 429 JSON error to the browser

    if (versioning_is_unsupported_version_path(path))                                                                             // Give unsupported API versions their own error instead of pretending the route doesn't exist
        return response_error(conn, 400, "Bad Request", "UNSUPPORTED_API_VERSION", "The requested API version is not supported"); // Sends 400 JSON error to the browser

    return response_error(conn, 404, "Not Found", "NOT_FOUND", "Route not found"); // Sends 404 JSON error to the browser
}

// Register the real endpoints first, then let / catch everything else
void routes_register(struct mg_context *ctx)
{
    mg_set_request_handler(ctx, "/test", route_test, NULL);                        // Keep the shorter test URL working too
    mg_set_request_handler(ctx, "/health", route_health, NULL);                    // Health check without the version prefix
    mg_set_request_handler(ctx, API_VERSION_PREFIX "/test", route_test, NULL);     // And the versioned URL points to the same handler
    mg_set_request_handler(ctx, API_VERSION_PREFIX "/health", route_health, NULL); // Same health check, just under /v1
    mg_set_request_handler(ctx, "/", route_fallback, NULL);                        // Anything else falls through to the not-found handler
}