# Sampling Basics

## Sampling frequency

A real vibration signal is continuous in time.

A digital system cannot observe every instant, so it measures the signal at
discrete time intervals.

The sampling frequency is written as:

```text
Fs = number of samples per second
```

For example:

```text
Fs = 2000 Hz
```

means:

```text
2000 samples / second
1 sample every 0.5 ms
```

## Nyquist frequency

To represent a signal containing frequencies up to `f_max`, the sampling
frequency must satisfy:

```text
Fs > 2 × f_max
```

The frequency:

```text
Fs / 2
```

is called the Nyquist frequency.

For example:

```text
Fs = 2000 Hz
Nyquist frequency = 1000 Hz
```

This is a theoretical limit. In practice, additional margin is desirable.

## Samples per vibration cycle

If a vibration occurs at frequency `f`, the approximate number of samples per
cycle is:

```text
samples per cycle = Fs / f
```

Example:

```text
Fs = 2000 Hz
f = 500 Hz

samples per cycle = 4
```

Lower-frequency vibration components therefore receive more samples per cycle.

## Aliasing

If the signal contains frequencies above the Nyquist frequency, they may appear
as incorrect lower frequencies in the sampled data.

This effect is called aliasing.

To reduce aliasing, the acquisition chain should combine:

- a suitable sampling frequency;
- a limited useful bandwidth;
- filtering before or during sampling.

## Initial V1 target

The first vibration-acquisition target is:

```text
Useful vibration bandwidth: approximately 0–500 Hz
Sampling frequency:         2 kHz
```

This keeps the acquisition rate manageable while leaving margin above the
minimum Nyquist requirement.

The target may be revised later as the physical test bench and sensor
characteristics become better defined.

## Sensor terminology

`ODR` means Output Data Rate.

For a digital accelerometer, ODR is the rate at which the sensor produces new
acceleration samples.

The sensor ODR must therefore be compatible with the sampling strategy used by
the STM32.