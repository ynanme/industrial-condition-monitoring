# STM32 Timer Basics

## Periodic timer

For TIM2, the timer clock is 84 MHz.

The update-event frequency is:

f_event = f_timer / ((PSC + 1) × (ARR + 1))

where:

- PSC: prescaler
- ARR: auto-reload register

For a 500 ms period:

f_event = 2 Hz

A convenient configuration is:

- PSC = 8399
- ARR = 4999

This gives:

84 MHz / 8400 = 10 kHz

One timer tick therefore lasts 0.1 ms.

5000 ticks × 0.1 ms = 500 ms.

TIM2 can then generate an interrupt every 500 ms without relying on
`HAL_Delay()` or polling in the main loop.