# Sampling and Filtering

## What the accelerometer measures

The accelerometer does not directly return vibration frequencies.

At each sampling instant, it returns an acceleration value.

For one axis:

```text
t0 → ax = ... g
t1 → ax = ... g
t2 → ax = ... g
...
```

With an output data rate of 2 kHz:

```text
ODR = 2000 Hz
```

the sensor provides 2000 acceleration samples per second.

Each sample is an acceleration value, not a frequency.

## Acceleration and frequency are different properties

Acceleration describes the instantaneous magnitude of motion and can be
expressed in `g` or `m/s²`.

Frequency describes how quickly a pattern in the acceleration signal repeats
over time and is expressed in hertz.

For example, if the acceleration waveform repeats 100 times per second, the
signal contains a 100 Hz component.

A single sample cannot have a vibration frequency by itself. Frequency is
determined from the evolution of multiple samples over time.

## Why filtering is possible without explicitly calculating frequency

A low-pass filter does not need to calculate a frequency value such as
`100 Hz` or `1500 Hz`.

It operates directly on successive acceleration samples.

Rapid changes between samples correspond to high-frequency content, while slow
changes correspond to lower-frequency content.

A filter uses the relationship between current and previous values to attenuate
rapid variations.

Conceptually:

```text
raw acceleration samples
        ↓
time-domain filtering
        ↓
filtered acceleration samples
```

The filter therefore has a frequency response even though it does not need to
run a frequency analysis such as an FFT.

## Filtering does not reduce the number of output samples

For the initial ADXL355 configuration:

```text
ODR = 2000 Hz
LPF = 500 Hz
```

the sensor still provides:

```text
2000 filtered acceleration samples per second
```

The LPF does not select 500 samples from the 2000 samples.

Instead, it modifies the values so that vibration components above the useful
band are progressively attenuated.

## Why filtering is useful

Sampling at a finite rate limits the frequencies that can be represented
correctly.

With:

```text
Fs = 2000 Hz
```

the Nyquist frequency is:

```text
Fs / 2 = 1000 Hz
```

High-frequency content can otherwise produce aliasing and appear as incorrect
lower-frequency content.

Filtering before the final sampled data is exposed therefore reduces unwanted
high-frequency content before it can pollute the measurements.

## Initial project configuration

The initial vibration acquisition target is:

```text
ODR: approximately 2000 samples/s
Useful vibration band: approximately 0–500 Hz
LPF cutoff: approximately 500 Hz
```

The resulting data remains a sequence of acceleration measurements in `g`.

Frequency-domain information will later be derived from blocks of these
samples, for example using an FFT.
