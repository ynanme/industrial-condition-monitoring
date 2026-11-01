# Sampling and Frequency Analysis Basics

## Sampling frequency

The sampling frequency, noted `Fs`, is the number of measurements acquired per second.

For the current ADXL355 configuration:

```text
Fs ≈ 2000 Hz
```

This means that a new acceleration sample is produced approximately every:

```text
Ts = 1 / Fs
   = 1 / 2000
   = 0.5 ms
```

`Fs` describes how fast the signal is measured. It is different from the frequency of the physical vibration itself.

For example, a vibration at `100 Hz` sampled at `2000 Hz` is represented by:

```text
2000 / 100 = 20 samples per period
```

In general:

```text
samples_per_period = Fs / f
```

where `f` is the physical signal frequency.

---

## Nyquist frequency

A sampled signal cannot represent arbitrarily high frequencies without ambiguity.

The Nyquist frequency is:

```text
f_Nyquist = Fs / 2
```

With:

```text
Fs = 2000 Hz
```

the Nyquist frequency is:

```text
f_Nyquist = 1000 Hz
```

A signal at `1000 Hz` corresponds to exactly two samples per period.

Frequencies above Nyquist cannot be uniquely distinguished from lower frequencies after sampling.

---

## Aliasing

Aliasing occurs when a signal contains frequencies above the Nyquist frequency.

Example with:

```text
Fs = 10 Hz
f_Nyquist = 5 Hz
```

A `4 Hz` cosine and a `6 Hz` cosine can produce exactly the same sampled values.

Even though the physical signals are different:

```text
4 Hz → 4 oscillations per second
6 Hz → 6 oscillations per second
```

the sampling system only observes values every:

```text
1 / 10 s = 0.1 s
```

The information describing what happened between two samples is lost.

Therefore, after sampling, the system cannot determine whether the original signal was `4 Hz` or `6 Hz`.

The `6 Hz` signal is said to alias to `4 Hz`.

More generally, with `Fs = 10 Hz`:

```text
6 Hz → appears as 4 Hz
7 Hz → appears as 3 Hz
8 Hz → appears as 2 Hz
9 Hz → appears as 1 Hz
```

Aliasing happens during sampling. A later FFT cannot recover the original frequency because that information has already been lost.

---

## Relation to the ADXL355 configuration

The current configuration uses approximately:

```text
ODR = 2000 Hz
LPF = 500 Hz
```

The low-pass filter limits the useful vibration content to approximately `500 Hz`.

This is comfortably below the Nyquist limit:

```text
useful band ≈ 0–500 Hz
Nyquist     ≈ 1000 Hz
```

This reduces the risk of high-frequency content aliasing into the useful vibration band.

---

## Time domain and frequency domain

The current processing operates in the time domain.

A buffer contains:

```text
x[0], x[1], ..., x[511]
```

Each value represents acceleration measured at a particular instant.

Features already extracted from the time-domain signal include:

```text
mean
standard deviation
peak-to-peak
```

These features describe the amplitude and variability of the vibration, but they do not directly identify which frequencies are responsible for it.

Two signals may have similar standard deviation and peak-to-peak values while vibrating at very different frequencies.

Frequency analysis is therefore useful for condition monitoring.

---

## DFT and FFT

The DFT, or Discrete Fourier Transform, converts a finite sequence of time-domain samples into frequency-domain components.

Conceptually, it asks:

```text
How much of each measurable frequency is present in this buffer?
```

The FFT, or Fast Fourier Transform, is an efficient algorithm used to compute the DFT.

The FFT does not acquire new information. It analyzes the same samples from a different perspective.

```text
time domain
x[0], x[1], ..., x[N-1]

        ↓ FFT

frequency domain
magnitude for each frequency bin
```

---

## Frequency bins

The FFT result is divided into discrete frequency positions called bins.

For:

```text
Fs = sampling frequency
N  = number of samples
```

the spacing between bins is:

```text
Δf = Fs / N
```

For the current acquisition:

```text
Fs = 2000 Hz
N  = 512
```

therefore:

```text
Δf = 2000 / 512
   = 3.90625 Hz
```

The bins correspond to:

```text
bin 0   →   0 Hz
bin 1   →   3.90625 Hz
bin 2   →   7.81250 Hz
bin 3   →  11.71875 Hz
...
bin 128 → 500 Hz
...
bin 256 → 1000 Hz
```

The frequency represented by bin `k` is:

```text
f_k = k × Fs / N
```

Conversely, the approximate bin corresponding to a frequency `f` is:

```text
k = f × N / Fs
```

Example:

```text
f = 500 Hz
Fs = 2000 Hz
N = 512
```

gives:

```text
k = 500 × 512 / 2000
  = 128
```

Therefore, a pure `500 Hz` component aligned with the FFT grid appears at bin `128`.

---

## Frequency resolution

The current FFT resolution is approximately:

```text
3.906 Hz
```

This does not mean that signals between two bins are invisible.

For example, a `1.5 Hz` signal exists physically, but the current FFT has no bin exactly at `1.5 Hz`.

The buffer duration is:

```text
512 / 2000 = 0.256 s
```

A `1.5 Hz` signal has a period of approximately:

```text
1 / 1.5 ≈ 0.667 s
```

Therefore, only about:

```text
0.256 / 0.667 ≈ 0.38 period
```

is observed inside one buffer.

This makes accurate identification of such a low frequency difficult with the current window length.

Increasing `N` while keeping the same `Fs` increases the observation duration and improves frequency resolution.

For example:

```text
N = 4096
Fs = 2000 Hz

Δf = 2000 / 4096
   ≈ 0.488 Hz
```

---

## Current condition-monitoring pipeline

The current signal-processing path is:

```text
ADXL355
    ↓
DRDY
    ↓
SPI + DMA
    ↓
512-sample X/Y/Z buffers
    ↓
time-domain features
    ├── mean
    ├── standard deviation
    └── peak-to-peak
    ↓
frequency-domain processing
    └── FFT
```

The next concepts to study are:

```text
FFT magnitude
spectral leakage
frequency components that fall between bins
windowing
dominant frequency
spectral peaks
```

In particular, a pure signal does not always produce one non-zero FFT bin with every other bin equal to zero. When a signal frequency does not align exactly with an FFT bin, its energy can spread across several neighboring bins. This phenomenon is called spectral leakage.