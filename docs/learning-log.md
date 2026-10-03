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

## Session 6 — Interrupt flow and UART communication

- Traced the complete TIM2 interrupt path from the hardware update event to application code.
- Studied the TIM2 update flag, interrupt enable and NVIC handling.
- Located `TIM2_IRQHandler()` in the Cortex-M interrupt vector table.
- Understood how weak handlers and HAL callbacks allow application-specific interrupt handling.
- Configured USART2 in asynchronous 115200 8N1 mode.
- Used PA2 and PA3 through their USART2 alternate functions.
- Sent serial data from the STM32 to a Linux host through the onboard ST-LINK virtual serial port.
- Implemented a blocking UART echo between the PC and STM32.
- Observed that TIM2 interrupts continue executing while the main flow waits for UART input.
- See [Interrupt flow](interrupt-flow.md) and [UART basics](uart-basics.md).

## Session 7 — V1 sensor strategy

- Defined rotating machinery as the initial condition-monitoring use case.
- Selected vibration as the first physical quantity to monitor.
- Chose a 3-axis digital accelerometer as the first sensor type.
- Introduced I²C and SPI and compared their trade-offs for sensor acquisition.
- Introduced sampling frequency, Nyquist frequency and aliasing.
- Set an initial target of roughly 0–500 Hz useful vibration bandwidth with a 2 kHz sampling rate.
- Compared LIS3DH, ADXL355 and IIS3DWB as candidate accelerometers.
- Selected the ADXL355 as the current leading candidate for V1, with SPI as the preferred interface.
- See [Sampling basics](sampling-basics.md) for sampling frequency, Nyquist and aliasing.

## Session 8 — ADXL355 and SPI preparation

- Clarified the physical monitoring setup: the accelerometer is mechanically fixed to the rotating machine and measures its vibration.
- Studied the ADXL355 SPI interface and the role of SCK, MOSI, MISO and CS.
- Introduced SPI Mode 0 (`CPOL = 0`, `CPHA = 0`) and software-controlled chip select.
- Configured SPI2 on the STM32F401RE:
  - PB10: SCK
  - PC2: MISO
  - PC3: MOSI
  - PB12: ADXL355 chip select
- Set the SPI prescaler to 8, giving an SCK frequency of approximately 5.25 MHz.
- Configured the ADXL355 CS pin to remain HIGH when idle.
- Prepared the firmware for the first ADXL355 register read.

## Session 9 — First ADXL355 driver function

- Clarified the STM32 hardware-peripheral model:
  - TIM2, USART2 and SPI2 are dedicated hardware peripherals.
  - GPIO ports are also hardware peripherals, but directly control physical pins.
  - PA5, PB12, PB10, PC2 and PC3 are physical MCU pins.
  - Alternate Functions connect pins to internal peripherals such as SPI2.
- Clarified the role of STM32 HAL handles such as `htim2`, `huart2` and `hspi2`.
- Created the first ADXL355 driver files:
  - `Core/Inc/adxl355.h`
  - `Core/Src/adxl355.c`
- Added the ADXL355 `PARTID` register constants.
- Implemented the first generic register-read function using:
  - software-controlled chip select;
  - `HAL_SPI_TransmitReceive`;
  - a command byte containing the register address and READ bit;
  - a dummy byte to clock the returned data.
- Verified the firmware still builds successfully with 0 errors and 0 warnings.
- See [STM32 MCU peripherals](mcu-peripherals.md) for the MCU peripheral and pin model.
