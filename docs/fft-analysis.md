# FFT Analysis

## FFT processing pipeline

The frequency-analysis pipeline currently implemented on the X axis is:

```text
512 raw ADXL355 samples
        ↓
convert to g
        ↓
remove buffer mean
        ↓
apply Hann window
        ↓
512-point real FFT
        ↓
magnitude spectrum
        ↓
amplitude normalization
        ↓
dominant-frequency and noise-floor analysis
```

The acquisition buffer itself is not modified. FFT processing uses a separate working buffer.

## FFT size and frequency bins

The FFT input contains:

```text
N = 512 time-domain samples
```

With the configured nominal sampling frequency:

```text
Fs = 2000 Hz
```

the frequency-bin spacing is:

```text
Δf = Fs / N
   = 2000 / 512
   = 3.90625 Hz
```

For a real-valued signal, bins `0..256` contain all unique frequency information:

```text
bin 0   →    0 Hz
bin 1   →    3.90625 Hz
...
bin 128 →  500 Hz
...
bin 256 → 1000 Hz
```

This gives `257` unique bins.

Bin `256` corresponds to the Nyquist frequency:

```text
Fs / 2 = 1000 Hz
```

The other half of the full DFT spectrum contains the corresponding negative-frequency information and is redundant for this analysis.

## Configured analysis band

The ADXL355 is configured with:

```text
ODR = 2000 Hz
LPF ≈ 500 Hz
```

Although the FFT can mathematically represent frequencies up to `1000 Hz`, the project initially analyzes:

```text
bins 1..128
≈ 3.9..500 Hz
```

This `0–500 Hz` range is an application design choice based on the configured sensor bandwidth. It is not a fundamental FFT limitation and does not imply that the monitored machine cannot contain useful vibration components above `500 Hz`.

The analysis-band assumption must therefore be validated on the physical test bench.

If significant vibration information is found above this range, the sensor ODR, LPF and analysis band must be reconsidered together.

## Mean removal and DC

FFT bin `0` represents the DC component of the analyzed buffer.

Before applying the FFT, the mean of the buffer is removed:

```text
centered_sample = sample - buffer_mean
```

This suppresses the constant component of that buffer from the frequency analysis.

Mean removal does not imply that gravity is always constant. Any gravity change, motion or other low-frequency variation occurring inside the buffer can still appear in the spectrum.

## Hann window

The FFT analyzes only a finite section of the signal.

If that section does not contain an integer number of periods, its implicit repetition introduces discontinuities at the buffer boundaries. Energy from a frequency component can then spread into neighboring FFT bins. This is spectral leakage.

A Hann window is therefore applied:

```text
windowed[n] = centered[n] × hann[n]
```

The Hann window gradually reduces the samples toward zero at both ends of the buffer, reducing spectral leakage.

Windowing intentionally modifies the analyzed signal:

- it reduces leakage away from spectral peaks;
- it widens their main lobes;
- it attenuates their measured amplitude.

The original acquisition data remains unchanged.

## Coherent gain

Because the Hann window attenuates sinusoidal amplitudes, its coherent gain is calculated as:

```text
CG = sum(hann[n]) / N
```

For a 512-point Hann window:

```text
CG ≈ 0.499
```

This value is used when converting FFT magnitudes into approximate physical amplitudes.

## FFT magnitude

Each FFT bin represents a complex coefficient:

```text
X[k] = Re[k] + j Im[k]
```

Its magnitude is:

```text
|X[k]| = sqrt(Re[k]² + Im[k]²)
```

Here, `X[k]` refers to the FFT coefficient at frequency bin `k`, not to the X axis of the accelerometer.

For ordinary positive-frequency bins, the normalized sinusoidal amplitude is approximated by:

```text
amplitude_g =
    2 × raw_fft_magnitude
    / (N × CG)
```

The factor `2` compensates for the positive- and negative-frequency components of a real-valued signal.

DC and Nyquist are special cases and are not doubled.

For a frequency that does not fall exactly on an FFT bin, the Hann window spreads part of its energy into neighboring bins. The magnitude of the single largest bin may therefore underestimate the total sinusoidal amplitude.

## Dominant frequency

The dominant bin is the bin with the largest normalized magnitude inside the configured analysis band.

Its frequency is obtained from:

```text
frequency = bin × Fs / N
```

A dominant bin does not by itself prove that a meaningful vibration exists.

Even random sensor noise has a largest bin.

## Noise floor

A noise floor is used to estimate the typical background spectral magnitude.

The implementation estimates it using the median magnitude of the bins inside the configured analysis band.

The median is preferred to the mean because isolated spectral peaks affect it much less.

The dominant peak and the noise floor must be evaluated over the same frequency band.

## Peak-to-noise ratio

A simple diagnostic metric compares the dominant spectral peak with the estimated noise floor:

```text
peak_ratio =
    dominant_peak_magnitude
    / noise_floor
```

A larger ratio means that the strongest spectral component stands further above the typical spectral background.

The peak ratio is a diagnostic feature. By itself, it is not yet a validated machine-health or anomaly-detection criterion.

## Design limitation

The configured `0–500 Hz` analysis band is an initial design assumption.

Its suitability must be validated experimentally using the physical rotating-equipment test bench.

If relevant vibration components exist outside this range, the correct response is not simply to inspect higher FFT bins. The sensor bandwidth, sampling rate and anti-alias filtering must first be configured appropriately for the desired measurement band.
