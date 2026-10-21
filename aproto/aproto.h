#ifndef APROTO_HEADER
#define APROTO_HEADER

#include <stddef.h>
#include <stdint.h>

#define APROTO_VERSION_MAJOR 1
#define APROTO_VERSION_MINOR 0

/** Initial receive CRC checking state; override to 0 at compile time if needed. */
#ifndef APROTO_CRC_DEFAULT
#define APROTO_CRC_DEFAULT 1
#endif

/** Initial successful-action acknowledgment state; override to 0 if needed. */
#ifndef APROTO_ACK_DEFAULT
#define APROTO_ACK_DEFAULT 1
#endif

/** Result of protocol initialization, parsing, or frame generation. */
typedef enum {
    APROTO_STATUS_OK,            /**< Operation completed successfully. */
    APROTO_STATUS_COMMAND_ERROR, /**< Command was unknown or rejected. */
    APROTO_STATUS_FRAME_ERROR,   /**< Frame syntax or checksum was invalid. */
    APROTO_STATUS_OVERFLOW,      /**< The supplied output buffer was too small. */
    APROTO_STATUS_CONFIG_ERROR   /**< Protocol instance or registry is invalid. */
} aproto_status_t;

/** Result returned by an application command handler. */
typedef enum {
    /** Successful action; emits an automatic `OK,<command>` acknowledgement. */
    APROTO_COMMAND_OK,
    /** Successful data response; sends the handler body unchanged. */
    APROTO_COMMAND_DATA,
    /** Invalid or failed command; emits `ERR,<command>`. */
    APROTO_COMMAND_ERROR
} aproto_command_result_t;

/** Read-only, length-delimited string view; data is not necessarily terminated. */
typedef struct {
    const char *data; /**< First character in the view. */
    size_t length;    /**< Number of characters in the view. */
} aproto_string_t;

/** Parsed request passed to an application handler. */
typedef struct {
    aproto_string_t command;   /**< Exact command name without separators. */
    aproto_string_t arguments; /**< Comma-separated argument span, if present. */
    int query;                 /**< Non-zero when the command used `?`. */
} aproto_request_t;

/** Capacity-aware response body writer supplied to a command handler. */
typedef struct {
    char *data;      /**< Caller-owned body buffer. */
    size_t capacity; /**< Maximum body length. */
    size_t length;   /**< Current body length. */
    int overflow;    /**< Non-zero after an append exceeds capacity. */
} aproto_response_t;

/**
 * Application command callback.
 *
 * Append only the response body or acknowledgement suffix to @p response;
 * aproto adds framing and checksums. @p context is the registry entry's
 * application-defined pointer.
 */
typedef aproto_command_result_t (*aproto_command_handler_t)(
    const aproto_request_t *request,
    aproto_response_t *response,
    void *context);

/** Static application command registry entry. */
typedef struct {
    const char *name;                 /**< Exact, null-terminated command name. */
    aproto_command_handler_t handler; /**< Function invoked for this command. */
    void *context;                    /**< Application data passed to handler. */
} aproto_command_t;

/** Protocol instance state. Initialize with aproto_init() before parsing. */
typedef struct {
    const aproto_command_t *commands; /**< Registered application commands. */
    size_t command_count;             /**< Number of registry entries. */
    int crc_enabled;                  /**< Receive checksum checking state. */
    int ack_enabled;                  /**< Successful action ACK state. */
    int configured;                   /**< Internal initialization marker. */
} aproto_t;

/**
 * Initialize a protocol instance and validate its application registry.
 *
 * CRC and acknowledgements start in APROTO_CRC_DEFAULT and
 * APROTO_ACK_DEFAULT states. Command names must be unique and must not collide
 * with built-in commands.
 *
 * @param proto Instance to initialize.
 * @param commands Static registry, or NULL when @p command_count is zero.
 * @param command_count Number of registry entries.
 * @return APROTO_STATUS_OK or APROTO_STATUS_CONFIG_ERROR.
 */
aproto_status_t aproto_init(aproto_t *proto,
                            const aproto_command_t *commands,
                            size_t command_count);

/**
 * Validate and dispatch one complete input frame.
 *
 * Parsing is bounded by @p input_length. A null, CR, or LF byte terminates the
 * frame. The generated response is returned in @p output; use
 * @p output_length for transmission because null termination is not required.
 * A successful action can produce zero output bytes when ACK is disabled.
 *
 * @return Protocol status describing parsing and command execution.
 */
aproto_status_t aproto_parse(aproto_t *proto,
                             const uint8_t *input,
                             size_t input_length,
                             char *output,
                             size_t output_capacity,
                             size_t *output_length);

/**
 * Build `$<body>*HH\r\n`, where HH is the XOR checksum of @p body.
 *
 * @param output Destination frame buffer.
 * @param output_capacity Destination capacity in bytes.
 * @param body Unframed response or command body.
 * @param body_length Number of body bytes.
 * @param output_length Receives the generated frame length; may be NULL.
 */
aproto_status_t aproto_build_frame(char *output,
                                   size_t output_capacity,
                                   const char *body,
                                   size_t body_length,
                                   size_t *output_length);

/** Append a null-terminated string to a handler response body. */
int aproto_response_append(aproto_response_t *response, const char *text);
/** Append exactly @p length characters to a handler response body. */
int aproto_response_append_n(aproto_response_t *response,
                             const char *text,
                             size_t length);
/** Append a base-10 integer to a handler response body. */
int aproto_response_append_int(aproto_response_t *response, int value);
/** Append a floating-point value with 0..9 decimal places. */
int aproto_response_append_float(aproto_response_t *response,
                                 float value,
                                 unsigned int precision);

/** Return the number of comma-separated arguments in @p request. */
size_t aproto_argument_count(const aproto_request_t *request);
/** Return argument @p index as a non-owning string view. */
int aproto_argument_string(const aproto_request_t *request,
                           size_t index,
                           aproto_string_t *argument);
/** Parse argument @p index as an exact base-10 integer. */
int aproto_argument_int(const aproto_request_t *request,
                        size_t index,
                        int *value);
/** Parse argument @p index as an exact floating-point value. */
int aproto_argument_float(const aproto_request_t *request,
                          size_t index,
                          float *value);

#endif