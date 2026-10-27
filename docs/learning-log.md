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

## Session 10 — Robust register access and UART behaviour

- Reviewed the STM32 peripheral model: SPI2, its HAL handle, physical pins and the external ADXL355.
- Improved `ADXL355_ReadRegister()` to return a HAL status separately from the register value.
- Clarified that `HAL_OK` validates the STM32-side SPI transfer, not the identity or correctness of the external device.
- Implemented `ADXL355_WriteRegister()` using a two-byte SPI transaction.
- Introduced register readback as a way to verify configuration writes.
- Clarified hardware registers as addressable configuration/status/data storage inside peripherals.
- Prepared a `PARTID` verification in `main.c` with UART diagnostic messages.
- Tested SPI without an ADXL355 connected and confirmed that the transaction can succeed while the returned Part ID is unexpected.
- Explored blocking UART reception, timeouts and the persistence of the last received byte when timeout results are ignored.
- Documented the Linux `picocom` serial-console command.

## Session 11 — Sampling and filtering fundamentals

- Read the ADXL355 `FILTER` register description and its `ODR_LPF` configuration table.
- Interpreted the register fields, including the reserved bit, `HPF_CORNER` and `ODR_LPF`.
- Clarified the difference between:
  - acceleration samples expressed in `g`;
  - output data rate expressed in samples per second;
  - vibration frequency expressed in hertz.
- Clarified that vibration frequency is derived from a sequence of acceleration samples rather than measured directly at one instant.
- Studied why a low-pass filter can attenuate high-frequency content without explicitly calculating vibration frequencies.
- Clarified that filtering changes sample values but does not reduce the configured 2000 samples/s output rate.
- See [Sampling and filtering](sampling-and-filtering.md).

## Session 12 — ADXL355 configuration and initialization

- Revisited the relationship between ODR, internal filtering and the final acceleration samples exposed by the ADXL355.
- Studied the `RANGE` register and selected the ±2 g range for small vibration measurements.
- Clarified range, saturation and resolution trade-offs.
- Practiced read-modify-write operations on register bit fields using bit masks.
- Studied the `POWER_CTL` register and the `STANDBY` bit used to enter measurement mode.
- Implemented the first `ADXL355_Init()` sequence:
  - verify `PARTID`;
  - configure `FILTER` for 2 kHz ODR / 500 Hz LPF;
  - configure the ±2 g range;
  - clear `STANDBY` to start measurements.
  
## Session 13 — ADXL355 raw data representation

- Studied how each ADXL355 axis is encoded as a signed 20-bit value across three 8-bit registers.
- Reconstructed a 20-bit axis value using bit shifts and bitwise OR operations.
- Clarified why the three data registers are treated as raw unsigned bytes rather than independent signed values.
- Studied two's complement representation and why sign extension is required when storing a signed 20-bit value in an `int32_t`.
- Derived the sign-extension method using bit 19 as the sign bit.

## Session 14 — ADXL355 hardware bring-up and first real measurement

- Received and connected the EVAL-ADXL355-PMDZ board to the NUCLEO-F401RE over SPI.
- Mapped the ADXL355 SPI signals to the Nucleo board connectors:
  - `CS` → PB12
  - `MOSI` → PC3
  - `MISO` → PC2
  - `SCLK` → PB10
  - `VDD` → 3.3 V
  - `GND` → GND
- Clarified the distinction between STM32 MCU pin names such as `PB10` and physical Nucleo connector pins such as `CN7` / `CN10`.
- Reviewed the CubeMX SPI2 pin configuration and the role of the custom `ADXL355_CS` GPIO label.
- Successfully validated real SPI communication by reading and verifying the ADXL355 `PARTID`.
- Replaced the standalone `PARTID` test in `main()` with `ADXL355_Init()`.
- Implemented `ADXL355_ReadX()`:
  - read `XDATA3`, `XDATA2` and `XDATA1`;
  - reconstructed the 20-bit raw value;
  - performed sign extension;
  - returned the result as `int32_t`.
- Successfully read real positive and negative X-axis acceleration values from the physical sensor.

## Session 15 — Three-axis burst reading

- Converted raw ADXL355 acceleration values to `g` using the ±2.048 g range sensitivity of 256000 LSB/g.
- Clarified the meaning of sensor sensitivity expressed in LSB/g.
- Identified the contiguous X, Y and Z data registers from `0x08` to `0x10`.
- Studied ADXL355 multi-byte reads and register address auto-increment.
- Implemented `ADXL355_ReadRegisters()` for burst SPI reads with a fixed maximum buffer size.
- Refactored `ADXL355_ReadRegister()` as a wrapper around the multi-register read function.
- Discussed fixed-size buffers versus variable-length arrays and predictable stack usage in embedded systems.
- Implemented a private `ADXL355_DecodeAxis()` helper for 20-bit reconstruction and sign extension.
- Implemented `ADXL355_ReadAcceleration()` to read X, Y and Z in a single 9-register SPI burst.
- Successfully validated real three-axis acceleration readings on the physical sensor.

## Session 16 — Live acceleration monitoring

- Enabled floating-point formatting for `printf` / `snprintf` using the linker option for newlib-nano.
- Displayed X, Y and Z acceleration values directly in `g`.
- Replaced the one-measurement-per-reset workflow with a non-blocking periodic acquisition using `HAL_GetTick()`.
- Removed the blocking UART receive from the main loop because it prevented periodic sensor reads from running independently.
- Validated live three-axis measurements at 10 Hz by physically rotating the sensor and observing gravity move between X, Y and Z.
- Clarified the distinction between the ADXL355 output data rate (2 kHz) and the slower debug/display rate.
- Introduced the target acquisition architecture: capture samples at sensor rate, store them in a buffer, then process or display them separately.

## Session 17 — DRDY interrupt preparation

- Chose a 512-sample block size as an initial acquisition buffer size.
- Introduced the role of `DRDY` (Data Ready) for synchronizing acquisition with the ADXL355 output data rate.
- Clarified the difference between GPIO polling and hardware interrupt-based event detection.
- Compared CPU execution times with the 0.5 ms sample period at 2 kHz.
- Connected the ADXL355 `DRDY` output to STM32 pin `PA0` (`A0`, CN8 pin 1).
- Configured `PA0` as `GPIO_EXTI0` in CubeMX.
- Configured a rising-edge external interrupt with no pull resistor.
- Enabled the corresponding EXTI interrupt in the NVIC.
- Regenerated the firmware configuration from CubeMX.

## Session 18 — Validating the ADXL355 data-ready interrupt

- Reviewed the GPIO/EXTI/NVIC code generated by CubeMX for the ADXL355 `DRDY` signal.
- Traced the complete interrupt path from the `PA0` rising edge to `EXTI0_IRQHandler()` and `HAL_GPIO_EXTI_Callback()`.
- Added a `volatile uint32_t` counter incremented from the GPIO EXTI callback.
- Physically validated the ADXL355 `DRDY` signal at approximately 2 kHz.
- Clarified why `volatile` is required for a variable modified asynchronously by an interrupt.
- Compared cumulative interrupt counting with per-second counting.
- Identified a measurement issue caused by reading `drdy_count` at different times around blocking UART transmissions; a single snapshot should be used for each measurement interval.
- Clarified that `DRDY` signals the availability of a sample, while the X/Y/Z measurements themselves will be stored in the acquisition buffer.

## Session 19 — First buffered acquisition

- Reviewed the distinction between the ADXL355 sampling clock, the STM32 clock and the SPI clock.
- Introduced a 512-sample acquisition buffer storing raw X, Y and Z acceleration values.
- Chose a Structure of Arrays layout with separate `x[]`, `y[]` and `z[]` arrays.
- Clarified the role of SRAM and why large long-lived buffers are better stored statically than as large local stack variables.
- Replaced the simple DRDY diagnostic counter logic with an event-production / event-consumption model using `drdy_events` and `handled_events`.
- Added SPI error counting and distinguished a handled DRDY event from a successfully stored sample.
- Implemented the first DRDY-driven buffered acquisition loop.
- Measured the time required to acquire a complete 512-sample block.
- Observed a clean acquisition result: 512 stored samples, 0 SPI errors and no final event backlog.
- Measured a buffer fill time of approximately 258 ms, consistent with the observed sensor sample rate.
- Identified an important startup issue: DRDY events generated before acquisition begins must be ignored, otherwise the main loop can consume a logical backlog and perform multiple reads without corresponding new sensor samples.

## Session 20 — Dual-buffer acquisition timing

- Reviewed why 512 samples contain 511 intervals between the first and last sample.
- Reviewed how pre-existing DRDY events can create a logical backlog and cause repeated reads without corresponding new sensor samples.
- Introduced two 512-sample acquisition buffers and an `active_buffer` pointer to switch between them.
- Acquired 1024 consecutive samples by filling buffer A and then buffer B.
- Measured approximately 515 ms for the complete two-buffer acquisition.
- Measured approximately 258 ms as the available processing budget while the second 512-sample buffer fills.
- Completed the acquisition with zero SPI errors and no DRDY event backlog.
- Clarified that the current implementation only validates buffer switching: acquisition still depends on `main()` performing the blocking SPI reads, so heavy processing cannot yet run concurrently with acquisition.

## Session 21 — Introducing DMA for asynchronous acquisition

- Identified the limitation of the current dual-buffer design: `main()` still performs every blocking SPI read, so heavy buffer processing would interrupt acquisition.
- Compared possible approaches including interrupt-driven acquisition, threads/RTOS and hardware-assisted transfers.
- Introduced DMA (Direct Memory Access) as a hardware peripheral capable of transferring data between SPI and memory without continuous CPU involvement.
- Clarified that DMA provides hardware concurrency: the CPU can execute code while SPI and DMA perform a transfer.
- Defined the intended acquisition flow: DRDY/EXTI starts a SPI DMA transfer, DMA transfers the bytes, and a transfer-complete callback finalizes the sample.
- Clarified that DMA only transfers bytes; decoding ADXL355 X/Y/Z data and higher-level signal processing still require CPU code.
- Distinguished blocking `HAL_SPI_TransmitReceive()` from asynchronous `HAL_SPI_TransmitReceive_DMA()`.
- Identified that the current synchronous `ADXL355_ReadAcceleration()` API cannot be reused unchanged for asynchronous DMA acquisition because the data is not yet available when the start function returns.

## Session 22 — Configuring SPI2 DMA in CubeMX

- Configured DMA support for SPI2 in CubeMX.
- Added separate DMA streams for SPI2 RX and TX.
- Configured peripheral-to-memory transfer for RX and memory-to-peripheral transfer for TX.
- Enabled memory address increment while keeping the SPI peripheral address fixed.
- Used byte-sized transfers and normal DMA mode.
- Enabled DMA1 Stream3 and Stream4 interrupts in the NVIC.
- Reviewed the generated `MX_DMA_Init()` function and DMA IRQ handlers.
- Reviewed the generated SPI2 DMA configuration inside `HAL_SPI_MspInit()`.
- Identified how `__HAL_LINKDMA()` associates the SPI handle with its RX and TX DMA handles.
- Clarified the relationship between CubeMX configuration, generated HAL code and underlying MCU peripherals.
- Introduced the lifetime constraint of DMA buffers: memory used by an asynchronous transfer must remain valid after the start function returns.
- Clarified why chip select cannot be released immediately after starting an asynchronous SPI DMA transfer.

## Session 23 — Understanding and starting SPI DMA acquisition

- Clarified the roles of DMA controller, stream, channel and peripheral DMA requests.
- Identified the DMA stream as the hardware transfer engine and the channel as the request-routing selection used by the stream.
- Clarified that SPI2 paces DMA transfers by issuing TX and RX requests when its data register is ready.
- Reviewed full-duplex SPI behavior with DMA: TX supplies the command and dummy bytes while RX simultaneously stores returned bytes.
- Clarified that the SPI peripheral address remains fixed while the DMA memory address increments through the TX and RX buffers.
- Related the generated CubeMX DMA configuration to the underlying SPI2 and DMA hardware behavior.
- Introduced static driver-private DMA TX/RX buffers whose lifetime extends beyond the asynchronous start function.
- Implemented `ADXL355_StartReadDMA()` as the asynchronous start half of an acceleration read.
- Kept chip select asserted after starting the DMA transfer because the transaction continues after the function returns.

## Session 24 — Completing the DMA acquisition path

- Introduced `HAL_SPI_TxRxCpltCallback()` as the CPU callback executed when a SPI DMA transfer completes.
- Added a driver-level completion function to keep the private DMA RX buffer encapsulated inside `adxl355.c`.
- Decoded the received DMA bytes into raw X, Y and Z acceleration values after transfer completion.
- Released the ADXL355 chip select only after the asynchronous SPI transaction had actually finished.
- Clarified why short, bounded work such as axis decoding and buffer writes can reasonably run inside the completion callback, while blocking I/O and heavy signal processing should not.
- Clarified the role of `NULL` pointer checks and why `HAL_StatusTypeDef` is not necessarily useful for a completion function that cannot currently fail.
- Moved sample storage and buffer index advancement into the DMA completion path.
- Clarified that `main()` must process a completed buffer, not the currently active acquisition buffer.
- Introduced the need for a separate ready-buffer indication so the DMA callback can hand a full buffer to `main()` while acquisition continues in the other buffer.

## Session 25 — Continuous DMA double buffering

- Connected the ADXL355 DRDY interrupt to asynchronous SPI DMA acquisition.
- Validated continuous alternating acquisition between two 512-sample buffers.
- Introduced `ready_buffer` as the handoff mechanism between interrupt-driven acquisition and processing in `main()`.
- Clarified the distinction between the active acquisition buffer and the completed buffer being processed by `main()`.
- Used a local buffer pointer in `main()` as a snapshot of the volatile shared `ready_buffer`.
- Clarified why `ready_buffer` remains `volatile` even though `main()` also modifies it.
- Added detection of the case where a buffer becomes full while the previous ready buffer has not yet been released.
- Validated repeated A/B buffer handoffs with zero observed buffer overruns and zero observed SPI start failures during the final run.
- Identified two points to resolve next: the overrun recovery policy and the distinction between a DMA start failure and an SPI/DMA transfer error.

## Session 26 — DMA failures and buffer overrun policy

- Clarified the lifecycle of an asynchronous SPI DMA transaction:
  - the return value of `HAL_SPI_TransmitReceive_DMA()` reports whether the transfer could be started;
  - completion and error callbacks report what happened after a successfully started transfer.
- Introduced a dedicated `dma_start_fail_count` to track failed acquisition starts separately from SPI transfer errors.
- Analyzed the double-buffer overrun case: one buffer is still being processed while the second buffer becomes full, leaving no free acquisition buffer.
- Compared three possible overrun policies: stop acquisition, discard the newly completed buffer, or sacrifice the older buffer currently being processed.
