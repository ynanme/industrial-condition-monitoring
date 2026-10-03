# STM32 MCU Peripherals

An STM32 microcontroller is not only a CPU connected to pins.

It contains a CPU surrounded by dedicated hardware peripherals, each designed
for a particular task.

```text
STM32F401
├── Cortex-M4 CPU
├── GPIO peripherals
├── timers
├── USART/UART peripherals
├── SPI peripherals
└── other hardware peripherals
```

## Hardware peripherals

A peripheral is a hardware block inside the MCU dedicated to a specific
function.

Examples used in this project:

```text
TIM2   → timing and periodic events
USART2 → asynchronous serial communication
SPI2   → synchronous serial communication
GPIO   → direct control and observation of pins
```

These operations are therefore not performed entirely in software by the CPU.

For example, SPI2 can generate its clock and shift bits through MOSI and MISO
without the CPU manually toggling a GPIO for every bit.

## Peripheral instances

A peripheral family can have several instances:

```text
TIM1, TIM2, ...
USART1, USART2, ...
SPI1, SPI2, SPI3
GPIOA, GPIOB, GPIOC, ...
```

`SPI2` therefore refers to one specific SPI hardware block inside the MCU.

## Peripheral handles

The STM32 HAL represents configured peripherals using handle structures.

Examples:

```c
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;
SPI_HandleTypeDef hspi2;
```

The handle is the software object used by the HAL to access and manage a
specific hardware peripheral.

For example:

```c
hspi2.Instance = SPI2;
```

associates the `hspi2` handle with the physical SPI2 peripheral.

## GPIO and physical pins

GPIO peripherals are slightly different from communication peripherals because
their main role is to directly control physical pins.

For example:

```text
GPIOA → PA0, PA1, ..., PA15
GPIOB → PB0, PB1, ..., PB15
```

`GPIOB` is a hardware peripheral.

`PB12` is one physical pin belonging to port B.

A GPIO pin can typically be configured as:

```text
Input
Output
Alternate Function
Analog
```

## Alternate Functions

Some physical pins can be connected internally to specialized peripherals.

For example, in the current project:

```text
PB10 → SPI2_SCK
PC2  → SPI2_MISO
PC3  → SPI2_MOSI
```

These pins are configured using an Alternate Function rather than as ordinary
GPIO outputs.

The SPI2 hardware peripheral then controls them.

In contrast:

```text
PB12 → ADXL355_CS
```

is configured as a normal GPIO output and is controlled explicitly by software.

## Current project mapping

```text
TIM2
└── periodic 500 ms interrupt
    └── LED toggle

USART2
├── TX: PA2
├── RX: PA3
└── STM32 ↔ ST-LINK ↔ USB ↔ Linux

SPI2
├── SCK:  PB10
├── MISO: PC2
├── MOSI: PC3
└── STM32 ↔ ADXL355

GPIO
├── PA5  → LD2
└── PB12 → ADXL355 chip select
```

The physical pins expose the MCU's internal hardware peripherals to the outside
world.