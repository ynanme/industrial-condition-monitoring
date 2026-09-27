# STM32 Interrupt Flow

This document summarizes the interrupt path observed while configuring TIM2
to generate a periodic event every 500 ms.

## Flow

TIM2 counts from 0 to ARR.

When the period expires:

```text
TIM2 counter reaches ARR
        |
        v
Update event
        |
        v
UIF flag is set
        |
        v
Update interrupt enabled (UIE)
        |
        v
TIM2 IRQ
        |
        v
NVIC
        |
        v
Vector table
        |
        v
TIM2_IRQHandler()
        |
        v
HAL_TIM_IRQHandler()
        |
        v
HAL_TIM_PeriodElapsedCallback()
        |
        v
Application code
```

## Important distinction

Three different conditions are involved:

1. The hardware event occurs and sets the update flag.
2. TIM2 is configured to generate an interrupt for that event.
3. The NVIC allows the TIM2 interrupt to reach the CPU.

The Cortex-M uses the interrupt vector table to find the address of
`TIM2_IRQHandler()`.

The HAL handler checks and clears the timer flag before invoking
`HAL_TIM_PeriodElapsedCallback()`, which is overridden by the application.