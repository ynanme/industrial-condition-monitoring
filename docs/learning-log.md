# Learning Log

## Session 1 — Toolchain and first firmware

- Set up STM32CubeMX + STM32CubeIDE workflow.
- Generated firmware for the NUCLEO-F401RE.
- Built and flashed firmware through the onboard ST-LINK.
- Implemented a first GPIO-based LED blink.
- Learned the role of CubeMX, HAL, CMSIS and the `.ioc` file.

## Session 2 — GPIO internals

- Traced `HAL_GPIO_TogglePin()` down to STM32 GPIO registers.
- Learned how GPIO ports group multiple physical pins.
- Understood bit masks such as `GPIO_PIN_5`.
- Studied `ODR` (Output Data Register) and `BSRR` (Bit Set/Reset Register).
- Understood how PA5 is configured as an output and drives the Nucleo LD2 LED.

## Session 3 — GPIO configuration and system time

- Traced `HAL_GPIO_Init()` down to STM32 GPIO configuration registers.
- Learned how `MODER`, `OTYPER`, `OSPEEDR` and `PUPDR` configure a GPIO pin.
- Understood the read-mask-modify-write pattern used to update register fields without affecting other pins.
- Traced `HAL_Delay()` through `HAL_GetTick()`, `uwTick` and `SysTick_Handler()`.
- Learned how the Cortex-M SysTick timer generates a 1 ms HAL time base.
- Introduced interrupt handling, ISR execution and NVIC interrupt priorities.
- Understood why blocking delays are unsuitable for precise periodic acquisition.

## Session 4 — Non-blocking software timing

- Replaced `HAL_Delay()` with non-blocking timing based on `HAL_GetTick()`.
- Used an elapsed-time check to toggle LD2 every 500 ms while keeping the main loop available.
- Understood the role of `last_toggle` as a timestamp of the previous event.
- Learned why `HAL_GetTick() - last_toggle >= period` remains safe across unsigned tick overflow.
- Compared blocking delays with polling-based periodic execution.

## Session 5 — Hardware timers and periodic interrupts

- Replaced software polling with a hardware timer for periodic execution.
- Configured TIM2 from the 84 MHz APB1 timer clock.
- Learned how the prescaler and auto-reload register determine the timer period.
- Generated a TIM2 interrupt every 500 ms.
- Used the HAL timer callback to toggle LD2 independently from the main loop.
- Introduced interrupt latency, jitter and the distinction between periodic hardware events and real-time guarantees.
- See [Timer basics](timer-basics.md) for the timer configuration and calculation.