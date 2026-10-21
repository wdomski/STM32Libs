# aproto

`aproto` is a hardware-agnostic ASCII command protocol for frames in the form
`$COMMAND,args*HH\r\n`, where `HH` is the XOR of the bytes between `$` and `*`.
Input is bounded by an explicit length; `\0`, `\r`, and `\n` terminate a frame.

The library performs no allocation and has no hardware dependency. Add
`src/aproto/aproto.c` and `src/eprintfs/eprintfs.c` to the target, and add `inc/` to its
include path.

## Register commands

Define a static command table. The context pointer can hold device or module
state and is passed unchanged to the handler.

```c
#include "aproto/aproto.h"

typedef struct { int gain; } app_t;

static aproto_command_result_t set_gain(const aproto_request_t *request,
										aproto_response_t *response,
										void *context)
{
	app_t *app = context;
	int gain;

	if (request->query || aproto_argument_count(request) != 1u ||
		aproto_argument_int(request, 0u, &gain) != 0) {
		return APROTO_COMMAND_ERROR;
	}

	app->gain = gain;
	aproto_response_append(response, ",");
	aproto_response_append_int(response, gain);
	return APROTO_COMMAND_OK;
}

static app_t app;
static const aproto_command_t commands[] = {
	{ "GAIN", set_gain, &app },
};
```

`APROTO_COMMAND_OK` automatically produces `OK,<command>` followed by the
handler's suffix. The example therefore replies with `OK,GAIN,<value>`.
Return `APROTO_COMMAND_DATA` when the handler writes a complete response body
that must always be sent, or `APROTO_COMMAND_ERROR` to produce `ERR,<command>`.

## Initialize and parse

```c
static aproto_t protocol;

void protocol_init(void)
{
	aproto_init(&protocol, commands, sizeof(commands) / sizeof(commands[0]));
}

void process_rx(const uint8_t *rx, size_t rx_length)
{
	char tx[128];
	size_t tx_length = 0u;

	aproto_status_t status = aproto_parse(&protocol, rx, rx_length,
										  tx, sizeof(tx), &tx_length);
	if (tx_length != 0u) {
		hardware_send((const uint8_t *)tx, tx_length);
	}
	(void)status;
}
```

Use `tx_length`, not `strlen()`: output is length-delimited and need not be
null-terminated when it exactly fills the buffer. `aproto_parse()` accepts one
complete frame per call; byte reception and frame buffering belong to the
application.

Use `aproto_build_frame()` to generate outbound commands:

```c
size_t length;
aproto_build_frame(tx, sizeof(tx), "GAIN,12", 7u, &length);
```

## Built-in commands

| Command | Purpose |
|---------|---------|
| `CRC,0` / `CRC,1` / `CRC?` | Disable, enable, or query receive checksum validation |
| `ACK,0` / `ACK,1` / `ACK?` | Disable, enable, or query successful action acknowledgements |
| `VER?` | Return the protocol version |

CRC and ACK start enabled. To disable CRC checking immediately, send this
frame with its valid XOR checksum:

```text
$CRC,0*4E\r\n
```

Define `APROTO_CRC_DEFAULT=0` at compile time to start with CRC checking
disabled. To disable successful acknowledgements immediately, send:

```text
$ACK,0*55\r\n
```

Define `APROTO_ACK_DEFAULT=0` at compile time to start with acknowledgements
disabled. With ACK disabled, successful
`APROTO_COMMAND_OK` responses are suppressed; data responses and errors are
still transmitted. Built-in names cannot be replaced by application commands.

Formatting and numeric conversion use `eprintfs`.