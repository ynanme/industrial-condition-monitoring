# Glossary

| Acronym / term | Meaning | Context |
|---|---|---|
| AHB | Advanced High-performance Bus | High-speed internal bus in the STM32 clock/bus architecture |
| APB | Advanced Peripheral Bus | Bus connecting many STM32 peripherals; TIM2 is on APB1 |
| ARR | Auto-Reload Register | Timer value defining when the counter resets and generates an update event |
| BSRR | Bit Set/Reset Register | GPIO register used to atomically set or reset output pins |
| CMSIS | Cortex Microcontroller Software Interface Standard | ARM software interface used underneath STM32 libraries |
| EXTI | External Interrupt/Event Controller | STM32 block used to generate interrupts/events from external lines |
| GPIO | General-Purpose Input/Output | Configurable digital input/output pins |
| HAL | Hardware Abstraction Layer | ST library providing higher-level access to STM32 hardware |
| IDE | Integrated Development Environment | Software used to edit, build, flash and debug code; here STM32CubeIDE |
| IRQ | Interrupt Request | Hardware request asking the CPU to service an interrupt |
| ISR | Interrupt Service Routine | Function executed in response to an interrupt |
| MCU | Microcontroller Unit | The microcontroller chip itself; here STM32F401RET6 |
| MODER | Mode Register | GPIO register selecting Input, Output, Alternate Function or Analog mode |
| NVIC | Nested Vectored Interrupt Controller | Cortex-M hardware block managing interrupts and priorities |
| ODR | Output Data Register | GPIO register representing output states |
| OSPEEDR | Output Speed Register | GPIO register controlling output switching speed |
| OTYPER | Output Type Register | GPIO register selecting push-pull or open-drain output |
| PSC | Prescaler | Timer divider used to reduce the timer input clock |
| PUPDR | Pull-Up/Pull-Down Register | GPIO register controlling internal pull resistors |
| RCC | Reset and Clock Control | STM32 block responsible for peripheral clocks and resets |
| SysTick | System Tick | Cortex-M timer used by the HAL as its default 1 ms time base |