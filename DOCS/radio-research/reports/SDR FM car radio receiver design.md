# Replace Godwit's SNR number before tuning anything

> **Verified 2026-11-21.** Some statements below were corrected after independent checking. See [VERIFICATION.md](VERIFICATION.md); where they disagree, the verification log wins.

Godwit's FM receiver (radio/src/fmreceiver.cpp) has a sound architecture: an fs/4 offset tune, FIR channel filter, atan2 discriminator, 15 Hz pilot PLL, and double- and triple-angle 38/57 kHz regeneration. The published reference receivers use the same design. Most of the planned improvements are confirmed, but they rank differently than the to-do list suggests. **The single most important change is to replace the envelope-variance "SNR" with the separate indicators car tuners actually use**: RF level, wideband AM (multipath), ultrasonic noise, pilot SNR and frequency offset. That one number currently drives stereo blend, soft mute, seek, scan and the HMI signal bars, and it measures multipath and filter-edge ripple rather than noise. Blend, soft mute, high cut, high blend and noise blanking can only be tuned sensibly once those indicators exist.

Three DSP corrections are small and can be pull requests on their own:
- **De-emphasis:** add one zero to the de-emphasis filter, cutting the +1.01 dB error at 15 kHz to about 0.03 dB.
- **Pilot PLL:** remove or compensate the phase of the Q=95 pilot biquad in front of the PLL. It slopes 0.6° per Hz and makes stereo separation sensitive to pilot and clock offset.
- **Blend:** ramp blend per sample instead of stepping it every 80 ms.

The RTL-SDR sets hard limits that software cannot remove:
- **ADC range:** the 8-bit ADC leaves about 50 dB of usable in-channel range.
- **Gain mode:** the current "auto" gain mode was measured to overload about 10 dB earlier than manual gain.
- **Clock drift:** the dongle's crystal clocks the audio, so the 57 kHz audio drifts against PipeWire's clock and will eventually glitch however good the TCXO is.

On RDS, the decoder is correct for basic PS and RT but misreads North America in ways a broadcast engineer will notice immediately:
- invented call signs outrank PS;
- PTY 24–26 and 30–31 are blank;
- every accented character is dropped.

AM is a hardware decision before it is a DSP one. The installed R820T dongle cannot tune medium wave; an RTL-SDR Blog V4 or an upconverter is required. Finally, the work plan at the end is ordered so that a measurement harness comes first, small fixes go out as first pull requests, and the metric/weak-signal rewrite, audio clock work, gain control and AM go to maintainer issues before any code is written.

## The DSP core is mainstream; three parameters are measurably off

### De-emphasis: keep the pole, add one zero

The 75 µs de-emphasis at fmreceiver.cpp:392 is an impulse-invariant one-pole filter at the 57 kHz audio rate. Its error is not a bug; it is inherent to the method at that rate. Aliasing of the −6 dB/octave skirt lifts the top octave.

I recomputed the response from the code's own coefficients:
- **+0.11 dB at 5 kHz, +0.44 dB at 10 kHz and +1.01 dB at 15 kHz**, matching the project's own probe.
- If the output moves to 48 kHz, the error grows to +1.44 dB at 15 kHz.

The other open-source receivers do no better:
- SDR++ uses a backward-Euler variant of the same one-pole ([SDR++ deephasis.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/filter/deephasis.h)).
- rtl_fm uses the impulse-invariant coefficient as a rounded integer reciprocal, hard-coded to 75 µs ([rtl_fm.c](https://github.com/osmocom/rtl-sdr/blob/master/src/rtl_fm.c)).
- GNU Radio pre-warps the corner and uses the bilinear transform, putting the zero at Nyquist ([GNU Radio fm_emph.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/fm_emph.py)). At audio rates that is worse in the opposite direction: about −2.3 dB at 15 kHz at 57 kHz (computed), which gives a dull top end.

**The cheapest accurate fix keeps the impulse-invariant pole p = exp(−1/(fs·τ)) and adds a real zero q**, giving H(z) = g·(1 − q·z⁻¹)/(1 − p·z⁻¹) with g = (1−p)/(1−q).
- At 57 kHz and 75 µs: p = 0.79143 and q = −0.1166 give a maximum error of **0.027 dB** from 20 Hz to 15 kHz. I re-verified this.
- At 50 µs: p = 0.70407 and the same q ≈ −0.116.
- At 48 kHz: q ≈ −0.124 keeps the error near 0.06 dB.

The cost is one multiply-add per channel per sample. Make τ a parameter (75/50 µs), since GNU Radio's own documentation notes that 75 µs applies in the US and Korea and 50 µs elsewhere ([fm_emph.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/fm_emph.py)). The fitted coefficients are my own computation; no primary paper with discretisation error tables was retrieved.

### Pilot PLL: the biquad in front of the loop is the separation risk

> **Correction (2026-11-21, verified by simulation):** this section is wrong about the effect on separation. A 38 kHz phase error φ only scales L−R by cos φ, so separation = 20·log10((1+cos φ)/(1−cos φ)) = 40·log10(cot(φ/2)), not 20·log10(cot(φ/2)). The biquad's 0.6°/Hz slope (confirmed) therefore limits separation to about 67 dB at ±2 Hz, and the repo's receiver measured 32.7 → 32.5 dB with the pilot moved to 19002/18998 Hz. The real limit on separation is the discriminator's sinc(f/fs) droop: the 38 kHz subcarrier comes out at sinc(38/228) = 0.955, which caps separation at about 32.7 dB; scaling L−R by 1.0472 raised it to 72.7 dB at 1 kHz (55.5 dB at 14 kHz). Plan item 1.2 was changed accordingly.

The 38 kHz carrier's phase error φ sets stereo separation as 20·log10(cot(φ/2)):

| 38 kHz phase error | 0.5° | 1° | 2° | 5° | 10° |
|---|---|---|---|---|---|
| Separation ceiling | 47 dB | 41 dB | 35 dB | 27 dB | 21 dB |

A 19 kHz error doubles at 38 kHz.

Godwit feeds its PLL through an RBJ bandpass biquad with Q = 95 at fmreceiver.cpp:428–438. Its phase is zero only exactly at 19,000.0 Hz in the code's assumed sample-rate scale. I computed the slope from the code's coefficients: **−0.6° at 1 Hz off, −1.2° at 2 Hz and −3.0° at 5 Hz**. That doubles to 2.4° at 38 kHz for a 2 Hz offset, which caps separation near 34 dB.

Two mechanisms produce that offset:
- **Transmitter pilot tolerance.** The often-quoted ±2 Hz figure was not fetched from ITU-R BS.450, so treat it as unverified.
- **The dongle's own clock error.** This point corrects the research note, which called it negligible. The discriminator output is sampled at 228 kHz·(1+ε) while the code assumes exactly 228 kHz, so a true 19 kHz pilot appears ε·19 kHz off. That is 0.01 Hz for a 0.5 ppm TCXO, which is irrelevant, but **1.9 Hz for a 100 ppm plain-crystal dongle**, which costs about 13 dB of separation.

The reference designs avoid the problem in one of two ways:
- **Linear-phase FIR plus delay matching.** GNU Radio uses an 18.98–19.02 kHz FIR pilot filter and delays the composite by the filters' group delays ([GNU Radio wfm_rcv_pll.py](https://github.com/gnuradio/gnuradio/blob/main/gr-analog/python/analog/wfm_rcv_pll.py)). SDR++ uses an 18.75–19.25 kHz FIR and delays both L+R and L−R by (taps−1)/2+1 ([SDR++ broadcast_fm.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/demod/broadcast_fm.h)).
- **No bandpass at all.** NGSoftFM feeds the MPX straight into a 50 Hz loop with a phase detector saturated at ±45° and a 0.4 s lock delay ([NGSoftFM FmDecode.cpp](https://github.com/f4exb/ngsoftfm/blob/master/sfmbase/FmDecode.cpp)). SDRangel does the same with a 50 Hz loop ([SDRangel bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)).

For Godwit the smallest change is to keep the biquad for the lock detector only and drive the PLL phase detector from the raw MPX. The 15 Hz loop already provides far narrower selectivity than the 200 Hz biquad.

The alternative is to subtract the biquad's known phase at the PLL's measured frequency. `m_freq` is already available, and the correction is about −2Q·Δf/f0 radians.

The 15 Hz loop bandwidth itself is fine and quieter than the references' 50 Hz. Phase jitter is about 1/√(2·SNR_loop) rad, so keeping it under 1° needs about 32 dB SNR in the loop bandwidth. On weak signals, separation therefore becomes jitter-limited, which is one reason pilot SNR belongs among the blend inputs. Regenerating 38 and 57 kHz by double and triple angle from the NCO, as Godwit does at fmreceiver.cpp:452–454, is exact and matches NGSoftFM and SDRangel.

Gain and timing mismatch between the M and S paths also limit separation:
- **Gain.** A 2% S/M gain error caps separation at 40 dB.
- **Timing.** One 228 kHz sample of skew costs 13.5 dB at 15 kHz.

Godwit filters M and S with identical 189-tap FIRs at the same point in the chain, so both are inherently matched. Keep it that way in any refactor.

### Two items on the list are already fine

The research notes worried that the 17 kHz audio low-pass leaves the 19 kHz pilot "in transition". **That concern does not survive computation.** Evaluating the actual 189-tap Hamming design at 228 kHz gives:
- −0.01 dB at 15 kHz;
- −6.0 dB at 17 kHz;
- **−52.7 dB at 19 kHz**;
- at least −60 dB from 21 kHz up through the 38 kHz products.

With the pilot at roughly −20 dB relative to full modulation, the residue sits more than 70 dB down, so no pilot notch or cancellation is needed.

Likewise, the 135-tap channel filter at 1.14 MS/s measures:
- −0.02 dB at ±100 kHz;
- −6 dB at ±114 kHz;
- −51 dB at ±128 kHz.

That is a reasonable "wide" setting. It does truncate the outer Bessel sidebands, because Carson's rule gives about 256–270 kHz for 75 kHz deviation with 53–60 kHz of baseband. The main side effect is envelope ripple on dense, heavily processed audio, which matters for the next section.

### Adaptive IF bandwidth is a later item

Car tuners vary the IF bandwidth by condition:
- NXP's TEF6862 automatic bandwidth "depends on the signal conditions", using adjacent-channel, deviation, detuning, multipath and level detectors ([NXP TEF6862HL](https://www.nxp.com/products/TEF6862HL)).
- The TEF668x calls this PACS, Precision Adjacent Channel Suppression ([NXP TEF668X](https://www.nxp.com/products/radio/low-if-tuner-high-performance-one-chip:TEF668X)).
- No open-source SDR receiver examined does this. SDRangel only exposes a user-set bandwidth.

A Godwit version would provide 3–5 precomputed FIR sets, for example ±114/90/70/50 kHz. It would switch narrower when guard-band power at 100–150 kHz offset exceeds in-channel power by a threshold, and widen again with hysteresis of a few hundred ms. Each switch costs nothing extra in compute; 135 taps polyphase at 1.14 MS/s is about 62 M MAC/s. The often-quoted TEF range of 56–311 kHz could not be verified because the manual returned 403.

## One envelope number is doing four jobs

### Why strong stations read only 15–20 dB

Godwit's quality figure (fmreceiver.cpp:471–480) is the coefficient of variation of |IQ| over 80 ms, converted as if all envelope variation were Gaussian noise. An FM carrier through an ideal filter has a constant envelope, so anything that creates AM pushes the reading down. Three things do:
- channel-filter truncation of sidebands at peak deviation;
- multipath;
- adjacent-channel leakage.

A cv of 0.07 maps to about 17 dB, matching the README's statement that real stations read 15–20 dB. That is a bias floor, not noise. The project's own probe shows the other side of the problem: a loud, heavily clipped *synthetic* programme read 27.8 dB and went to full stereo. Real-air behaviour is therefore unknown until recordings are made.

In NXP's terminology, this metric is **WAM** (wideband AM), not SNR. NXP defines WAM as the AC component of the level signal through a 21 kHz bandpass: "There is no WAM signal at good RF receiving conditions," and under multipath it indicates the magnitude of the multipath ([NXP AN02106 §4.7.2](https://www.nxp.com/docs/en/application-note/AN02106.pdf)).

### The indicator set car tuners use

**Car tuners never use a single SNR.** The TEF668x derives its quality picture from separate detectors ([NXP TEF668X data sheet](https://radiokot.ru/forum/download/file.php?id=381990)):
- level;
- frequency offset;
- USN, for multipath and adjacent channels;
- WAM, for multipath;
- modulation (deviation);
- pilot.

Its weak-signal actions each take the worst of several indicators:
- soft mute uses level, USN and WAM;
- high cut, stereo blend and stereo high blend each use level, USN, WAM and modulation.

The TEF689x defines USN as the MPX through a 4-pole 100 kHz high-pass ([AN02106 §4.7.3](https://www.nxp.com/docs/en/application-note/AN02106.pdf)). The Si47xx similarly reports RSSI, SNR, a 0–100 multipath metric and pilot presence ([Skyworks AN332](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)).

The table maps each indicator to a concrete Godwit implementation.

| Indicator | Godwit implementation | Drives |
|---|---|---|
| Level | mean \|IQ\|² after the channel filter in dBFS, minus tuner gain; 60 ms–1.5 s smoother | soft mute, seek, signal bars |
| WAM | \|IQ\|/mean − 1, band-passed ~10–40 kHz (or 21 kHz as NXP), RMS over ~1 ms, in % AM; this is the existing cv recast | blend (fast), high cut |
| USN | MPX power at 100–112 kHz at 228 kHz rate, or pre-decimation discriminator above 100 kHz, normalised to 75 kHz deviation | blend, high cut, soft mute, noise blanker |
| Pilot SNR | pilot power from the PLL I-arm against noise in the arm bandwidth | blend (≈35–40 dB for full stereo) |
| Offset | mean of the demodulated MPX, in Hz | tuning/ppm loop, seek validation |
| Guard-band ratio | power at 100–150 kHz offset before vs after the channel filter | adaptive IF bandwidth only |

**USN placement.** The 228 kHz MPX rate puts Nyquist at 114 kHz, so the TEF's above-100 kHz band barely fits. The research notes also caution, from general knowledge rather than a fetched source, that North American SCA subcarriers at 67 and 92 kHz can occupy the 60–100 kHz region. Measuring USN at 100–112 kHz, or on the discriminator output before decimation, is therefore safer than the 60–100 kHz band suggested elsewhere.

**Combining indicators.** Combine them NXP-style as a "max-of" per action, so one good reading cannot mask a bad one. The PE5PVB TEF6686 firmware's display SNR, `0.462·level − 0.082·usn + 10`, shows how a single display number can be synthesised from these indicators for the HMI bars ([PE5PVB TEF6686_ESP32](https://github.com/PE5PVB/TEF6686_ESP32)).

**Protocol.** The `status` line and both seek thresholds depend on the old number:
- the 9 dB station threshold at main.cpp:61 and hmi/src/radio.cpp:18;
- the 3–19 dB bar mapping at radio.cpp:28–31.

So the metric change must add fields to the helper protocol rather than silently redefine `snr`.

### Weak-signal processing parameters from the datasheets

Godwit currently has two weak-signal controls:
- **Blend** ramps over cv-SNR 10→16 dB and is updated once per 80 ms block with a 0.5 smoothing factor (fmreceiver.cpp:493–494). It therefore steps, and steps symmetrically.
- **Soft mute** ramps over 6→12 dB down to a floor of 0.03, which is −30 dB (fmreceiver.cpp:496).

There is no high cut, high blend, multipath override or noise blanker.

The industry defaults are asymmetric (fast attack, slow release), wider and shallower:

| Control | Si47xx default (AN332) | NXP practice | Recommended Godwit starting point |
|---|---|---|---|
| Stereo blend, level | full stereo ≥49 dBµV, full mono ≤30 dBµV | blend begins ~40 dBµV conventionally | ramp span 10–15 dB on level |
| Stereo blend, SNR | stereo ≥27 dB, mono ≤14 dB | — | drive from USN + pilot SNR |
| Blend multipath | stereo <20%, mono >60%; release ≈1.64 s | MPH decay 6–24 s | WAM override, release 1–2 s |
| Blend timing | attack ≈16 ms, release ≈164 ms | WAM/USN 1 ms detectors | attack 15–20 ms, release 150–300 ms |
| Soft mute | starts below SNR 4 dB, 2 dB/dB, max 16 dB, ~8000 dB/s | level + fast USN; USN depth limited | max 12–20 dB, engage only near noise |
| High cut | SNR 24→15 dB; corner floor 2–11 kHz; attack ≈3 ms, release ≈3.3 s | separate LPF, not shifted de-emphasis; FM floor to ~6.8 kHz fixed, 2–3 kHz dynamic | 15 kHz → 2–3 kHz floor, 3 ms / 1–3 s |
| After retune | — | level/MPH switch to 60 ms attack/decay | fast constants for ~0.5 s |

The Si47xx values are from [AN332](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf). The NXP values are from [AN02106](https://www.nxp.com/docs/en/application-note/AN02106.pdf) and the [TEF668X data sheet](https://radiokot.ru/forum/download/file.php?id=381990).

Three details matter for implementation:
- **High cut is a separate filter.** NXP states that high cut is a standalone low-pass "not realized by shifting of the de-emphasis filter" ([AN02106 §4.5](https://www.nxp.com/docs/en/application-note/AN02106.pdf)). The de-emphasis fix and high cut are therefore independent work items.
- **Stereo high blend.** The PE5PVB firmware configures the TEF's stereo high blend with its own level, noise and multipath curves, limiting L−R bandwidth before full blend ([PE5PVB TEF668X.cpp](https://github.com/PE5PVB/TEF6686_ESP32)). In Godwit this is a moving low-pass on L−R from 15 kHz down to 2–3 kHz, which keeps low-frequency separation where noise is least audible.
- **Ramp, don't step.** Hardware tuners express transitions as rates, such as blend as 65536/time-ms and soft mute in dB/s, not as block steps ([AN332](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)). Compute targets per block, then ramp per sample or per 32–64-sample sub-block.

**Godwit's soft mute is far deeper than industry practice.** The Si47xx engages only below 4 dB SNR and stops at 16 dB. Car tuners let blend and high cut do most of the work.

All thresholds in the table are starting points. No published mapping from envelope-AM percent to Si47xx "multipath %" exists, and the TEF668x defaults for its *_Time commands were not retrievable, so calibration on Godwit field recordings is unavoidable.

### Multipath and noise are distinguishable, and the blanker goes before decimation

**Separating multipath from noise:**
- **Multipath** produces strong envelope AM at moderate or high level. Respond with fast blend, slow release, mild high cut and little soft mute.
- **Noise** raises USN at low level without strong low-frequency envelope AM. Respond with blend, high cut and soft mute.

A constant-modulus (CMA) equaliser on IQ is the software analogue of the TEF668x "Channel Equalizer" and "Improved Multipath Suppression" ([TEF668X data sheet](https://radiokot.ru/forum/download/file.php?id=381990)). It is a later, research-grade item.

Two RTL-specific cautions apply to WAM:
- R820T AGC gain steps and 8-bit quantisation create envelope artefacts. Run with tuner AGC off, or ignore WAM samples around gain changes.
- The detector needs the envelope after the channel filter, which Godwit already computes.

**Noise blanker design.** Car FM blankers trigger on ultrasonic MPX energy plus spikes on the level signal, with adaptive thresholds from an AGC on each. In FM they act as a sample-and-hold on the decoder output ([AN02106 §2.2.4–2.2.5](https://www.nxp.com/docs/en/application-note/AN02106.pdf)). The patent literature adds three points:
- detect above 90 kHz with a threshold tracking average noise ([US4327446](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/4327446));
- de-emphasis stretches pulses, and an ultrasonic detector can mistake adjacent-channel energy for noise ([US4975953](https://patents.google.com/patent/US4975953));
- a delay line aligns the gate with the impulse ([US5036543](https://patents.google.com/patent/US5036543)).

For Godwit, put the detector on the pre-decimation discriminator or the |IQ| at 1.14 MS/s, where ignition pulses are shortest, and blank the MPX before stereo decoding and de-emphasis:
- trigger at k ≈ 4–10× a 10–50 ms running mean;
- replace about 10–50 µs (2–12 samples at 228 kS/s) by interpolation;
- rate-limit triggers, and if the trigger rate exceeds a ceiling, hand over to USN-driven processing.

No public default blank widths or thresholds exist for TEF or Si blankers, so these values are engineering estimates.

## The dongle sets the ceiling: 8 bits, an AGC that hurts, and two clocks

### Gain: manual steps plus a software loop

librtlsdr's "auto" tuner gain is not a software AGC:
- **Mode 0** turns on the R82xx's LNA and mixer AGC and fixes the VGA at "26.5 dB".
- **Manual mode** disables both AGCs and sets the VGA to 16.3 dB.

Both settings are in [tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c). Godwit uses mode 0 unless `--gain` is passed (main.cpp:280, 304).

HB9AJG's measurements, the only published characterisation found, show ([HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)):
- an R820T at full gain overloads with phantom signals at **about −62 to −68 dBm** for an in-passband signal;
- each dB of gain reduction raises that limit about 1 dB;
- tuner AGC "degrades the overload limit by about 10 dBs".

These measurements were taken at 145 MHz, not 100 MHz. The ADC has an ENOB of about 7 ([rtl-sdr.com](https://www.rtl-sdr.com/about-rtl-sdr/)), giving roughly 44 dB across 1.14 MHz plus about 7.6 dB of processing gain into a 200 kHz channel. **That is about 50 dB of usable range.** A strong local inside the capture window will desense a weak station 400 kHz away, and no DSP fixes that.

The recommended control is manual gain chosen from `rtlsdr_get_tuner_gains()`, with RTL2832 digital AGC left off and a software loop running every 100–250 ms:
- **Step down at once** when the fraction of 0/255 bytes exceeds about 1e-5 to 1e-4, or RMS exceeds about −10 dBFS.
- **Step up** only after 2–3 s without clipping, with RMS below about −25 dBFS, and at most once per second.

These thresholds are engineering suggestions to calibrate near a known strong transmitter.

Three hardware limits come with this:
- **No FM band-stop.** A band-stop cannot protect FM reception, because the wanted signal is FM. Only attenuation and the R820's ±3 MHz tracking filter help against strong in-band stations.
- **The V4 notch is off for FM.** The RTL-SDR Blog V4's broadcast notch is switched off when tuned to 85–112 MHz ([tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c)). The V4 protects other services from FM, not FM from FM.
- **Clock harmonics.** The 28.8 MHz harmonics at 86.4 and 115.2 MHz fall just outside the band.

Godwit's fs/4 offset already keeps the LO's DC spur 285 kHz away from the wanted station.

### Frequency accuracy is solved by the TCXO; the audio clock is not

Measured clock accuracy differs sharply by oscillator:
- **TCXO dongles** are close enough that FM needs no calibration: NESDR SMArt v5 at ±0.5 ppm ([Nooelec](https://www.nooelec.com/store/sdr/nesdr-smart-sdr.html)), Blog V4 at 1 ppm ([rtl-sdr.com](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)). That is under 100 Hz at 100 MHz.
- **Plain-crystal dongles** run 30–150 ppm off and drift 20+ ppm while warming up ([rtl-sdr.com TCXO review](https://www.rtl-sdr.com/review-tcxo-modified-rtl-sdr-dongle/)). At 100 MHz that is a 5–15 kHz tuning offset, which pushes the signal asymmetrically into the channel filter.

The fix is cheap. The mean of the demodulated MPX is the carrier offset, and it can feed `rtlsdr_set_freq_correction`, which corrects both the tuner and the RTL2832 resampler ([librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)). Run it as a loop with a time constant of seconds. As shown above, it also removes the plain-crystal contribution to pilot-biquad phase error.

**The audio clock problem remains even with a perfect TCXO.**

*Why it glitches.* The 57 kHz output is 57000·(1+ε) in PipeWire's terms, and nothing closes the loop between dongle and sound-card clocks:
- Plan for up to ±200 ppm relative error; the head-unit codec's accuracy is unsourced.
- At 100 ppm the 16 KB pipe (main.cpp:295; 4096 frames, about 72 ms) fills from empty in about 12 minutes. Then `writeAudio` drops audio, or, if the dongle is slow, `pw-play` underruns.
- Most SDR apps simply live with this. GNU Radio's list notes that with two hardware clocks underrun or overrun "is inevitable" ([discuss-gnuradio](https://lists.gnu.org/archive/html/discuss-gnuradio/2016-06/msg00257.html)), and SDR++'s audio sink has no drift correction ([SDR++ audio_sink](https://raw.githubusercontent.com/AlexandreRouma/SDRPlusPlus/master/sink_modules/audio_sink/src/main.cpp)).

*The clean fix.* Replace `pw-play` with a native `pw_stream` inside the helper, feeding it from a lock-free ring:
- size the ring at about 500 ms, with a 150–200 ms target fill;
- run a PI controller on the fill level, with τ ≈ 1–2 s and the ratio clamped to ±300 ppm for TCXO dongles or ±1000 ppm for plain crystals;
- apply the ratio with `pw_stream_set_rate()`, which "adjust[s] the resampler rate" and exists **only since PipeWire 1.4.0** ([PipeWire pw_stream](https://docs.pipewire.org/group__pw__stream.html)).

pw-cat has no drift option, and its `--latency` only delays the glitch ([pw-cat man page](https://docs.pipewire.org/page_man_pw-cat_1.html)). If AGL Vimba ships an older PipeWire, the fallback is a variable-ratio resampler in the helper:
- the nominal ratio is 57→48 kHz = 16/19;
- libsamplerate, soxr and speexdsp all support variable ratios;
- emitting 48 kHz directly also removes PipeWire's second resampler and makes de-emphasis run at a known rate.

Two things remain unconfirmed: the PipeWire version in the AGL Vimba image, and the sign convention of the rate ratio.

### USB: small buffers are right, DSP in the callback is not

This corrects the research note, which assumed librtlsdr's default 15 × 256 KB buffers and a 115 ms burstiness problem. **Godwit already uses 6 × 16 KB buffers (main.cpp:55–56), about 7 ms each and 43 ms in flight.** Burstiness is therefore not the issue.

The real exposure is that the whole receiver runs inside the USB callback (main.cpp:107–123). Any scheduler stall over about 43 ms loses samples, and librtlsdr does not detect drops ([librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)).

Simply enlarging the USB queue would lengthen the retune discard (`kDiscard`, 60 ms) and slow seek. The better design:
- the callback copies into a ring tagged with the retune generation;
- a worker thread processes the ring and discards old-generation data exactly;
- the worker counts bytes per second against 2.28 MB/s to detect drops, and resets the PLL and de-emphasis state when it finds one.

A related packaging point: the image builds against the generic `rtl-sdr` recipe (godwit-radio.bb:23). The Blog V4 needs osmocom ≥ 2.0 or the rtlsdrblog fork, otherwise it shows "no signals, wrong frequency, or corrupted signals" ([RTL-SDR Blog V4 guide](https://www.rtl-sdr.com/v4/)). Pin the version before supporting the V4.

## RDS decodes cleanly but misreads North America

### Call signs: demote them, then complete the mapping

Godwit's K/W arithmetic (fmreceiver.cpp:290–305) matches NRSC-4-B's base formula: K = 0x1000–0x54A7, W = 0x54A8–0x994F ([NRSC-4-B §D.7](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)). It is incomplete in four ways:
- Calls whose PI has a zero second nibble are reassigned to A___.
- Calls whose PI ends in 00 are reassigned to AF__.
- Three-letter calls use a fixed table at 0x9950–0x9EFF.
- B/D/E codes are nationally-linked networks.

It is also wrong in three cases: TMC substitution of a leading 0x1, NAB-assigned translator PIs, and Canadian 0xC___ and Mexican 0xF___ codes, which have no published reverse formula ([ISED PI codes](https://ised-isde.canada.ca/site/spectrum-management-telecommunications/en/node/587)).

Most importantly, **the NRSC tells receivers not to do this at all.** The relevant guidance ([NRSC-G300-C §5.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)):
- "the receiver cannot rely on PI codes to provide call letters for U.S. broadcast stations";
- receivers should decode RT+ StationName.Short instead;
- G300-C also documents factory-default PIs left on air.

Godwit does the opposite. hmi/src/radio.cpp:405–409 prefers the computed call over PS, and because PI→call runs for any PI in range regardless of Country, invented K/W calls replace real names outside North America.

The fix order:
1. Demote the call to a fallback (RT+ StationName.Short, then a stable PS, then call).
2. Port redsea's un-mapping and tables (MIT, licence-compatible with attribution; [redsea tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc)). Check the 3-letter table's ordering, since it is searched with `lower_bound`.
3. Show no call for 0xC/0xF/B/D/E PIs, or for raw PIs that should have been remapped.
4. Gate the whole feature on region.

Store station names keyed by (frequency, PI), not frequency alone, so a new market invalidates them. NRSC-4-B §D.7.3 likewise says to accept a new PI on a preset after a failed PI search.

### PTY tables and the alert codes

hmi/src/radio.cpp:36–41 has the right RBDS names for 0–23 and 29. It blanks **24 Spanish Talk, 25 Spanish Music, 26 Hip Hop, 30 Emergency Test and 31 Emergency**, all defined in NRSC-4-B Table F.2. Only 27–28 are unassigned ([NRSC-4-B Annex F](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)). G300-C notes that older radios showed these newer codes as "Undefined" ([G300-C §5.2](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)).

Handling for the alert codes:
- **PTY 30** may at most display "TEST" and is not for switching.
- **PTY 31** is "only used in a receiver for dynamic switching". On the tuned station, show an ALERT banner and override mute for its duration.

The European RDS table differs from code 2 upward ([redsea tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc)), so select it by the Country setting. Do not infer it from the PI: 0xC is also a European country code.

### Character set, PS assembly and RT handling

`rdsChar` (fmreceiver.cpp:59–62) passes only 0x20–0x7E. That is wrong in three ways:
- RDS uses its own IEC 62106 Table E.1 set, "not ASCII, ISO-8859-1 or Unicode" ([G300-C §9.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)).
- Every accented character used by Canadian French and US Spanish stations sits in 0x80–0xFF and is blanked.
- 0x24 is ¤ in RDS, while "$" is 0xAB, and 0x5E, 0x60 and 0x7E also differ from ASCII ([G300-C §9.2–9.3](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)).

Port the 256-entry table to UTF-8, keeping 0x24 as "$" pragmatically, since US broadcasters misuse it. The E.1 glyph table itself was not fetched, so take it from IEC 62106 or redsea's charset code.

RT A/B clearing and 0x0D termination already match NRSC-4-B §6.1.5.3. Two changes remain for RT:
- 0x0A and 0x0B should become line or headline breaks.
- Unknown codes should become spaces, which they already do.

PS assembly waits for all four segments. That is good for static PS, but G300-C calls scrolling PS "a common practice within the U.S." ([G300-C §6.9](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)), so segments from different frames still mix. Require each segment twice identically, flag PS as dynamic when it changes often, and never save a dynamic PS as the preset name.

### Sync and demodulation sensitivity

Godwit's RDS path is already better than the notes assumed. It derives 57 kHz from the pilot PLL by triple angle and uses squaring with 0.2 s averaging only to trim residual phase (fmreceiver.cpp:118–128, 452–459). The weaknesses are elsewhere:
- **Sync on two blocks.** redsea requires three ([redsea block_sync.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/block_sync.cc)).
- **No error correction.** redsea corrects 1- and 2-bit bursts only, only in sync, and only for the expected offset word.
- **8-phase max-energy timing.** redsea uses an RRC-matched polyphase symbol synchroniser and a decision-directed PLL ([redsea subcarrier.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/dsp/subcarrier.cc)).
- **Mono stations with RDS.** These have no pilot, so the squaring estimator must track any subcarrier offset. The project's probe saw groups fall from 80 to 56 with a 3 Hz offset.

A Costas loop and a Gardner-type timing loop address the last two. FEC raises the undetected-error rate, so keep the PI-twice and segment-repeat checks.

### Feature priorities for a North American car

After the fixes above, the North American priorities are:
1. **RT+** for artist, title and StationName.Short. Group 3A ODA parsing comes first. The RT+ AID 0x4BD7 is from memory and needs verification.
2. **PTY 31 handling.**
3. **AF method A with PI verification.** "AF has not been a widely used feature in the U.S.", and receivers "should not switch if the code is not the same" ([G300-C §5.7](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)).
4. **CT** for display only. Stations are told not to send it unless accurate ([G300-C §5.8](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)), so never set the clock from it unless it agrees with GPS or RTC.
5. **A TP indicator.** Automatic TA switching "is discouraged" in the US ([G300-C §5.4](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)).

EON, Long PS and eRT are European priorities.

The SDR's wideband advantage for AF is limited at 1.14 MS/s. Only alternate frequencies inside roughly ±570 kHz of the LO can be PI-checked in the background. A wider capture would turn the dongle into the "dual tuner" that single-tuner car radios emulate with muted PI checks ([US 20110306313](https://patents.justia.com/patent/20110306313)).

## AM is a hardware decision before it is a DSP one

> **Correction (2026-11-21):** this section assumes the project's NESDR SMArt v5 cannot tune medium wave. It can: Nooelec's datasheet gives 0.1–25 MHz by direct sampling (Q branch), with no modification needed. AM can therefore start on the current dongle, with the direct-sampling limits described below for the V3 (no front-end filtering, 8-bit, aliasing and overload from strong locals). The V4 and upconverters remain the higher-quality options.

The NESDR SMArt v5's R820T2 cannot tune medium wave, so AM needs one of three hardware routes:
- **RTL-SDR Blog V4 (recommended).** It puts an SA612 upconverter with a 28.8 MHz LO and a triplexer on one SMA. Its datasheet plot shows an MDS of **about −134 dBm at 1 MHz against about −109 dBm for V3 direct sampling**, about 25 dB better, with no Nyquist folding at 14.4 MHz ([RTL-SDR Blog V4 datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)). The costs are about 2–3 dB worse VHF/UHF sensitivity, which matters to ADS-B and AIS margins, and the R828D is out of production ([V4 release](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)). Field reports at MW are mixed, from many European stations at night to "barely" hearing locals ([RadioReference thread](https://forums.radioreference.com/threads/zero-success-with-rtl-sdr-v4-on-medium-wave.492023/latest)).
- **External upconverter** such as the SpyVerter R2, with a 120 MHz LO ([Airspy](https://airspy.com/spyverter-r2/)). It works, but it needs an RF switch to coexist with FM and the other services.
- **V3 direct sampling.** It is unfiltered and attenuated below 2 MHz ([V3 guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/)), so rule it out for broadcast quality.

The antenna matters as much as the dongle. A car whip is electrically tiny at MW, and loading it with 50 Ω throws away most of the signal. It needs a JFET high-impedance buffer ([PA3FWM mini-whip](https://pa3fwm.nl/projects/miniwhip)), which can be powered from the V4's 4.5 V, 180 mA bias tee ([V4 guide](https://www.rtl-sdr.com/v4/)).

EV propulsion noise is why several automakers dropped AM ([IEEE Spectrum](https://spectrum.ieee.org/am-radio-ev-interference)). The AM Radio for Every Vehicle Act passed the House by voice vote on 15 September 2026 with an eight-year sunset, and still needs the Senate ([Radio World](https://www.radioworld.com/news-and-business/headlines/u-s-house-passes-bill-to-require-am-radio-in-new-cars)).

The DSP design is well defined:
- **Detector:** synchronous AM with DSB/USB/LSB selection, falling back to envelope detection when the PLL cannot lock. SDRangel's demodulator is a working template ([SDRangel amdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodam/amdemodsink.cpp)).
- **AGC:** carrier-derived, like SDR++'s CARRIER mode ([SDR++ am.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/demod/am.h)), with a few ms attack and hundreds of ms decay with hang.
- **Bandwidth:** audio settings of 10/6/4.5/3 kHz. 10 kHz matches NRSC-1's analog limit and NRSC-2's 20 kHz occupied bandwidth.
- **De-emphasis:** NRSC-1-C's complement of the "modified 75 µs" curve, which limits boost to 10 dB at 10 kHz ([NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)). The exact constants were not extracted, so read §6 before coding.
- **Interference:** a 10 kHz heterodyne notch (9 kHz in 9 kHz regions), and a noise blanker on wideband IQ before the channel filter.
- **Channel plan:** 10 kHz steps from 530 to 1700 kHz, configurable by region.

NRSC-5 AM IBOC is decodable with nrsc5 `--am` ([nrsc5](https://github.com/theori-io/nrsc5)), but it is reverse-engineered against an Xperi-licensed system and should stay optional. A 2.4 MS/s capture spans the whole 1.18 MHz MW band, so a channelizer could give instant tuning and a live station list. Because all locals then share 8 bits, offer a narrower "DX" capture as well.

AM still occupies the single shared dongle, blocking Aircraft, Ships and Weather exactly as FM does. A second dongle is the only route to concurrent service.

## The project's own docs and code disagree in a dozen places

The repository review at commit 438e20b found problems that matter as much as the DSP.

**The radio is FM-only, with North America hard-coded:**
- AM survives only as mockup and simulator demo data. Hmi.qml:105 hard-codes band "FM".
- The v1 mockup's band toggle and Station List were dropped without a note, and v1 remains in DOCS/ unmarked as superseded.
- Settings > Country gates only Aircraft, Ships and Weather. FM is always 87.9–107.9 MHz with 200 kHz steps (main.cpp:47–50), 75 µs, the RBDS PTY table and PI→call.

**Frequency ranges disagree:**
- Voice accepts 87.5–108 MHz in 0.1 MHz steps (Intents.js:210–212).
- The helper silently ignores tunes outside 87.9–107.9 (main.cpp:341).
- `Radio::setFrequency` accepts anything (radio.cpp:104–118), so the HMI shows a frequency the tuner never went to.
- Even-tenth frequencies such as 88.0 pass through and push the knob off the 200 kHz raster.
- The AM ceiling is 1710 kHz in voice but 1700 kHz in the dial.

**Preset save:** a knob-hold uses a rotating slot pointer that resets to 0 on each head-unit boot and is not saved. The first save after every boot overwrites preset 1 (Hmi.qml:105, 1737–1750). This behaviour is undocumented.

**Dongle sharing and lifecycle:**
- The tuner is shared four ways (Radio, Aircraft, Ships, Weather), but comments name only Aircraft (radio.h:11, 35–37). The udev rule comment and os/README omit Weather.
- `Radio::seek` is dropped silently while no helper exists (radio.cpp:122), so a seek just after leaving another SDR app can vanish.
- Phone calls and mute stop the tuner process instead of ducking.
- The last audio source is not saved, so radio never resumes at power-on.
- An antenna-less first scan leaves presets empty, and every later start silently rescans for about 20 s. That figure is unmeasured.

**Documentation gaps:**
- librtlsdr's licence is GPL-2.0-only in Legal.js:64 but GPL-2.0-or-later everywhere else.
- The udev rule covers only 0bda:2832 and 0bda:2838. R828D/V4 and upconverters are unmentioned, and device 0 is always opened.
- "Tuner not connected" also covers permission errors.
- The `GODWIT_RADIO`/`*_BIN` environment variables appear only in code comments.
- FM antenna guidance is one phrase.
- Ducking relies on AGL's stock WirePlumber policy, unverified.
- The DSP design lives only in radio/README.md:41–61.

**Tests and CI:**
- Four Qt-free ctest targets pass, including fmreceiver_test for pilot, blend, separation above 30 dB, RDS and retune.
- Nothing tests main.cpp seek/scan, the HMI radio side or Intents.js parsing.
- There is no CI at all.

## Prioritized work plan

The ordering follows dependencies, not appeal:
- **Phase 0** gives every later change a way to be measured on real air.
- **Phase 1** is a set of independent, low-risk pull requests that fix documented errors without changing behaviour that needs tuning.
- **Phase 2** (signal-quality indicators) is the keystone. Blend, soft mute, high cut, high blend, the noise blanker, adaptive bandwidth and seek all consume it, so it needs a maintainer issue that agrees the protocol fields first.
- **Phases 3–6** are the larger behavioural changes. Each should start as an issue with the parameters below as the proposal.

"PR" means suitable as a first pull request; "Issue" means discuss with the maintainers first.

### Phase 0: measurement foundation

| # | What | Why | Concrete parameters | Files | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 0.1 | IQ record and playback in godwit-radio | Thresholds must be calibrated on real air; synthetic and real readings already disagree (27.8 vs 15–20 dB) | `--record <file>` writes raw u8 IQ with a header (rate, frequency, gain); `--iq-file <file>` replaces librtlsdr input; capture strong, weak, multipath, ignition-noise and adjacent-channel cases | radio/src/main.cpp, radio/README.md | Replayed file produces bit-identical PCM and status lines | M | Issue (CLI surface) |
| 0.2 | Receiver measurement tests | Lock in the computed figures so regressions show | De-emphasis response at 1/5/10/15 kHz; separation at 1 kHz with pilot at 19000 ± 2 Hz and with a simulated ±100 ppm rate error; pilot rejection at 19 kHz above 50 dB | radio/tests (fmreceiver_test) | ctest passes; new cases fail on current code where expected | S | PR |
| 0.3 | CI workflow running the four ctest targets | No CI exists; every later PR benefits | GitHub Actions, Ubuntu runner, Qt-free radio/ build plus ctest | .github/workflows/ | Green run on a PR | S | PR (mention in description) |

### Phase 1: small, self-contained fixes (first pull requests)

| # | What | Why | Concrete parameters | Files | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 1.1 | One-pole/one-zero de-emphasis with τ as a parameter | +1.01 dB error at 15 kHz | p = exp(−1/(fs·τ)); q = −0.1166 (57 kHz); g = (1−p)/(1−q); τ ∈ {75, 50} µs, default 75 | radio/src/fmreceiver.cpp, fmreceiver.h | Error ≤ 0.05 dB from 20 Hz to 15 kHz in test 0.2 | S | PR |
| 1.2 | Remove pilot-biquad phase from the PLL path | 0.6°/Hz phase slope makes separation offset-sensitive | Phase detector on raw MPX (15 Hz loop unchanged); keep the biquad output only for lock detection, or subtract −2Q·Δf/f0 using `m_freq` | radio/src/fmreceiver.cpp | Separation ≥ 40 dB at 1 kHz with pilot ±2 Hz and ±100 ppm in test 0.2 | S | PR |
| 1.3 | Per-sample blend ramp | 80 ms stepped blend produces zipper steps | Interpolate `m_blendNow` linearly across each block, or a one-pole per sample with separate attack and release coefficients | radio/src/fmreceiver.cpp | No step discontinuity in L−R gain in a test sweep | S | PR |
| 1.4 | Complete the RBDS PTY table | Codes 24–26, 30 and 31 are blank | 24 Spanish Talk, 25 Spanish Music, 26 Hip Hop, 30 Test, 31 Alert; 27–28 blank | hmi/src/radio.cpp | Unit or manual check of `genreOf` | S | PR |
| 1.5 | Demote PI call sign below PS | NRSC-G300-C advises against back-calculation; invented calls outside NA | Order: PS (stable) before call; call only as fallback | hmi/src/radio.cpp:405–409 | HMI shows PS when both exist | S | PR |
| 1.6 | Complete PI→call un-mapping | A___, AF__, 3-letter and B/D/E PIs are misdecoded | Port redsea `getCallsignFromPI` order and tables (MIT, attribute); return empty for 0xC/0xF, or for second nibble 0 / trailing 00 | radio/src/fmreceiver.cpp, THIRD_PARTY_LICENSES.md | Test vectors KGTB=0x21C7, WKTI=0x7106, A145, AFA1, 9950=KEX | S | PR |
| 1.7 | RDS E.1 character set to UTF-8 | Accented characters are dropped | 256-entry table; 0x24 shown as "$", 0xAB → "$"; 0x0A/0x0B as breaks; unknown → space; HMI parses UTF-8 | radio/src/fmreceiver.cpp, hmi/src/radio.cpp | é/ñ/ç round-trip test in fmreceiver_test | S–M | PR |
| 1.8 | Frequency range and raster consistency | Voice, helper and HMI disagree | One band constant set (87.9–107.9 MHz, 200 kHz for NA); `setFrequency` validates and snaps; voice rejects out-of-band values; AM ceiling unified at 1700 | hmi/src/radio.cpp, Intents.js, main.cpp | "Set the radio to 87.5/88.0" gives an explicit refusal or snap | S | PR |
| 1.9 | Preset-save slot persistence or documentation | The first knob-hold after boot overwrites preset 1 | Persist `radio.ptr`, or save to the first empty slot; document in radio/README | Hmi.qml, radio/README.md | Reboot and save does not clobber preset 1 | S | PR |
| 1.10 | Documentation fixes | Contradictions and gaps | Licence (Legal.js:64 → GPL-2.0-or-later); dongle-sharing comments; supported dongles and udev IDs; permission troubleshooting; env vars; FM antenna section; left-knob "Next Station"; mark v1 mockup superseded | Legal.js, radio.h, os/README.md, 72-godwit-sdr.rules, hmi/README.md, radio/README.md | Review | S (several) | PR |
| 1.11 | Clip and level telemetry | Overload is currently invisible | Per block: fraction of 0/255 bytes, IQ RMS in dBFS, DC mean; add to the `status` line as new trailing fields | radio/src/main.cpp, fmreceiver.cpp | Values move with `--gain` near a strong station | S | PR |

### Phase 2: signal-quality indicators (keystone; issue first)

| # | What | Why | Concrete parameters | Files | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 2.1 | Level, WAM, USN, pilot SNR, offset indicators | The cv "SNR" is a multipath/ripple metric capped near 17 dB | Level: post-filter dBFS minus gain table, 60 ms–1.5 s smoothing. WAM: envelope AC, 10–40 kHz BP, ~1 ms detector, % AM. USN: MPX 100–112 kHz RMS re 75 kHz dev. Pilot SNR from PLL arm. Offset: mean MPX in Hz | radio/src/fmreceiver.cpp/.h | Recordings from 0.1 separate strong/multipath/weak/adjacent cases cleanly | M–L | Issue |
| 2.2 | Protocol and HMI migration | Seek, scan and bars depend on the old number | Add `level`, `usn`, `wam`, `pilotsnr`, `offset` fields; keep `snr` as a synthesised display value; seek station = level and USN thresholds, pilot or RDS as confirmation | main.cpp, hmi/src/radio.cpp, Hmi.qml | Seek stops on real stations, not spurs; scan time measured and documented | M | Issue |
| 2.3 | Offset-driven ppm correction | Plain-crystal dongles sit 5–15 kHz off; also reduces pilot phase error | Mean-MPX offset, τ of several seconds, `rtlsdr_set_freq_correction` in 1 ppm steps with hysteresis; persist per dongle serial | main.cpp | Offset under 500 Hz after warm-up on a non-TCXO unit | S–M | Issue |

### Phase 3: weak-signal processing (depends on Phase 2)

| # | What | Why | Concrete parameters | Files | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 3.1 | Max-of stereo blend | Current ramp is 6 dB wide, symmetric and from a biased metric | Inputs: level, USN, pilot SNR, WAM override. Span 10–15 dB. Attack 15–20 ms, release 150–300 ms; multipath release 1–2 s. Fast 60 ms constants for 0.5 s after retune | fmreceiver.cpp | Recorded drives: no audible stereo pumping; separation vs level curve | M | Issue |
| 3.2 | Stereo high blend | Keeps low-frequency separation while removing L−R hiss | One-pole or biquad LPF on L−R, corner 15 kHz down to 2–3 kHz, driven before full blend | fmreceiver.cpp | Weak-signal recordings: lower hiss at equal separation below 1 kHz | M | Issue |
| 3.3 | High cut | Missing; NXP implements it separately from de-emphasis | LPF on L/R, corner swept log-linearly 15 kHz → 2–3 kHz floor; attack ≈ 3 ms, release 1–3 s; per-sample or 32–64-sample coefficient updates | fmreceiver.cpp | No zipper noise; corner tracks USN | M | Issue |
| 3.4 | Shallow soft mute | Current −30 dB floor is about twice Si47xx's 16 dB maximum | Max 12–20 dB; engage only near the noise threshold; slope about 2 dB/dB; driven by max(level, USN) | fmreceiver.cpp | Fringe reception stays audible; empty channel still quiet | S | Issue (behaviour) |
| 3.5 | FM noise blanker | Ignition and LED-driver impulses | Detector on pre-decimation discriminator or \|IQ\| at 1.14 MS/s, k = 4–10 × 10–50 ms mean; blank 10–50 µs by interpolation on MPX before stereo decoding; rate-limit; disable above a trigger-rate ceiling | fmreceiver.cpp | Ignition-noise recordings: click count down, no audible chopping on clean recordings | M | Issue |
| 3.6 | Adaptive IF bandwidth | Strong adjacent channels with 8-bit range | FIR sets ±114/90/70/50 kHz; guard-band ratio threshold; widen with hysteresis of a few hundred ms; pair with blend | fmreceiver.cpp | Adjacent-channel recordings: lower USN/THD without loss on clean stations | L | Issue |

### Phase 4: dongle, USB and audio clock

| # | What | Why | Concrete parameters | Files | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 4.1 | Manual gain with software AGC | Tuner AGC overloads about 10 dB earlier | Manual mode; DAGC off; step down at once if clip > 1e-5–1e-4 or RMS > −10 dBFS; step up after 2–3 s clean with RMS < −25 dBFS, at most 1 step/s; per-service defaults for the shared dongle | main.cpp (and other SDR helpers) | Near a strong local: no phantom stations; weak-station quieting equal or better | M | Issue |
| 4.2 | DSP out of the USB callback | A stall over 43 ms loses samples silently | Callback copies into a ring tagged with retune generation; worker thread runs the DSP; drop detection by byte rate (2.28 MB/s) and short buffers; reset PLL and de-emphasis on a drop | main.cpp | Induced CPU stall: no lost samples, or a logged drop with clean recovery; seek time unchanged | M | Issue |
| 4.3 | Native pw_stream with drift control | 57 kHz dongle clock vs sound-card clock; a 72 ms pipe overflows in about 12 min at 100 ppm | Ring of about 500 ms, target fill 150–200 ms; PI loop τ 1–2 s, clamp ±300 ppm (±1000 ppm plain crystal); `pw_stream_set_rate` (PipeWire ≥ 1.4.0), else own variable resampler 57→48 kHz (16/19 nominal); media role Multimedia; node latency about 1024/48000 | main.cpp, CMakeLists, godwit-radio.bb, hmi/src/radio.cpp (process launch) | 8-hour run: zero xruns, fill converges; first confirm the PipeWire version in AGL Vimba | L | Issue |
| 4.4 | librtlsdr version pin and tuner detection | V4 needs osmocom ≥ 2.0 or the Blog fork | Yocto recipe pinned; log `rtlsdr_get_tuner_type` and USB strings; extend udev IDs if needed | os/meta-godwit, main.cpp, 72-godwit-sdr.rules | V4 tunes FM correctly; log shows the tuner type | S | Issue |

### Phase 5: RDS robustness and features

| # | What | Why | Concrete parameters | Files | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 5.1 | Three-block sync and 1–2-bit burst FEC | Sensitivity gap vs redsea | Port redsea sync (3 blocks, at most 6 apart; loss at >42 bad of 50) and syndrome table for 0b1/0b11 at 26 shifts, only in sync, expected offset only; keep PI-twice | fmreceiver.cpp | Group rate vs added noise in fmreceiver_test; no rise in bad PS/RT | M | PR after brief issue |
| 5.2 | Costas carrier and timing loop | Mono+RDS stations with offset lose 30% of groups at 3 Hz | Decision-directed Costas after matched filter; Gardner or polyphase timing loop replacing the 8-phase pick | fmreceiver.cpp | Group rate at 0/3/6 Hz offset roughly flat | M | Issue |
| 5.3 | PS confirmation and dynamic-PS handling; (freq, PI) station keys | Scrolling PS mixes segments; names go stale across markets | Segment accepted after two identical receptions; dynamic flag at more than N changes per minute; never save dynamic PS; key the stored name by (freq, PI) | fmreceiver.cpp, hmi/src/radio.cpp | Scrolling-PS synthetic test; market-change test | M | Issue |
| 5.4 | Region setting for FM | Country is ignored for FM | Band, step, τ (75/50), PTY table (RBDS/RDS), call-sign enable from Settings > Country; new `region` command in the helper protocol | main.cpp, fmreceiver.cpp, hmi/src/radio.cpp, Hmi.qml | Europe: 87.5–108, 50 µs, RDS PTY, no call signs | M | Issue |
| 5.5 | PTY 31 alert and PTY 30 test | NRSC-4-B dynamic-switching intent | Banner while PTY = 31 and override mute; "TEST" only for 30; no auto-tune | hmi/src/radio.cpp, Hmi.qml | Synthetic PTY 31 group raises the banner | S–M | Issue (UX) |
| 5.6 | Group 3A ODA, RT+, CT, TP, AF method A | North American value order | RT+ (verify AID) → artist/title/StationName.Short; CT for display only, sanity-checked against RTC/GPS; TP indicator; AF list parsing with PI-verified, mute-checked switching | fmreceiver.cpp, hmi | redsea comparison on recordings | L | Issue |

### Phase 6: AM

| # | What | Why | Concrete parameters | Files | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 6.1 | Hardware decision | The R820T cannot tune MW | Blog V4 (−134 dBm MDS at 1 MHz; −2–3 dB VHF penalty; end-of-life part) vs upconverter vs second dongle; active high-Z antenna on the 4.5 V bias tee | DOCS, os, radio/README.md | Bench MDS and strong-local test | — | Issue |
| 6.2 | AM demodulator | Broadcast-quality AM | SAM (DSB/USB/LSB) with envelope fallback; carrier AGC (ms attack, hundreds of ms decay, hang); audio BW 10/6/4.5/3 kHz; NRSC-1-C de-emphasis (constants to be read from §6); 10 kHz heterodyne notch; pre-filter blanker; 10 kHz raster 530–1700 kHz | new radio/src/amreceiver.*, main.cpp | Tests modelled on fmreceiver_test; night skywave recordings | L | Issue |
| 6.3 | HMI band switch | AM is hard-coded off | Restore the v1 band toggle; band-specific presets; voice range 530–1700 | Hmi.qml, RadioApp.qml, Intents.js | Simulator and head-unit parity | M | Issue |

## Conclusion

The radio's limits are less about DSP sophistication than about measurement. Godwit already implements the hard parts correctly: the pilot-locked 38/57 kHz regeneration, matched M/S filtering and a pilot rejection of about 53 dB. What it lacks is a truthful picture of the signal. Every car-radio behaviour the user wants (blend, high cut, soft mute, multipath handling, blanking, adaptive bandwidth, seek) is a function of four or five indicators that NXP and Silicon Labs have documented for two decades. Building them, together with a record/playback harness, turns the rest of the list from guesswork into parameter tuning against recordings. A broadcast engineer is unusually well placed to do that tuning.

Two implications are easy to miss.
- **The worst audio artefact is not RF at all.** The audio-clock mismatch will glitch even a perfect signal, and the clean fix depends on the PipeWire version in the AGL Vimba image, which should be checked before any design is proposed.
- **North American RDS correctness and AM are mostly decisions, not code.** Demoting call signs, choosing a dongle and adding a region setting each change user-visible behaviour, so they belong in maintainer issues with the standards cited, while the small, verifiable fixes in Phase 1 build credibility as first pull requests.
