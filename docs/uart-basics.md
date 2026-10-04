# UART Basics

## Configuration

The project currently uses USART2 in asynchronous mode:

- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1
- Mode: TX/RX

This configuration is commonly written as:

```text
115200 8N1
```

## NUCLEO-F401RE connection

USART2 is mapped to:

```text
PA2 -> USART2_TX
PA3 -> USART2_RX
```

The pins use Alternate Function 7 (`AF7`) to connect them to the USART2
peripheral instead of behaving as ordinary GPIO pins.

On the NUCLEO-F401RE, USART2 is connected to the onboard ST-LINK virtual
serial port.

The complete path is therefore:

```text
STM32 USART2
      |
      v
ST-LINK
      |
     USB
      |
      v
Linux /dev/ttyACM0
      |
      v
Serial terminal
```

## First communication tests

The firmware first transmitted a message from the STM32 to the PC using
`HAL_UART_Transmit()`.

A second test implemented a blocking serial echo:

```text
PC
 |
 v
HAL_UART_Receive()
 |
 v
STM32
 |
 v
HAL_UART_Transmit()
 |
 v
PC
```

While the main loop was blocked waiting for UART data, the TIM2 interrupt
continued to toggle LD2 every 500 ms.

This demonstrated that interrupt-driven activity can continue while the main
execution flow is blocked.

## Linux serial console

Connect to the NUCLEO virtual serial port with:

```bash
picocom -b 115200 /dev/ttyACM0
```

- `picocom`: serial terminal program
- `-b 115200`: UART baud rate
- `/dev/ttyACM0`: Linux serial device exposed by the ST-LINK VCP

Exit with `Ctrl+A`, then `Ctrl+X`.
