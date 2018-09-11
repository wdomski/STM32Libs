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

## PID -- proportional integral derivative controller

Library facilitating implementation of PID controller.
It comes with two flavours: float and int32.

### Example

Create data structures and initialize them:
```C
pid_f32_t pid_data;
float P, I, D, DT_S;
float u, mv, dv;

P = 1.0000f;
I = 0.0000f;
D = 0.1234f;
DT_S = 0.010f; //freq = 100Hz, period = 10 ms
```

Initialize PID data structure and set limits:
```C
pid_f32_init(&pid_data, P, I, D, DT_S);
pid_data.p_max = 3000;
pid_data.p_min = -3000;
pid_data.i_max = 3000;
pid_data.i_min = -3000;
pid_data.d_max = 3000;
pid_data.d_min = -3000;
pid_data.total_max = 3000;
pid_data.total_min = -3000;
```

To reset integral term (sets integral sum to 0).
It only make sense when you use integral term; 
I coefficient is different from 0.
```C
pid_f32_reset_int(&pid_data);
```

Calculate control signal with PID controller:
```C
mv = 10.0f;
dv = -5.0f;
u = pid_f32_calc(&pid_dat, mv, dv);
```


