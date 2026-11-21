# FM broadcast demodulator and stereo decoder DSP design (validation of radio/src/fmreceiver.cpp)

Method note: I fetched the primary source files of GNU Radio, SDR++, SDRangel, NGSoftFM and rtl_fm. I also computed the de-emphasis error and stereo-separation figures with a short numpy script (scratch only; formulas are given so they can be re-derived). The computed numbers are marked "(computed)" in the Inferences. I could not get the TEF668x user manual text: manuals.plus returned 403 and the GitHub driver path returned 404. I did not fetch ITU-R BS.450/BS.412, Redsea, or Lyons, so the numbers that depend on them are listed under Gaps.

## De-emphasis: best digital implementation of 75 µs / 50 µs at 48–57 kHz

### Takeaway
Godwit's impulse-invariant one-pole filter is the same design SDR++ and rtl_fm use (they use the forward-Euler variant). Its error measured in the project, +0.44 dB at 10 kHz and +1.01 dB at 15 kHz, is exactly what theory predicts at 57 kHz. GNU Radio uses a pre-warped bilinear transform instead, and at audio rates that is worse: about −0.9 dB at 10 kHz and −2.3 dB at 15 kHz. The cheapest accurate fix keeps the impulse-invariant pole and adds one real zero near z ≈ −0.12. That brings the error to about 0.03 dB over 20 Hz–15 kHz at 57 kHz. A second option is to run the existing one-pole filter at 228 kHz on the MPX-derived L/R paths, which gives at most +0.06 dB at 15 kHz.

### Cited Findings
- **GNU Radio `fm_deemph`** pre-warps the corner and then uses the bilinear transform. It sets `w_c = 1/tau` and `w_ca = 2·fs·tan(w_c/(2·fs))`, with `k = −w_ca/(2·fs)`, `p1 = (1+k)/(1−k)` and `b0 = −k/(1−k)`. The taps are `b = [b0, b0]` and `a = [1, −p1]`, which puts the zero at z = −1 (Nyquist). The default tau is 75 µs; the docstring says 75 µs applies in the US/KR and 50 µs elsewhere. The code gives no numerical accuracy analysis. — [gnuradio fm_emph.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/fm_emph.py)
- **GNU Radio `wfm_rcv_pll`** applies `fm_deemph(fs=audio_rate, tau)` separately to L and R, after matrixing and decimation. — [gnuradio wfm_rcv_pll.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/wfm_rcv_pll.py)
- **SDR++** uses a one-pole IIR with `alpha = dt/(tau + dt)`, `dt = 1/samplerate`, and `out[i] = alpha·in[i] + (1−alpha)·out[i−1]`. This is the backward-Euler form; its pole is 1/(1+dt/tau) rather than exp(−dt/tau). — [SDR++ core/src/dsp/filter/deephasis.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/filter/deephasis.h)
- **rtl_fm** computes `deemph_a = round(1/(1 − exp(−1/(rate_out·75e-6))))`, which is the impulse-invariant alpha stored as a rounded integer reciprocal, and runs `avg += (d + a/2)/a` in integer arithmetic on a static state. Tau is hard-coded to 75 µs. The wbfm preset sets rate_in = rate_out = 170 kHz with a 32 kHz final output and enables de-emphasis. The code comment calls this "cheating". — [rtl-sdr src/rtl_fm.c](https://github.com/osmocom/rtl-sdr/blob/master/src/rtl_fm.c)
- **NGSoftFM** uses a `LowPassFilterRC` with time constant `deemphasis·sample_rate_pcm·1e-6`, so in samples at the PCM rate. It applies it after the PCM resampler for both mono and stereo; 0 disables it. — [NGSoftFM sfmbase/FmDecode.cpp](https://github.com/f4exb/ngsoftfm/blob/master/sfmbase/FmDecode.cpp)
- **SDRangel BFM** de-emphasises after stereo matrixing at the audio rate, using `getDeEmphasisTimeConstant()·sampleRate`. The filter type lives in another class and was not inspected. — [SDRangel bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)

### Inferences
- **(computed) Impulse-invariant / matched-z one-pole error.** The filter is `p = exp(−1/(fs·tau))`, `b = [1−p]`, `a = [1, −p]`; for a single real pole, matched-z is the same filter. Error versus the analogue 1/(1+jωτ), in dB:

  | fs | 1 kHz | 5 kHz | 10 kHz | 15 kHz |
  |---|---|---|---|---|
  | 57 kHz | 0.00 | +0.11 | +0.44 | +1.01 |
  | 48 kHz | +0.01 | +0.15 | +0.63 | +1.44 |
  | 228 kHz | 0.00 | +0.01 | +0.03 | +0.06 |

  The 75 µs and 50 µs errors are essentially identical. The 57 kHz row matches Godwit's measured figures exactly, so the implementation is correct and the error is inherent to the method: aliasing of the −6 dB/oct skirt lifts the response near Nyquist.
- **(computed) GNU Radio pre-warped bilinear error.** The corner is exact, but the zero at Nyquist pulls the top octave down. At 57 kHz the error is −0.16, −0.87 and −2.30 dB at 5, 10 and 15 kHz. At 48 kHz it is −0.22, −1.28 and −3.56 dB. At 228 kHz it is −0.05 and −0.12 dB at 10 and 15 kHz. Pre-warping at a low audio rate is therefore worse in magnitude than impulse invariance, and the error has the opposite sign: it gives a dull top end.
- **(computed) Recommended fix: one-pole, one-zero filter.** Keep the impulse-invariant pole `p = exp(−1/(fs·τ))` and add a real zero `q`:
  - `H(z) = g·(1 − q·z⁻¹)/(1 − p·z⁻¹)`, with `g = (1−p)/(1−q)` for unity DC gain.
  - A least-squares/minimax fit over 20 Hz–15 kHz gives:
    - τ = 75 µs, fs = 57 kHz: p = 0.79143, q = −0.1166, max error 0.027 dB. Jointly optimised: p = 0.79093, q = −0.1210, max error 0.020 dB.
    - τ = 50 µs, fs = 57 kHz: p = 0.70407, q = −0.1162, max error 0.027 dB.
    - fs = 48 kHz: q ≈ −0.124 for both tau values, max error 0.058 dB.
  - The cost is one extra multiply-add per sample per channel. This is a simplified form of the "magnitude-matched" first-order designs, such as Vicanek's matched filters; that reference was not fetched.
- **(computed) De-emphasis at 228 kHz.** The plain impulse-invariant one-pole has at most +0.06 dB error at 15 kHz. It costs 4× the multiplies (2 channels × 228k × 2 MAC ≈ 1 M MAC/s, which is negligible on an A76).
  - In Godwit, L+R and L−R are separate signals until matrixing. Because de-emphasis is linear, it can be applied to L+R and L−R before matrixing with an identical filter on both. Both paths then see the same phase, so separation is preserved. This must happen before the ×4 decimation, which in practice means filtering at 228 kHz before the 189-tap decimator.
  - Do not de-emphasise the composite MPX before stereo demodulation. That would attenuate the 23–53 kHz subcarrier band.
- **Recommendation.** Use the one-pole/one-zero filter at the final audio rate; it is cheapest and has under 0.03 dB error. Make tau selectable (75/50 µs). Apply identical filters to L and R, or to M and S.

### Gaps
- I did not fetch a primary paper with published error tables for FM de-emphasis discretisation (e.g., Vicanek "Matched One-Pole Digital Shelving Filters"). The figures above are my own computation.
- SDRangel's de-emphasis class (`LowPassFilterRC` or similar) was not inspected.

## Channel filtering: 114 kHz cutoff, ~28 kHz transition, and adaptive IF bandwidth

### Takeaway
A ±114 kHz cutoff is a reasonable "wide" setting for 200 kHz spacing, since adjacent channels start about 100 kHz from the band edge. With an RTL-SDR's limited dynamic range, though, a fixed filter cannot cover both strong-adjacent-channel cases and clean cases. Car tuners such as NXP's TEF68xx use automatic, condition-driven bandwidth: wide when the channel is clean, narrowed when adjacent-channel energy, multipath or a weak signal is detected. Among the open-source receivers, SDRangel exposes the RF bandwidth as a user setting; I found no adaptive bandwidth in any of the open-source references.

### Cited Findings
- **NXP TEF6862** (car-radio predecessor): in automatic mode the bandwidth "depends on the signal conditions". The algorithm detects adjacent-channel interference, the deviation of the wanted signal, detuning, multipath and signal strength, and its settings are adjustable over I²C. — [NXP TEF6862HL product page](https://www.nxp.com/products/TEF6862HL)
- **TEF668x** has digital IF processing with "variable IF bandwidth filtering (PACS)". — [NXP TEF668X product page](https://www.nxp.com/products/radio/low-if-tuner-high-performance-one-chip:TEF668X); [pu2clr/tef668x README](https://github.com/pu2clr/tef668x)
- **TEF668x API manual (Rev 1.6, V102)** lists "FM cmd 10 Set_Bandwidth" and "FM cmd 86 Set_Bandwidth_Options". I saw only the table of contents. — [User Manual TEF6686 (usermanual.wiki)](https://usermanual.wiki/Document/UserManualTEF6686.297261401/html)
- **A patent on adjacent-channel rejection** describes switching in progressively narrower predetermined IF filters when adjacent-channel interference is detected. — [EP0629053A1](https://patents.google.com/patent/EP0629053A1/en); see also [US 8135370 "FM tuner having adaptive bandpass filter for adjacent channel interference"](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/8135370)
- **SDRangel BFM** uses an FFT filter with cutoffs ±rfBandwidth/2, set by the user. It initialises to ±50 kHz at 384 kHz and then takes the settings value. — [SDRangel bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)
- **NGSoftFM** uses a `LowPassFilterFirIQ(10, bandwidth_if/sample_rate_if)` IF filter, which is a short FIR (order parameter 10), followed by a baseband downsampler with 8×downsample taps and cutoff 0.4/downsample. — [NGSoftFM FmDecode.cpp](https://github.com/f4exb/ngsoftfm/blob/master/sfmbase/FmDecode.cpp)

### Inferences
- **Bandwidth needed.** With 75 kHz deviation and 53 kHz top modulating frequency (60 kHz with RDS at 57 kHz ± ~2.4 kHz), Carson's rule gives 2(75+53) = 256 kHz, or 2(75+60) ≈ 270 kHz. This is standard textbook material; I did not fetch a source.
  - A ±114 kHz (228 kHz) passband with a 28 kHz transition is about −3 to −6 dB near ±114 kHz and reaches its stopband around ±128–142 kHz. This truncates the outer Bessel sidebands at full deviation.
  - Peak deviation occurs rarely, and most sideband power lies inside ±(Δf + fm), so the distortion is small. The truncation shows up mainly as envelope ripple (AM), which matters for the SNR estimator (next section), and as some L−R/RDS degradation on heavily processed stations.
- **Adjacent-channel geometry.** The adjacent carrier at ±200 kHz has its own energy spread over roughly 100–300 kHz from our centre. A ±114 kHz filter with stopband from about 140 kHz therefore admits the adjacent signal's inner skirts (100–140 kHz) only partially. Rejection of a strong +200 kHz neighbour is limited mainly by the transition band.
  - Suggested adaptive scheme, modelled on the TEF approach: provide 3–5 selectable FIR sets, for example ±114 (wide), ±90, ±70, ±50 kHz one-sided. Switch narrower when the measured power in 100–150 kHz offset "guard" bins relative to in-channel power exceeds a threshold, or on low RSSI. Widen with hysteresis of a few hundred ms.
  - The cost of narrowing is more THD and loss of stereo separation and RDS. That is why tuners pair narrowing with stereo blend.
- **Compute.** A 135-tap complex FIR at 1.14 MS/s decimating by 5 costs 135 × 2 × 228k ≈ 62 M MAC/s with a polyphase implementation, which is fine on an A76. Swapping coefficient sets costs nothing extra.

### Gaps
- I could not verify the TEF668x bandwidth range (the commonly quoted "56–311 kHz") or its default automatic-mode thresholds; the manual was inaccessible (403). Treat 56–311 kHz as unverified.
- I found no quantitative distortion-versus-bandwidth curve from a primary source.

## Stereo decoder: pilot PLL, phase accuracy, delay matching, 38 kHz regeneration

### Takeaway
The phase error between the regenerated 38 kHz carrier and the transmitted subcarrier sets separation by the formula separation = 20·log10(cot(φ/2)). That gives about 41 dB at 1°, 35 dB at 2° and 27 dB at 5°, where φ is the error at 38 kHz (twice the 19 kHz error). A Q=95 biquad pilot bandpass has a steep phase slope at 19 kHz: about ±0.6° of phase per Hz of pilot offset. Any frequency mismatch between the biquad centre and the actual pilot, or any un-modelled delay, therefore translates directly into lost separation. The reference designs avoid this in one of two ways:
- SDR++ and GNU Radio use a linear-phase FIR pilot filter and delay the audio paths by its group delay.
- NGSoftFM and SDRangel feed the PLL directly, so the PLL's own loop filter does the narrowing and there is no bandpass phase to compensate.

### Cited Findings
- **GNU Radio `wfm_rcv_pll`**
  - Pilot filter: complex bandpass FIR, 18.98–19.02 kHz, 1.5 kHz transition, Hamming.
  - PLL: `pll_refout_cc(0.001, 2π·19200/fs, 2π·18800/fs)`.
  - 38 kHz: obtained by squaring the PLL reference (`multiply_cc` of the PLL output with itself), followed by a 37.6–38.4 kHz bandpass with gain −2.
  - Delay compensation: the composite is delayed by `(len(pilot_taps)−1)//2 + (len(stereo_taps)−1)//2` before the stereo mixer and the mono low-pass.
  - Audio filters: 15 kHz low-pass with 1.5 kHz transition for both M and S.
  - Source: [gnuradio wfm_rcv_pll.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/wfm_rcv_pll.py)
- **SDR++ broadcast FM**
  - Pilot filter: complex bandpass FIR, 18.75–19.25 kHz with 3 kHz transition.
  - PLL: `init(…, 25000/fs, 0, 19 kHz, 18.75 kHz, 19.25 kHz)`.
  - 38 kHz: the conjugated PLL output multiplies the L−R signal twice, which is equivalent to squaring.
  - Delay compensation: the L+R and L−R paths are both delayed by `(pilotFirTaps.size−1)/2 + 1` samples.
  - Matrixing and audio: L−R is scaled ×2, then L = M + S and R = M − S. An optional audio low-pass at 15 kHz with 4 kHz transition follows.
  - RDS: shifted by −57 kHz and resampled to 5 kHz.
  - Source: [SDR++ core/src/dsp/demod/broadcast_fm.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/demod/broadcast_fm.h)
- **NGSoftFM pilot PLL**
  - Loop bandwidth is 50 Hz, with no bandpass in front.
  - Loop constants: `p1 = exp(−1.146·bw·2π)`, `p2 = exp(−5.331·bw·2π)`, `q1 = exp(−0.1153·bw·2π)`, `b0 = 0.62·bw·2π`.
  - Phase detector: Q/I, saturated beyond ±45°.
  - Lock detector: the delay is `20/bw` (≈0.4 s), and the minimum pilot level is 0.01, previously 0.04.
  - 38 kHz: `2·sin·cos`, i.e. sin(2θ).
  - Stereo product: `rawstereo *= 1.17·baseband`.
  - Matrixing and filtering: L = m + s, R = m − s. The mono and stereo paths use identical resamplers, with no explicit delay line, plus a 30 Hz DC blocker.
  - Source: [NGSoftFM sfmbase/FmDecode.cpp](https://github.com/f4exb/ngsoftfm/blob/master/sfmbase/FmDecode.cpp)
- **SDRangel BFM** uses a pilot PLL with parameters `(19000/fs, 50/fs, 0.01)`, i.e. a 50 Hz loop. Stereo uses demod × 1.17 × PLL[1], or an LSB (quadrature) variant. The third harmonic of the PLL phase is used for 57 kHz RDS. No explicit delay compensation was seen, because mono and stereo use matched 16-tap interpolators. — [SDRangel bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)
- **Patent literature** states that a constant phase error between the local and the suppressed subcarrier produces crosstalk that limits the maximum attainable separation, and that amplitude correction cannot fix it. One FMX-related patent cites ~10° typical PLL phase shift and says "2 degrees… deterioration by over 10 dB" for that system. — [US 4860354 "Arrangement for decoding a stereo multiplex signal"](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/4860354); [US 4852167 "FMX stereophonic receiver"](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/4852167). These are system-specific and should not be taken as a general rule.

### Inferences
- **(computed) Separation versus 38 kHz carrier phase error φ.** Separation = 20·log10((1+cosφ)/(1−cosφ))/2 = 20·log10(cot(φ/2)) ≈ 20·log10(2/φ):

  | φ | 0.5° | 1° | 2° | 5° | 10° |
  |---|---|---|---|---|---|
  | Separation | 47.2 dB | 41.2 dB | 35.2 dB | 27.2 dB | 21.2 dB |

  A 19 kHz phase error doubles at 38 kHz. A 1° error at the pilot therefore means 2° at 38 kHz, which limits separation to 35 dB.
- **(computed) Separation versus S/M gain mismatch g.** Separation = 20·log10((1+g)/(1−g)):

  | g | 0.99 | 0.98 | 0.95 | 0.90 |
  |---|---|---|---|---|
  | Separation | 46 dB | 40 dB | 32 dB | 26 dB |

  The ×2 (or 1.17×-type) S gain must be correct, and the M and S low-pass filters must have identical passband ripple; using the same taps for both guarantees this.
- **(computed) Separation versus time skew between the M and S paths.** The effective phase error is 2π·f·Δt:

  | Skew | 1 kHz | 15 kHz |
  |---|---|---|
  | 1 µs | 50 dB | 26.5 dB |
  | 4.4 µs (one sample at 228 kHz) | 37 dB | 13.5 dB |

  Godwit filters M and S with the same 189-tap FIR after the 38 kHz mixer, at the same point in the chain, so the paths are inherently aligned. Check that the code does not add a separate delay in only one path.
- **Biquad pilot bandpass (Q=95).**
  - The −3 dB bandwidth is 19000/95 = 200 Hz. The phase slope at centre is dφ/df = −2Q/f0 rad/Hz = −0.01 rad/Hz ≈ −0.57°/Hz.
  - If the PLL locks to the filtered pilot, the regenerated carrier lags the true pilot by the filter phase at the actual pilot frequency. That phase is 0 only if the pilot sits exactly at the biquad centre.
  - Pilot tolerance is ±2 Hz (a standard fact, not fetched here). Combined with the RTL-SDR's ppm error, which scales the demodulated MPX only negligibly since MPX frequencies come from the transmitter, the residual is mainly the biquad's coefficient accuracy. A ±2 Hz pilot offset gives ±1.1° at 19 kHz, which is ±2.3° at 38 kHz and limits separation to about 34 dB.
  - Fixes:
    1. Feed the PLL the raw MPX, or a wide (±1–3 kHz) linear-phase FIR, and let a 10–50 Hz loop do the selectivity, as NGSoftFM and SDRangel do.
    2. Alternatively, keep the biquad but compute its exact phase at the PLL's measured frequency (the NCO frequency is known) and subtract it from the regenerated phase.
    3. Or use a symmetric FIR bandpass and delay M/S by (N−1)/2, as SDR++ and GNU Radio do.
- **PLL bandwidth.** A ~15 Hz loop is tighter than the 50 Hz used by NGSoftFM and SDRangel. It gives lower phase jitter (better separation on weak signals) but slower acquisition and less tolerance to the ppm-induced pilot shift. That shift is zero for the pilot, though, because the MPX is demodulated, so 15 Hz is fine.
  - Phase jitter standard deviation ≈ 1/√(2·SNR_loop) rad. For φ_rms < 1°, SNR in the loop bandwidth must exceed about 32 dB.
  - The pilot is at 8–10% injection (about 6–7.5 kHz deviation), so on weak signals separation becomes jitter-limited. That is one argument for blend driven by pilot SNR.
- **38 kHz regeneration.** Use double-angle generation from the NCO phase, cos(2θ) or 2·sin·cos, as Godwit, NGSoftFM and SDRangel do. This is exact and cheap. Squaring a complex reference (GNU Radio, SDR++) is equivalent. Also use 3θ for 57 kHz RDS.
- **19 kHz leakage into audio.** The 17 kHz cutoff of the 189-tap Hamming filter at 228 kHz has a transition width of about 3.3·228k/189 ≈ 4 kHz (Hamming main-lobe rule of thumb), so 19 kHz is only partly into the stopband. Hamming stopband is about −53 dB, but at 19 kHz the filter may still be in transition.
  - Recommendation: put the passband edge at 15 kHz and the stopband at 19 kHz, or add a 19 kHz notch, or subtract the PLL-reconstructed pilot from the MPX (pilot cancellation) before the M low-pass.
  - Note that the pilot in L+R is at about −20 dB relative to full modulation; it must be cut by more than 40 dB to stay inaudible and to avoid recorder/resampler aliasing.

### Gaps
- I found no authoritative standard (ITU-R BS.450) figure for required receiver phase accuracy or pilot tolerance. The ±2 Hz pilot tolerance is from general knowledge; the search snippet echoed it, but I did not fetch it from the standard.
- I did not fetch Redsea's demodulator chain.

## Signal-quality estimation from complex baseband

### Takeaway
The envelope-cv estimator measures AM on a constant-envelope signal. Channel-filter truncation of FM sidebands (strong at peak deviation) and multipath both create AM that is unrelated to noise. That explains why strong stations read only 15–20 dB. The robust quantities car tuners use, and that can be measured cheaply here, are:
- RSSI: I/Q power after the channel filter, calibrated with the tuner gain.
- Ultrasonic noise (USN): MPX power in a band above the stereo and RDS content, e.g. 60–100 kHz or 64–76 kHz.
- Multipath (WAM): the envelope-AM measure. It should be used as a multipath indicator, not as SNR.
- Pilot SNR from the PLL.

These should drive blend and soft mute.

### Cited Findings
- **SDRangel BFM squelch** uses the mean of |I+jQ|² after the channel filter against a dB threshold, with an asymmetric attack/decay hysteresis counter: open when the counter exceeds rfBandwidth/20. — [SDRangel bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)
- **NGSoftFM** tracks the IF RMS level (smoothed `0.95·old + 0.05·new`), the baseband mean (a DC offset, i.e. tuning-offset indicator) and the baseband RMS. It uses the pilot I-component level as a lock/stereo-presence detector with a 0.4 s lock delay. — [NGSoftFM FmDecode.cpp](https://github.com/f4exb/ngsoftfm/blob/master/sfmbase/FmDecode.cpp)
- **NXP TEF6862** automatic bandwidth control uses adjacent-channel, deviation, detuning, multipath and signal-strength detectors. These are the same family of quality metrics used for blend and soft mute. — [NXP TEF6862HL](https://www.nxp.com/products/TEF6862HL)

### Inferences
- **Why cv under-reads.**
  - An FM signal through an ideal wide filter has constant envelope. A finite ±114 kHz filter removes sideband energy whenever instantaneous deviation plus modulation frequency approaches the band edge. Heavily processed stations (dense audio plus high-frequency L−R and RDS content) then show several percent envelope ripple.
  - For example, cv = 0.07 gives 20·log10(1/(0.07·√2)) ≈ 17 dB, matching the README's 15–20 dB. This is a bias floor, not noise.
  - Multipath creates large envelope ripple at rates set by the modulation, so cv rises sharply. That makes cv a good multipath detector, comparable to the WAM idea.
- **Ultrasonic noise (recommended main SNR proxy).**
  - With the MPX at 228 kHz, measure the power in a band such as 70–100 kHz (or 60–90 kHz, avoiding RDS at 57 ± 2.4 kHz) and compare it with the pilot power, or with the total deviation reference (75 kHz deviation = 0 dB).
  - The FM discriminator's noise output rises as f² (triangular noise), so the ultrasonic band sees noise long before the audio does. Programme content is absent above 60 kHz.
  - This is the standard way car tuners derive "USN". The TEF manual exposes a USN field; that field name is from memory and was not verified because the manual was inaccessible.
  - Cost: a decimated FIR bandpass or a short FFT every 80 ms, which is negligible.
- **Pilot SNR.** The ratio of pilot power (from the PLL I-arm, averaged) to noise power in the PLL's arm-filter bandwidth is directly what limits separation. It suits blend control: full stereo above about 35–40 dB pilot-band SNR, ramping to mono below it.
- **RSSI.** Use I/Q power after the channel filter in dBFS minus the tuner gain (R820T gain table) to get a dBm estimate. It is useful for soft mute and the seek threshold. It is not SNR when AGC is on, and adjacent-channel power that leaks through the filter inflates it.
- **Guard-band power.** Power at 100–150 kHz offsets before the channel filter (FFT bins, or the pre/post-filter power ratio) detects adjacent channels. This should drive IF narrowing, not blend.
- **Recommended control mapping:**
  - Blend: USN and pilot SNR, with cv/WAM as a multipath override.
  - High-cut: USN.
  - Soft mute: RSSI and USN.
  - IF bandwidth: guard-band ratio.
  - Use attack and release time constants of tens to hundreds of ms with hysteresis.

### Gaps
- I found no primary reference quantifying cv-estimator bias versus filter bandwidth or multipath delay. The explanation above is reasoned, not sourced.
- The TEF668x quality fields (level, USN, WAM, offset, bandwidth, modulation) and the blend thresholds tied to them are unverified (manual inaccessible).

## Audio sample rate: 57 kHz plus PipeWire versus producing 48 kHz directly

### Takeaway
Either approach is acceptable. PipeWire's resampler is high quality but adds CPU and latency in the audio server, and runs every block regardless. The reference receivers all resample internally to the device rate: SDR++, SDRangel, NGSoftFM and GNU Radio. 228k → 48k is 4.75 = 19/4, which needs a 19/4 polyphase resampler.

### Cited Findings
- **SDRangel** uses fractional interpolators (`create(16, channelSampleRate, afBandwidth)`, distance = channelRate/audioRate) to go straight from the channel rate to the audio device rate. — [SDRangel bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)
- **NGSoftFM** resamples baseband to PCM with filter order `baseband_rate/1000` and cutoff bandwidth_pcm/baseband_rate, non-integer ratio. — [NGSoftFM FmDecode.cpp](https://github.com/f4exb/ngsoftfm/blob/master/sfmbase/FmDecode.cpp)
- **rtl_fm** wbfm runs at 170 kHz and produces the final rate via a second downsample to 32 kHz. — [rtl_fm.c](https://github.com/osmocom/rtl-sdr/blob/master/src/rtl_fm.c)

### Inferences
- **Direct 48 kHz.** Use a 19/4 polyphase FIR from 228 kHz (upsample by L=19, downsample by M=4), with 19 phases. Each output sample costs about N_total/19 MACs per channel (M and S each).
  - With a ~380-tap prototype (≈20 taps per phase) at 48 kHz × 2 channels, that is about 2 M MAC/s, which is cheap.
  - The same filter can replace the 189-tap 17 kHz low-pass, so it costs no extra.
  - Benefits: deterministic latency, no double filtering, and de-emphasis at a known rate (48 kHz needs the one-zero fix; its impulse-invariant error is +1.44 dB at 15 kHz).
- **57 kHz plus PipeWire.** This works, and PipeWire's default resampler quality is good. It does add a second anti-alias filter and tens of ms of buffering, and its behaviour depends on PipeWire's quality setting.
  - The 57 kHz output also leaves 19 kHz at 1/3 of fs. Pilot residue that is not removed survives the 57→48 resample: 19 kHz is below 24 kHz, so it is not filtered out.
- **Alternative decimation chain.** Pick the RTL rate so that the chain lands on 48 kHz with integer steps. For example, 1.152 MS/s ÷ 4 = 288 kHz MPX, then ÷ 6 = 48 kHz. 1.152 MS/s is a valid RTL2832U rate (it is in the 900k–3.2M range). This avoids rational resampling entirely, and 288 kHz still holds the 0–60 kHz MPX plus ultrasonic USN measurement up to 144 kHz.

### Gaps
- I did not fetch PipeWire resampler documentation (quality levels, latency). The claims about it are general knowledge.
- I did not fetch Lyons or another reference text on rational resampling.

## Open-source reference implementations (links)

### Takeaway
GNU Radio and SDR++ use an FIR pilot bandpass, a PLL, squaring and explicit delay matching. NGSoftFM and SDRangel use an unfiltered 50 Hz PLL with sin(2θ) and matched interpolators. rtl_fm is minimal: mono, integer impulse-invariant de-emphasis at 170 kHz, and fast atan.

### Cited Findings
- **GNU Radio** stereo receiver — [gr-analog/python/analog/wfm_rcv_pll.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/wfm_rcv_pll.py); de-emphasis — [fm_emph.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/fm_emph.py)
- **SDR++** — [core/src/dsp/demod/broadcast_fm.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/demod/broadcast_fm.h), [core/src/dsp/filter/deephasis.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/filter/deephasis.h)
- **SDRangel** — [plugins/channelrx/demodbfm/bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)
- **NGSoftFM** (fork of SoftFM) — [sfmbase/FmDecode.cpp](https://github.com/f4exb/ngsoftfm/blob/master/sfmbase/FmDecode.cpp)
- **rtl_fm** — [src/rtl_fm.c](https://github.com/osmocom/rtl-sdr/blob/master/src/rtl_fm.c). Its discriminator options are `atan2`, a fast approximation (`pi4 − pi4·(x−|y|)/(x+|y|)`), and a 131072-entry LUT.

### Inferences
- Godwit's architecture (fs/4 offset, FIR decimation, atan2 discriminator, PLL with double/triple angle) is mainstream and comparable to these references. Its specific weak points, in priority order:
  1. Biquad pilot filter phase not compensated (separation).
  2. 19 kHz leakage margin with a 17 kHz cutoff.
  3. De-emphasis HF error of +1 dB (easy fix with one zero).
  4. A cv-based SNR metric that is biased low and should be replaced by USN or pilot SNR for blend and mute.

### Gaps
- I did not fetch Redsea (windytan/redsea) source, so its MPX chain (it consumes MPX, typically 171 kHz from rtl_fm) is not documented here.
- The original SoftFM (jorisvr) repo paths returned 404, so only the NGSoftFM fork was read.
