// Archie McVey (Discord: stathri, Roblox: stathri)

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "middleware.h"
#include "responses.h"
#include "routes.h"
#include "versioning.h"
#include "validation.h"

// Reject methods that an endpoint does not explicitly support
static int require_method(struct mg_connection *conn, const char *expected)
{
    const struct mg_request_info *request = mg_get_request_info(conn);

    if (request == NULL || request->request_method == NULL ||
        strcmp(request->request_method, expected) != 0)
    {
        response_error(conn, 405, "Method Not Allowed", "METHOD_NOT_ALLOWED",
                       "HTTP method is not supported for this route");
        return 0;
    }
    return 1;
}

// Forward declaration lets the versioned test route dispatch POST requests
static int route_test_post(struct mg_connection *conn, void *callback_data);

// Shared test handler used by both /test and /v1/test
static int route_test(struct mg_connection *conn, void *callback_data)
{
    const struct mg_request_info *request = mg_get_request_info(conn);

    // Dispatch POST /v1/test to its body-validation handler
    if (request != NULL && request->request_method != NULL &&
        strcmp(request->request_method, "POST") == 0 &&
        versioning_is_supported_path(request->local_uri))
    {
        return route_test_post(conn, callback_data);
    }

    (void)callback_data;
    middleware_log_request(conn);

    if (!middleware_rate_limit_allow(conn))
    {
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED",
                              "Too many requests; try again later");
    }
    if (!require_method(conn, "GET"))
        return 1;

    return response_json(conn, 200, "OK", "{\"message\":\"API is working\"}");
}

// Shared health handler used by both /health and /v1/health
static int route_health(struct mg_connection *conn, void *callback_data)
{
    (void)callback_data;
    middleware_log_request(conn);

    if (!middleware_rate_limit_allow(conn))
    {
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED",
                              "Too many requests; try again later");
    }
    if (!require_method(conn, "GET"))
        return 1;

    return response_json(conn, 200, "OK", "{\"status\":\"ok\"}");
}

// Demonstrate a validated POST endpoint without requiring an external JSON library
static int route_test_post(struct mg_connection *conn, void *callback_data)
{
    char body[API_MAX_BODY_BYTES + 1U];

    (void)callback_data;
    middleware_log_request(conn);

    if (!middleware_rate_limit_allow(conn))
    {
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED",
                              "Too many requests; try again later");
    }
    if (!require_method(conn, "POST"))
        return 1;

    // Delegate header, size, and body checks to the dedicated validation module.
    if (!validation_read_json_body(conn, body, sizeof(body)))
        return 1;

    // Echo only the fact of acceptance; never reflect untrusted body bytes into JSON
    return response_json(conn, 200, "OK", "{\"message\":\"Request accepted\"}");
}

// Handle unknown routes and unsupported version prefixes with consistent JSON errors
static int route_fallback(struct mg_connection *conn, void *callback_data)
{
    const struct mg_request_info *request = mg_get_request_info(conn);
    const char *path = request != NULL ? request->local_uri : NULL;
    (void)callback_data;
    middleware_log_request(conn);

    if (middleware_rate_limit_allow(conn) == 0)
    {
        return response_error(conn, 429, "Too Many Requests", "RATE_LIMITED",
                              "Too many requests; try again later");
    }
    if (versioning_is_unsupported_version_path(path))
    {
        return response_error(conn, 400, "Bad Request", "UNSUPPORTED_API_VERSION",
                              "The requested API version is not supported");
    }
    return response_error(conn, 404, "Not Found", "NOT_FOUND", "Route not found");
}

// Register exact routes before the root fallback so known paths take precedence
void routes_register(struct mg_context *ctx)
{
    mg_set_request_handler(ctx, "/test", route_test, NULL);
    mg_set_request_handler(ctx, "/health", route_health, NULL);
    mg_set_request_handler(ctx, API_VERSION_PREFIX "/test", route_test, NULL);
    mg_set_request_handler(ctx, API_VERSION_PREFIX "/health", route_health, NULL);
    mg_set_request_handler(ctx, "/", route_fallback, NULL);
}