#include "aproto/aproto.h"

#include "eprintfs/eprintfs.h"

#define APROTO_FRAME_OVERHEAD 6u
#define APROTO_NUMBER_BUFFER_SIZE 48u

static const char *const aproto_builtin_names[] = {
    "CRC",
    "ACK",
    "VER",
    "CMDS"
};

static int aproto_name_equal(const char *name, const aproto_string_t *command)
{
    size_t index = 0;

    while (name[index] != '\0' && index < command->length)
    {
        if (name[index] != command->data[index])
        {
            return 0;
        }
        ++index;
    }

    return name[index] == '\0' && index == command->length;
}

static int aproto_names_equal(const char *first, const char *second)
{
    size_t index = 0;

    while (first[index] != '\0' && second[index] != '\0')
    {
        if (first[index] != second[index])
        {
            return 0;
        }
        ++index;
    }

    return first[index] == second[index];
}

static uint8_t aproto_xor(const char *data, size_t length)
{
    uint8_t checksum = 0;
    size_t index;

    for (index = 0; index < length; ++index)
    {
        checksum ^= (uint8_t)data[index];
    }
    return checksum;
}

static int aproto_hex_value(char character)
{
    if (character >= '0' && character <= '9')
    {
        return character - '0';
    }
    if (character >= 'A' && character <= 'F')
    {
        return character - 'A' + 10;
    }
    if (character >= 'a' && character <= 'f')
    {
        return character - 'a' + 10;
    }
    return -1;
}

static size_t aproto_text_length(const char *text)
{
    return (size_t)estrlen(text);
}

int aproto_response_append_n(aproto_response_t *response,
                             const char *text,
                             size_t length)
{
    size_t index;

    if (response == NULL || text == NULL || response->overflow)
    {
        return -1;
    }
    if (length > response->capacity - response->length)
    {
        response->overflow = 1;
        return -1;
    }

    for (index = 0; index < length; ++index)
    {
        response->data[response->length + index] = text[index];
    }
    response->length += length;
    return 0;
}

int aproto_response_append(aproto_response_t *response, const char *text)
{
    if (text == NULL)
    {
        return -1;
    }
    return aproto_response_append_n(response, text, aproto_text_length(text));
}

int aproto_response_append_int(aproto_response_t *response, int value)
{
    char number[APROTO_NUMBER_BUFFER_SIZE];

    eitoa(value, number, 10);
    return aproto_response_append(response, number);
}

int aproto_response_append_float(aproto_response_t *response,
                                 float value,
                                 unsigned int precision)
{
    char number[APROTO_NUMBER_BUFFER_SIZE];

    if (precision > 9u)
    {
        return -1;
    }
    esprintf(number, "%.*f", (int)precision, value);
    return aproto_response_append(response, number);
}

static int aproto_find_argument(const aproto_request_t *request,
                                size_t wanted,
                                aproto_string_t *argument)
{
    size_t start = 0;
    size_t current = 0;
    size_t index;

    if (request == NULL || argument == NULL || request->arguments.length == 0u)
    {
        return -1;
    }

    for (index = 0; index <= request->arguments.length; ++index)
    {
        if (index == request->arguments.length ||
            request->arguments.data[index] == ',')
        {
            if (current == wanted)
            {
                if (index == start)
                {
                    return -1;
                }
                argument->data = request->arguments.data + start;
                argument->length = index - start;
                return 0;
            }
            ++current;
            start = index + 1u;
        }
    }
    return -1;
}

size_t aproto_argument_count(const aproto_request_t *request)
{
    size_t count = 0;
    size_t index;

    if (request == NULL || request->arguments.length == 0u)
    {
        return 0u;
    }

    count = 1u;
    for (index = 0; index < request->arguments.length; ++index)
    {
        if (request->arguments.data[index] == ',')
        {
            ++count;
        }
    }
    return count;
}

int aproto_argument_string(const aproto_request_t *request,
                           size_t index,
                           aproto_string_t *argument)
{
    return aproto_find_argument(request, index, argument);
}

static int aproto_copy_argument(const aproto_request_t *request,
                                size_t index,
                                char *buffer,
                                size_t capacity)
{
    aproto_string_t argument;
    size_t position;

    if (aproto_find_argument(request, index, &argument) != 0 ||
        argument.length + 1u > capacity)
    {
        return -1;
    }

    for (position = 0; position < argument.length; ++position)
    {
        buffer[position] = argument.data[position];
    }
    buffer[argument.length] = '\0';
    return 0;
}

int aproto_argument_int(const aproto_request_t *request,
                        size_t index,
                        int *value)
{
    char buffer[APROTO_NUMBER_BUFFER_SIZE];
    char *stop;

    if (value == NULL ||
        aproto_copy_argument(request, index, buffer, sizeof(buffer)) != 0)
    {
        return -1;
    }

    *value = eatoi(buffer, 10, &stop);
    return stop != buffer && *stop == '\0' ? 0 : -1;
}

int aproto_argument_float(const aproto_request_t *request,
                          size_t index,
                          float *value)
{
    char buffer[APROTO_NUMBER_BUFFER_SIZE];
    char *stop;

    if (value == NULL ||
        aproto_copy_argument(request, index, buffer, sizeof(buffer)) != 0)
    {
        return -1;
    }

    *value = eatof(buffer, &stop);
    return stop != buffer && *stop == '\0' ? 0 : -1;
}

aproto_status_t aproto_build_frame(char *output,
                                   size_t output_capacity,
                                   const char *body,
                                   size_t body_length,
                                   size_t *output_length)
{
    static const char hex[] = "0123456789ABCDEF";
    uint8_t checksum;
    size_t index;

    if (output_length != NULL)
    {
        *output_length = 0u;
    }
    if (output == NULL || body == NULL ||
        body_length + APROTO_FRAME_OVERHEAD > output_capacity)
    {
        return APROTO_STATUS_OVERFLOW;
    }

    checksum = aproto_xor(body, body_length);
    output[0] = '$';
    for (index = 0; index < body_length; ++index)
    {
        output[index + 1u] = body[index];
    }
    output[body_length + 1u] = '*';
    output[body_length + 2u] = hex[checksum >> 4];
    output[body_length + 3u] = hex[checksum & 0x0fu];
    output[body_length + 4u] = '\r';
    output[body_length + 5u] = '\n';
    if (body_length + APROTO_FRAME_OVERHEAD < output_capacity)
    {
        output[body_length + APROTO_FRAME_OVERHEAD] = '\0';
    }
    if (output_length != NULL)
    {
        *output_length = body_length + APROTO_FRAME_OVERHEAD;
    }
    return APROTO_STATUS_OK;
}

aproto_status_t aproto_init(aproto_t *proto,
                            const aproto_command_t *commands,
                            size_t command_count)
{
    size_t index;
    size_t other;
    size_t builtin;

    if (proto == NULL || (command_count != 0u && commands == NULL))
    {
        return APROTO_STATUS_CONFIG_ERROR;
    }

    proto->commands = commands;
    proto->command_count = command_count;
    proto->crc_enabled = APROTO_CRC_DEFAULT != 0;
    proto->ack_enabled = APROTO_ACK_DEFAULT != 0;
    proto->configured = 0;

    for (index = 0; index < command_count; ++index)
    {
        if (commands[index].name == NULL || commands[index].name[0] == '\0' ||
            commands[index].handler == NULL)
        {
            return APROTO_STATUS_CONFIG_ERROR;
        }
        for (builtin = 0;
             builtin < sizeof(aproto_builtin_names) / sizeof(aproto_builtin_names[0]);
             ++builtin)
        {
            if (aproto_names_equal(commands[index].name,
                                   aproto_builtin_names[builtin]))
            {
                return APROTO_STATUS_CONFIG_ERROR;
            }
        }
        for (other = 0; other < index; ++other)
        {
            if (aproto_names_equal(commands[index].name, commands[other].name))
            {
                return APROTO_STATUS_CONFIG_ERROR;
            }
        }
    }

    proto->configured = 1;
    return APROTO_STATUS_OK;
}

static aproto_status_t aproto_emit_simple(char *output,
                                          size_t output_capacity,
                                          size_t *output_length,
                                          const char *prefix,
                                          const aproto_string_t *command,
                                          const aproto_response_t *tail)
{
    aproto_response_t response;
    size_t prefix_length = aproto_text_length(prefix);
    size_t head_length = prefix_length + command->length;
    size_t index;

    if (output_capacity < APROTO_FRAME_OVERHEAD)
    {
        return APROTO_STATUS_OVERFLOW;
    }
    response.data = output + 1u;
    response.capacity = output_capacity - APROTO_FRAME_OVERHEAD;
    response.length = 0u;
    response.overflow = 0;
    if (tail != NULL && tail->length != 0u && tail->data == response.data)
    {
        if (head_length + tail->length > response.capacity)
        {
            return APROTO_STATUS_OVERFLOW;
        }
        for (index = tail->length; index > 0u; --index)
        {
            response.data[head_length + index - 1u] = tail->data[index - 1u];
        }
    }
    aproto_response_append(&response, prefix);
    aproto_response_append_n(&response, command->data, command->length);
    if (tail != NULL && tail->length != 0u)
    {
        if (tail->data == response.data)
        {
            response.length += tail->length;
        }
        else
        {
            aproto_response_append_n(&response, tail->data, tail->length);
        }
    }
    if (response.overflow)
    {
        return APROTO_STATUS_OVERFLOW;
    }
    return aproto_build_frame(output, output_capacity, output + 1u,
                              response.length, output_length);
}

static aproto_status_t aproto_builtin(aproto_t *proto,
                                      const aproto_request_t *request,
                                      char *output,
                                      size_t output_capacity,
                                      size_t *output_length,
                                      int *handled)
{
    char body[32];
    aproto_response_t response;
    int value;
    size_t index;
    size_t length;

    *handled = 1;
    if (aproto_name_equal("CMDS", &request->command))
    {
        if (!request->query || request->arguments.length != 0u)
        {
            aproto_emit_simple(output, output_capacity, output_length,
                               "ERR,", &request->command, NULL);
            return APROTO_STATUS_COMMAND_ERROR;
        }
        if (output_capacity < APROTO_FRAME_OVERHEAD)
        {
            return APROTO_STATUS_OVERFLOW;
        }
        response.data = output + 1u;
        response.capacity = output_capacity - APROTO_FRAME_OVERHEAD;
        response.length = 0u;
        response.overflow = 0;
        aproto_response_append(&response, "OK,CMDS");
        for (index = 0u;
             index < sizeof(aproto_builtin_names) / sizeof(aproto_builtin_names[0]);
             ++index)
        {
            aproto_response_append(&response, ",");
            aproto_response_append(&response, aproto_builtin_names[index]);
        }
        for (index = 0u; index < proto->command_count; ++index)
        {
            aproto_response_append(&response, ",");
            aproto_response_append(&response, proto->commands[index].name);
        }
        if (response.overflow)
        {
            return APROTO_STATUS_OVERFLOW;
        }
        return aproto_build_frame(output, output_capacity, output + 1u,
                                  response.length, output_length);
    }
    if (aproto_name_equal("VER", &request->command))
    {
        if (!request->query || request->arguments.length != 0u)
        {
            aproto_emit_simple(output, output_capacity, output_length,
                               "ERR,", &request->command, NULL);
            return APROTO_STATUS_COMMAND_ERROR;
        }
        length = (size_t)esprintf(body, "OK,VER,%d.%d",
                     APROTO_VERSION_MAJOR, APROTO_VERSION_MINOR);
        return aproto_build_frame(output, output_capacity, body,
                                  (size_t)length, output_length);
    }
    if (!aproto_name_equal("CRC", &request->command) &&
        !aproto_name_equal("ACK", &request->command))
    {
        *handled = 0;
        return APROTO_STATUS_OK;
    }

    if (request->query)
    {
        if (request->arguments.length != 0u)
        {
            aproto_emit_simple(output, output_capacity, output_length,
                               "ERR,", &request->command, NULL);
            return APROTO_STATUS_COMMAND_ERROR;
        }
        value = aproto_name_equal("CRC", &request->command) ?
                    proto->crc_enabled : proto->ack_enabled;
    }
    else
    {
        if (aproto_argument_count(request) != 1u ||
            aproto_argument_int(request, 0u, &value) != 0 ||
            (value != 0 && value != 1))
        {
            aproto_emit_simple(output, output_capacity, output_length,
                               "ERR,", &request->command, NULL);
            return APROTO_STATUS_COMMAND_ERROR;
        }
        if (aproto_name_equal("CRC", &request->command))
        {
            proto->crc_enabled = value;
        }
        else
        {
            proto->ack_enabled = value;
        }
    }

    response.data = body;
    response.capacity = sizeof(body);
    response.length = 0u;
    response.overflow = 0;
    aproto_response_append(&response, "OK,");
    aproto_response_append_n(&response, request->command.data,
                             request->command.length);
    aproto_response_append(&response, ",");
    aproto_response_append_int(&response, value);
    if (response.overflow)
    {
        return APROTO_STATUS_OVERFLOW;
    }
    length = response.length;
    return aproto_build_frame(output, output_capacity, body,
                              length, output_length);
}

aproto_status_t aproto_parse(aproto_t *proto,
                             const uint8_t *input,
                             size_t input_length,
                             char *output,
                             size_t output_capacity,
                             size_t *output_length)
{
    aproto_request_t request;
    aproto_response_t response;
    aproto_command_result_t command_result;
    const aproto_command_t *command = NULL;
    size_t end = 0u;
    size_t star = 0u;
    size_t separator;
    size_t index;
    int high;
    int low;
    int handled;
    aproto_status_t status;

    if (output_length != NULL)
    {
        *output_length = 0u;
    }
    if (proto == NULL || !proto->configured || input == NULL || output == NULL)
    {
        return APROTO_STATUS_CONFIG_ERROR;
    }

    while (end < input_length && input[end] != '\0' &&
           input[end] != '\r' && input[end] != '\n')
    {
        ++end;
    }
    if (end < 5u || input[0] != '$')
    {
        return APROTO_STATUS_FRAME_ERROR;
    }
    for (index = 1u; index < end; ++index)
    {
        if (input[index] == '*')
        {
            star = index;
            break;
        }
    }
    if (star < 2u || star + 3u != end)
    {
        return APROTO_STATUS_FRAME_ERROR;
    }
    high = aproto_hex_value((char)input[star + 1u]);
    low = aproto_hex_value((char)input[star + 2u]);
    if (high < 0 || low < 0)
    {
        return APROTO_STATUS_FRAME_ERROR;
    }

    request.command.data = (const char *)input + 1u;
    request.command.length = star - 1u;
    request.arguments.data = NULL;
    request.arguments.length = 0u;
    request.query = 0;

    for (separator = 0u; separator < request.command.length; ++separator)
    {
        char character = request.command.data[separator];
        if (character == ',' || character == '?')
        {
            request.command.length = separator;
            if (character == ',')
            {
                request.arguments.data = (const char *)input + separator + 2u;
                request.arguments.length = star - separator - 2u;
            }
            else
            {
                request.query = 1;
                if (separator + 2u != star)
                {
                    request.arguments.data = (const char *)input + separator + 2u;
                    request.arguments.length = star - separator - 2u;
                }
            }
            break;
        }
    }
    if (request.command.length == 0u)
    {
        return APROTO_STATUS_FRAME_ERROR;
    }

    if (proto->crc_enabled &&
        aproto_xor((const char *)input + 1u, star - 1u) !=
            (uint8_t)((high << 4) | low))
    {
        static const char crc_error[] = "ERR,CRC";
        aproto_build_frame(output, output_capacity, crc_error,
                           sizeof(crc_error) - 1u, output_length);
        return APROTO_STATUS_FRAME_ERROR;
    }

    status = aproto_builtin(proto, &request, output, output_capacity,
                            output_length, &handled);
    if (handled)
    {
        return status;
    }

    for (index = 0u; index < proto->command_count; ++index)
    {
        if (aproto_name_equal(proto->commands[index].name, &request.command))
        {
            command = &proto->commands[index];
            break;
        }
    }
    if (command == NULL)
    {
        aproto_emit_simple(output, output_capacity, output_length,
                           "ERR,", &request.command, NULL);
        return APROTO_STATUS_COMMAND_ERROR;
    }

    response.data = output + 1u;
    response.capacity = output_capacity > APROTO_FRAME_OVERHEAD ?
                            output_capacity - APROTO_FRAME_OVERHEAD : 0u;
    response.length = 0u;
    response.overflow = 0;
    command_result = command->handler(&request, &response, command->context);
    if (response.overflow)
    {
        return APROTO_STATUS_OVERFLOW;
    }

    if (command_result == APROTO_COMMAND_ERROR)
    {
        aproto_emit_simple(output, output_capacity, output_length,
                           "ERR,", &request.command, NULL);
        return APROTO_STATUS_COMMAND_ERROR;
    }
    if (command_result == APROTO_COMMAND_DATA)
    {
        return aproto_build_frame(output, output_capacity, output + 1u,
                                  response.length, output_length);
    }
    if (!proto->ack_enabled)
    {
        return APROTO_STATUS_OK;
    }
    return aproto_emit_simple(output, output_capacity, output_length,
                              "OK,", &request.command, &response);
}