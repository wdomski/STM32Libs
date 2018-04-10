# STM32Libs
Miscellaneous algorithm and hardware libraries for STM32

# Available libraries

## circ buff -- Circular buffer

This library contains files implementing circular buffer using DMA with UART.
It allows to read data from UART in non--blocking mode and setting a timeout.
This timeout allows to measure time gap between burst of transmissions and 
this is most likely a single command, line of text, response, etc. from 
external device.

### Example

You can define circ buff logic and hardware structures:
```C
circ_buff_hw_t cb_hw;
circ_buff_t cb;
```

Initialization of the interface is following
```C
//initialize hardware
circ_buff_hw_init(&cb_hw, &hdma_usart2_rx, &huart2);
//initialize circ buff logic
circ_buff_init(&cb, &cb_hw, buffer_uart_rx, 512, 50);
//start receiving
circ_buff_start_rx(&cb);
```

Checking if there is a new portion of data and reading it to a buffer is following
```C
if (circ_buff_new_data(&cb)) {
	len = circ_buff_new_data_amount(&cb);
	if (len > 0) {
		circ_buff_read(&cb, buffer, len);
	}
}
```
