# RTL-SDR hardware limits for an in-vehicle FM receiver, and glitch-free audio streaming to PipeWire

Scope: RTL2832U + R820T/R820T2 (Nooelec NESDR SMArt v5, RTL-SDR Blog V3) and R828D (RTL-SDR Blog V4), as used by the Godwit receiver: librtlsdr, 1.14 MS/s, tuner gain mode 0 ("auto") unless `--gain`, RTL2832 digital AGC off, no clip detection, no ppm correction, 57 kHz s16 stereo written non-blocking into a 16 KB pipe feeding `pw-play --raw --rate 57000`.

## Gain stages, librtlsdr "auto" gain, RTL2832U digital AGC, ADC dynamic range, overload with strong FM signals

### Takeaway
librtlsdr's "automatic" tuner gain is **not** a software AGC. It turns on the R82xx's own LNA and mixer AGC and fixes the VGA at about 26.5 dB. The only published measurement (HB9AJG) found that tuner AGC makes the R820T overload about 10 dB earlier, and that RTL digital AGC does not help. An R820T overloads when one in-passband signal reaches about -62 to -68 dBm at full gain. Each dB of manual gain reduction raises that limit by about 1 dB. So for broadcast FM in a car, the receiver should use manual gain plus its own software AGC loop driven by ADC statistics, and stations above about -40 dBm need a hardware band-stop or notch filter.

### Cited Findings
**What librtlsdr does**
- `rtlsdr_set_tuner_gain_mode(dev, 0)` on R820T/R828D calls `r82xx_set_gain(&devt->r82xx_p, manual, 0)` — [rtl-sdr-blog librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)
- In auto mode, `r82xx_set_gain` writes the following — [rtl-sdr-blog tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c):
  - reg 0x05 bit 0x10 cleared, which turns LNA auto on;
  - reg 0x07 bit 0x10 set, which turns mixer auto on;
  - reg 0x0c = 0x0b (mask 0x9f), with the comment "set fixed VGA gain for now (26.5 dB)".
- In manual mode, LNA and mixer auto are turned off and the VGA is set to 0x08, commented as "16.3 dB". The frequency-dependent VGA selection is commented out. — [tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c)
- Manual gain uses the LNA and mixer step tables (tenths of dB per step). They were measured at 928 MHz, -60 dBm. — [tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c)
  - LNA: `{0,9,13,40,38,13,31,22,26,31,26,14,19,5,35,13}`
  - Mixer: `{0,5,10,10,19,9,10,25,17,10,8,16,13,6,3,-8}`
  - A VGA table exists but the manual loop does not use it.
- Filtering inside the R82xx — [tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c):
  - IF low-pass options from 1.7 MHz down to 350 kHz, set via `r82xx_set_bandwidth` (`rtlsdr_set_tuner_bandwidth`);
  - bandwidths of 2.43 MHz or less use IF = 2.3 MHz.
- `rtlsdr_set_agc_mode(on)` writes RTL2832 demod page 0, reg 0x19, as 0x25 (on) or 0x05 (off), which toggles the DAGC bit. Init leaves it off. — [librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)

**Measured overload and IMD (HB9AJG, Aug 2013; one R820T dongle; SDR# at 2.048 MS/s)** — [HB9AJG measurements PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf); summary at [rtl-sdr.com](https://www.rtl-sdr.com/measurements-on-rtl-sdr-e4000-and-r820t-dvb-t-dongles-image-rejection-internal-signals-sensitivity-overload-1db-compression-intermodulation/)
- MDS (3 dB (S+N)/N in 500 Hz) was -134 dBm at 110 MHz. At 145 MHz, max gain (49 dB), overload ("phantom signals" appearing all at once) began at about -62 to -68 dBm for a signal within ±1 MHz of centre.
- The R820's tracking RF filter is about ±3 MHz wide. Beyond about ±10 MHz, 1 dB compression becomes the limit. Far off-tune (about +50 MHz), the overload/1 dB compression limit was about -20 dBm.
- Reducing R820 gain by 11/20/30 dB raised the overload limit by 12/20/30 dB, a nearly linear 1:1 relationship.
- Tuner AGC "is to be avoided: it degrades the overload limit by about 10 dBs, independent of the gain setting." RTL AGC at full gain changed the limit by only -1 to +2 dB. The author's conclusion was that "there is nothing to be gained from activating RTL AGC or Tuner AGC."
- IMD products appear close to the overload/compression limit, and earlier when the strong signal sits on the filter roll-off. Two -37 dBm tones at 150.0 and 150.1 MHz (outside the filter, centre 145 MHz) produced a few IM products. At -20 dBm far off-tune, the noise floor rose about 3 dB.
- Recommended practice from the same report:
  - keep in-band signals no more than about 60 dB above the noise floor;
  - in broadcast bands with many strong signals, reduce gain further, since several equal signals add voltage-wise (+6 dB each, in HB9AJG's wording);
  - when gain no longer improves SNR, back off one step;
  - signals above -40 dBm "should be attenuated by bandstop or notch filters".
- Image rejection: the R820 is a low-IF design, so it has no DC "hump" and no IQ-image problem like the E4000. Harmonics of the 28.8 MHz clock are visible up to at least 1 GHz. Aliasing occurs because there is little room for brick-wall filters. — [HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)

**ADC**
- The RTL2832U ADC is 8-bit with an ENOB of about 7. Decimation can raise effective resolution. — [rtl-sdr.com About RTL-SDR](https://www.rtl-sdr.com/about-rtl-sdr/)
- Nooelec specifies the NESDR SMArt v5 as follows — [Nooelec NESDR SMArt product page](https://www.nooelec.com/store/sdr/nesdr-smart-sdr.html), [Nooelec datasheet](https://nooelec.com/datasheets/100700):
  - R820T2 tuner, user tuner gain 0–49.6 dB, NF about 10 dB at 1 GHz;
  - maximum RF input +10 dBm, supply current about 270–330 mA.

**RTL-SDR Blog V4 (R828D) filtering** — [RTL-SDR Blog V4 release post](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)
- A triplexer splits the input into HF 0–28 MHz, VHF 28–250 MHz and UHF 250 MHz–1.766 GHz.
- The R828D open-drain pin switches a notch for broadcast AM, FM and DAB. The notch is ON when tuned outside those bands and OFF when tuned inside them, and gives about 5–10 dB attenuation.
- In a two-tone desense test with the blocker at 95 MHz, same-band isolation improved about 8 dB and out-of-band isolation improved 28–43 dB.
- VHF/UHF sensitivity is about 2–3 dB worse than the V3 on some bands. Phase noise is lower thanks to a better LDO.
- Driver code: the notch is disabled (open_d = 0x00) for freq ≤ 2.2 MHz, 85–112 MHz and 172–242 MHz. — [tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c)
- So when the V4 is tuned to FM, its FM notch is OFF. The V4 gives no protection against strong FM stations while receiving FM; it only protects other bands from FM.

### Inferences
**ADC limits**
- Theoretical 8-bit SNR is 6.02·8 + 1.76 ≈ 49.9 dB. At ENOB 7 it is about 44 dB across the full 1.14 MHz. Decimating to a 200 kHz FM channel adds about 10·log10(1.14/0.2) ≈ 7.6 dB of processing gain.
- That leaves roughly 50 dB of usable in-channel dynamic range between a strong adjacent station that fills the ADC and the noise in the wanted channel. With a strong local transmitter in the 1.14 MHz window, a weak station 400 kHz away will be desensed. Software cannot fix this. The fixes are narrower RF filtering, tuning so the strong station falls outside the IF filter, or lowering gain.

**What the current Godwit configuration does**
- Mode 0 means LNA and mixer AGC are on and the VGA is fixed at 26.5 dB. That is the configuration HB9AJG measured as about 10 dB worse for overload.
- The R82xx internal AGC detector reacts to total wideband RF power, not to the wanted channel. Without clip detection, the receiver has no feedback at all.

**Recommended gain strategy for a car (design proposal, not taken from a source)**
- Use `rtlsdr_set_tuner_gain_mode(dev, 1)` and pick from `rtlsdr_get_tuner_gains()`. Keep the RTL2832 DAGC off: it scales after the ADC, so it cannot prevent clipping and only hides headroom.
- Run a software AGC every ~100–250 ms block (about 114k–285k complex samples at 1.14 MS/s) and compute:
  - (a) the clip fraction, counting any I or Q byte equal to 0 or 255;
  - (b) the RMS of the (u8 − 127.5) samples, in dBFS.
- Gain-change rules:
  - Reduce by one or two steps immediately if the clip fraction exceeds about 1e-4, or the RMS exceeds about -10 dBFS.
  - Increase by one step only if there was no clipping for 2–3 s, the RMS is below about -25 dBFS, and the station's post-filter SNR or quieting is still improving.
  - The asymmetric attack/release and the ~10 dB hysteresis window prevent hunting.
  - Rate-limit increases to at most about one per second, so gain-switch transients (an I2C write, then a step in IQ amplitude) do not "pump".
  - FM is amplitude-insensitive after the limiter, so an amplitude step is benign as long as no clipping occurs. Sample-domain smoothing is not required, but a short mute or crossfade is cheap insurance if a click is heard.
- Optional "max SNR" heuristic, following HB9AJG: step gain down while the channel SNR does not drop.
- Hardware: for car installs near FM transmitters, an external FM band-pass helps only against out-of-band blockers such as VHF TV, DAB or pagers. An FM band-stop cannot be used, because the wanted signal is FM. Against strong in-band FM stations the only remedies are attenuation (gain reduction) and the R820's ±3 MHz tracking filter.
- Narrow the IF filter with `rtlsdr_set_tuner_bandwidth(dev, ~1.0–1.2 MHz)` to match the 1.14 MS/s rate. This adds analog rejection of adjacent stations before the ADC.

**Typical symptoms of overload**
- "Phantom" stations appearing across the band at once.
- IMD products at 2f1−f2 and 2f2−f1 (for example, two locals at 98.1 and 98.5 produce products at 97.7 and 98.9).
- A raised noise floor and loss of stereo/RDS on weak stations (desense).
- Harmonics of 28.8 MHz: 86.4 MHz is just below the band, and 115.2 MHz is just above it.

**Cannot be fixed in software**
- 8-bit ADC headroom.
- R820 front-end linearity (about 1:1 with gain only).
- The V4 notch being disabled within FM.
- Clock-harmonic birdies.
- Aliasing from the limited anti-alias filtering.

### Gaps
- No published R820T/R820T2 IIP3, SFDR or blocking-dynamic-range figure in dBm was found, apart from HB9AJG's overload numbers measured at 145 MHz, not 100 MHz. The Politecnico di Milano 2025 thesis and the "Evaluation of SDR Boards" report reportedly contain IIP3/BDR plots, but they were not read.
- Gain-change settling time (I2C write latency, then IQ settling) for the R82xx was not found in any source. Measure it on target.
- Exact behaviour and time constants of the R82xx internal LNA/mixer AGC are undocumented in the open sources read.
- I did not re-fetch librtlsdr's list of 29 discrete R820T gains (0–49.6 dB). Read it from `rtlsdr_get_tuner_gains()` at runtime.

## ADC clipping detection

### Takeaway
librtlsdr delivers raw interleaved unsigned 8-bit I/Q bytes centred at about 127.5, so clipping detection is simply counting bytes equal to 0 or 255. No library support exists; it must be done in the sample callback.

### Cited Findings
- librtlsdr passes USB bulk buffers straight to the user callback, with no processing or status. — [librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)
- The ADC is 8-bit, ENOB about 7. — [rtl-sdr.com About](https://www.rtl-sdr.com/about-rtl-sdr/)
- HB9AJG's practical heuristic: if a signal is close to the top of a 60 dB display range, you are close to overload. — [HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)

### Inferences
- Method: over a block of N bytes, compute `clip = count(b==0 || b==255) / N`. Optionally keep a histogram of the top and bottom 2 codes (0, 1, 254, 255), which gives early warning.
- A full-scale sinusoid hits the rails only at its peaks. With a multi-carrier FM band (noise-like, crest factor about 10–12 dB), an RMS around -12 dBFS already gives occasional rail hits. A clip fraction of about 1e-5 to 1e-4 is a reasonable "reduce gain" threshold, and 0 for a few seconds a reasonable "may increase" condition. These are engineering suggestions, not sourced values.
- Also track the DC offset (mean I and Q), to detect a stuck or odd dongle.
- Cost is negligible: about 2.3 MB/s of bytes at 1.14 MS/s.

### Gaps
- No published threshold recommendations specific to the RTL-SDR were found. Tune the thresholds empirically with a car antenna near a known strong transmitter.

## Frequency accuracy (ppm), drift in a vehicle, impact on FM and on audio rate

### Takeaway
TCXO dongles (NESDR SMArt v5 at ±0.5 ppm, Blog V3/V4 at 1 ppm) are accurate enough for FM without calibration: under 100 Hz at 100 MHz. Plain-crystal dongles can be 30–150 ppm off and drift tens of ppm while warming up. The same 28.8 MHz crystal clocks the ADC, so the IQ and audio sample rate carry that same ppm error relative to PipeWire's clock. That is the root cause of the audio drift problem.

### Cited Findings
- NESDR SMArt v5: TCXO stability ±0.5 ppm max; phase noise -138 dBc/Hz at 1 kHz. — [Nooelec product page](https://www.nooelec.com/store/sdr/nesdr-smart-sdr.html), [Nooelec datasheet](https://nooelec.com/datasheets/100700)
- RTL-SDR Blog V4 has a 1 ppm TCXO. A single 28.8 MHz oscillator is shared by the tuner, the RTL2832U and the HF upconverter. — [RTL-SDR Blog V4 release](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)
- Non-TCXO dongles — [rtl-sdr.com TCXO review](https://www.rtl-sdr.com/review-tcxo-modified-rtl-sdr-dongle/), [rtl-sdr.com USA TCXO review](https://www.rtl-sdr.com/review-usa-tcxo-modified-rtl-sdr-dongle/):
  - typical offsets of 30–150 ppm;
  - drift of 20+ ppm during warm-up;
  - in one measured unit, 58 → 72 ppm (14 ppm drift);
  - a chilled unit drifted 6–7 ppm in 30 minutes.
- Single-device GSM-based calibration measured about +3.6 ppm and -9 ppm errors after warm-up. Warm-up requires the dongle to be streaming. Sources disagree on the direction of drift. — [rxseger, Medium](https://medium.com/@rxseger/sdr-calibration-via-gsm-fcch-using-kalibrate-and-lte-cell-scanner-on-rtl-sdr-and-hackrf-193a7fb8a3eb)
- How `rtlsdr_set_freq_correction(dev, ppm)` works — [librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c):
  - it writes an RTL2832 sample-frequency correction (`offs = -ppm·2^24/1e6`, regs 0x3e/0x3f of page 1);
  - it scales the tuner crystal estimate by (1 + ppm/1e6) and retunes;
  - it returns -2 if the value is unchanged.
- PPM can be calibrated against DAB+ (CalibrateSDR) or GSM (kalibrate), or against an FM station's 19 kHz pilot. — [rtl-sdr.com CalibrateSDR](https://www.rtl-sdr.com/calibratesdr-calibrating-your-sdr-frequency-offset-with-dab/)

### Inferences
**Effect on FM tuning**
- RF offset = ppm × f. At 100 MHz: 0.5 ppm ≈ 50 Hz, 1 ppm ≈ 100 Hz, 50 ppm ≈ 5 kHz, 150 ppm ≈ 15 kHz.
- FM deviation is ±75 kHz with about a 200 kHz channel, so with a TCXO the offset is irrelevant.
- With a plain crystal, a 5–15 kHz offset:
  - produces a DC term in the discriminator output (MPX), which a DC-blocking high-pass filter removes;
  - shifts the signal asymmetrically in the channel filter, so peaks on one side are clipped, giving distortion and adjacent-channel leakage on one side;
  - slightly degrades stereo and RDS.
- Software mitigation: estimate the mean of the discriminator output, which equals the carrier offset in Hz × gain, and either retune (or fine-shift with an NCO) or derive the ppm and call `rtlsdr_set_freq_correction`. This is a slow loop with a time constant of seconds or more.

**Effect on audio rate**
- The ADC rate is (1 + ε) × 1.14 MS/s, so the 57 kHz output is really 57000·(1 + ε).
- Pi 5 / Dragon Q6A audio-codec crystals are presumably also tens of ppm accurate (not sourced). The relative error between the two clocks is the sum of both errors, so plan for ±200 ppm worst case.
- At 57 kHz, 100 ppm is 5.7 frames/s.
- The current 16 KB pipe holds 4096 stereo s16 frames, about 72 ms. A 100 ppm surplus fills it from empty in about 12 minutes, after which data is dropped. If the dongle is slower, `pw-play` underruns instead.

**Vehicle temperature**
- Cabin temperature can range from about -30 °C to +70 °C.
- A TCXO keeps its specified ppm over the rated range, though that range is not stated for Nooelec. A plain crystal will move tens of ppm.
- Recommendation: require a TCXO dongle (all three candidates have one). Still implement software offset estimation and the audio rate loop, because the audio-clock problem exists even with a perfect TCXO.

### Gaps
- No published data was found on TCXO stability across the automotive temperature range for these specific dongles, or on their TCXO operating temperature range.
- The Raspberry Pi 5 and Radxa Dragon Q6A audio clock accuracy was not researched. HDMI or USB audio output each have their own clocks.

## Audio clock drift: how SDR apps cope, and the cleanest design for Godwit + PipeWire

### Takeaway
Most simple SDR pipelines, including SDR++, GNU Radio audio sink and rtl_fm piped into aplay, do not compensate drift. They rely on large buffers and accept periodic underrun or overrun glitches. PipeWire can do it properly: a `pw_stream` playback stream whose input rate (57 kHz) differs from the graph rate already has a resampler, and since PipeWire 1.4.0 `pw_stream_set_rate()` lets the application steer that adaptive resampler. The cleanest design is to replace `pw-play` with a native `pw_stream` inside the helper process, holding a ring buffer whose fill level drives a slow PI controller that calls `pw_stream_set_rate()`. A ±300 ppm clamp suits TCXO dongles; plain-crystal dongles may need about ±1000 ppm.

### Cited Findings
**How other SDR apps handle it**
- **SDR++ audio sink** — [SDR++ audio_sink main.cpp](https://raw.githubusercontent.com/AlexandreRouma/SDRPlusPlus/master/sink_modules/audio_sink/src/main.cpp):
  - it sets the demodulator output rate to the sound device's rate (`_stream->setSampleRate(sampleRate)`) and has no resampler in the sink;
  - RtAudio buffer = `sampleRate / 60` frames (about 16.7 ms), with `RTAUDIO_MINIMIZE_LATENCY`;
  - on underflow the callback returns without writing;
  - the file contains no drift correction.
- **GNU Radio** — [GNU Radio discuss 2016-06](https://lists.gnu.org/archive/html/discuss-gnuradio/2016-06/msg00257.html), [Ettus lab](https://files2.ettus.com/tutorials/labs/html/text60.html), [discuss-gnuradio 2016-06/msg00267](https://lists.gnu.org/archive/html/discuss-gnuradio/2016-06/msg00267.html):
  - with two hardware clocks (SDR and sound card), underrun or overrun "is inevitable";
  - `ok_to_block=True` makes the audio device set the graph's real-time rate;
  - `False` makes the sink drop samples it cannot write, and underruns still print "aU";
  - GNU Radio does no automatic rate adaptation.
- **Gqrx** — [Gqrx mailing list thread](https://groups.google.com/g/gqrx/c/qFUJcWHRTzY):
  - crackling on a Pi was traced to PulseAudio underruns caused by bursty sample delivery;
  - the FIFO level swung between very high and empty, and about 80 ms of latency was needed;
  - the fix was to drop the low-latency setting and use PulseAudio's default latency;
  - disabling `module-suspend-on-idle` on the Pi was also suggested.
- **General technique** — [DSPRelated thread](https://www.dsprelated.com/thread/6923/synchronizing-sampling-rates-between-a-device-and-an-audio-output), [osmocom-analog commit](https://gitea.osmocom.org/cellular-infrastructure/osmocom-analog/commit/92ce6d4a428bb72692800ef32b5b80e69fef032b): use high and low water marks, or better, a continuously adjusted resampler controlled by buffer fill level.

**PipeWire tools and API**
- **pw-cat / pw-play options** — [pw-cat man page](https://docs.pipewire.org/page_man_pw-cat_1.html):
  - `--latency` (default 100 ms; node latency, not a FIFO), `--rate` (default 48000), `-q/--quality` 0–15 (default 4; resampler quality), `-P/--properties` (JSON stream properties), `--raw`, `--target`, `--media-role`;
  - pw-cat has no option for adaptive rate or drift control.
- **pw_stream_set_rate** — [PipeWire pw_stream docs](https://docs.pipewire.org/group__pw__stream.html), [stream.h](https://raw.githubusercontent.com/PipeWire/pipewire/master/src/pipewire/stream.h):
  - "Adjust the rate of the stream. When the stream is using an adaptive resampler, adjust the resampler rate";
  - returns -ENOTSUP if there is no resampler; a value ≤ 0.0 deactivates it;
  - activating it adds a small delay; RT-safe; since 1.4.0.
- **pw_time fields** — [stream.h](https://raw.githubusercontent.com/PipeWire/pipewire/master/src/pipewire/stream.h):
  - `now` (ns); `rate`; `ticks` (monotonic, usable for xrun detection); `delay` to the device, which includes resampler and filter delay unless REPORT_BUFFERED is set;
  - `queued` (bytes in queued pw_buffers); `buffered` (frames in the resampler; since 1.8.0 only with `PW_STREAM_FLAG_REPORT_BUFFERED`);
  - `size` (samples the resampler requests this quantum, since 1.1.0); `queued_buffers`; `avail_buffers`.
  - Total playback delay = buffered/stream_rate + queued/app_rate + (delay − elapsed)·rate.
- **Rate-match IO** — [libspa_sys docs](https://pipewire.pages.freedesktop.org/pipewire-rs/libspa_sys/struct.spa_io_rate_match.html):
  - `SPA_IO_RateMatch` (IO id 8) carries `spa_io_rate_match {delay, size, rate, flags}`;
  - this is the mechanism PipeWire's adapter and resampler use for rate matching between nodes;
  - `pw_stream_set_rate()` is the public way to drive it.
- **Stream flags** — [pw_stream docs](https://docs.pipewire.org/group__pw__stream.html): `PW_STREAM_FLAG_RT_PROCESS` (process in the RT thread), `PW_STREAM_FLAG_AUTOCONNECT`, `PW_STREAM_FLAG_MAP_BUFFERS`. A non-driver stream is pulled by the graph driver, i.e. the sound card clock.

### Inferences
**Why the current design glitches**
- `pw-play` reads stdin as fast as the graph pulls it, at sound-card rate × 57000/48000. The helper writes at the dongle rate. Nothing closes the loop.
- If the dongle is faster, the pipe fills and the helper drops a whole chunk at once (an audible skip) every few minutes to tens of minutes.
- If the dongle is slower, `pw-play` starves and the graph plays silence or garbage.
- USB burstiness — 15 × 256 KB transfers ≈ 115 ms of IQ each at 1.14 MS/s — makes this worse. A 72 ms pipe is smaller than one USB transfer's worth of audio, so even with perfect clocks the fill level swings by about a whole pipe.

**Recommended design (helper process, C++)**
1. Replace the `pw-play` subprocess with a native `pw_stream`:
   - format: F32 or S16 stereo, rate 57000 (or resample to 48000 in your own DSP);
   - properties: `PW_KEY_MEDIA_TYPE=Audio`, `PW_KEY_MEDIA_CATEGORY=Playback`, `PW_KEY_MEDIA_ROLE=Multimedia`, `PW_KEY_NODE_LATENCY="1024/48000"` (about 21 ms quantum);
   - flags: `AUTOCONNECT | MAP_BUFFERS | RT_PROCESS`.
2. Between the demod thread and `on_process`, use a lock-free SPSC ring buffer sized for about 500 ms, with a target fill of about 150–200 ms. That target must exceed one USB transfer burst (about 115 ms with default buffers, or less with smaller `buf_len`; see the USB section).
3. In `on_process`, copy `min(requested, available)` frames. On underrun, output zeros and count an xrun, preferably with a short fade. Never drop on overflow; instead, if fill exceeds about 90 %, skip ahead with a crossfade and count it, as a last-resort resync.
4. Control loop, run every ~100 ms:
   - error e = (fill_frames − target) / target, low-pass filtered with τ ≈ 1–2 s to reject USB burstiness;
   - ratio = 1 + Kp·e + Ki·∫e, clamped to 1 ± 300e-6 for TCXO or ±1e-3 for plain crystal;
   - call `pw_stream_set_rate(stream, ratio)`;
   - starting gains such as Kp ≈ 1e-4 and Ki ≈ 1e-6 per tick give a settling time of tens of seconds, with rate steps small enough to be inaudible (under 1 ppm per tick in steady state);
   - check that the ratio direction matches PipeWire's convention, and confirm the sign empirically: fill should converge.
5. Fallback for PipeWire < 1.4 or -ENOTSUP: do the adaptive resampling yourself. Use libsamplerate `src_process` with a variable `src_ratio` (SRC_SINC_FASTEST is adequate), speexdsp `speex_resampler_set_rate_frac` (integer ratios only, so use fine num/den such as 57000·1e6 / (48000·1e6 + adj)), or soxr with `soxr_set_io_ratio` (variable-rate mode). Output 48 kHz to match the graph and avoid a second resampler.
6. Also run the 57→48 kHz conversion in your own DSP (a fixed 19/16 rational ratio, since 57000/48000 = 1.1875 = 19/16) if you want deterministic quality instead of PipeWire's default quality 4.

**Qt HMI alternative**: Qt Multimedia's `QAudioSink` has no rate control. Keep audio in the helper and keep only control in the HMI.

**Alternatives rejected or less clean**
- Using `pw-play` with larger `--latency` only delays the glitch.
- module-loopback adds a second hop without solving the input-side clock.
- Using the dongle as a PipeWire driver (`PW_STREAM_FLAG_DRIVER`) is possible in principle, but would force the whole car audio graph onto the dongle's clock, which is unsuitable for a head unit that mixes navigation prompts and similar.

### Gaps
- Not checked: whether PipeWire's adapter enables the adaptive resampler for a pw_stream automatically when the stream rate equals the graph rate. With 57 kHz versus 48 kHz a resampler is always present, so `pw_stream_set_rate` should work. Verify that `pw_stream_set_rate` returns 0 on the AGL PipeWire version.
- The PipeWire version shipped with the target AGL release is unknown. `pw_stream_set_rate` requires ≥ 1.4.0, and `PW_STREAM_FLAG_REPORT_BUFFERED` requires ≥ 1.8.0.
- SDRangel's and Gqrx's current audio-FIFO code was not read. SDRangel is believed to use an AudioFifo with drop/zero-fill behaviour, but this is unverified.
- The exact sign convention of the `pw_stream_set_rate` ratio was not confirmed from source.

## USB streaming on Raspberry Pi 5 / Linux: buffer sizes and dropped-sample detection

### Takeaway
librtlsdr defaults to 15 × 256 KiB async buffers, which at 1.14 MS/s is about 115 ms per buffer and about 1.7 s in total. librtlsdr does not detect dropped samples. 1.14 MS/s is far below the about 2.56 MS/s drop-free ceiling, so USB bandwidth is not the issue. The issues are latency/burstiness and scheduling stalls.

### Cited Findings
- Defaults — [librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c):
  - `DEFAULT_BUF_NUMBER 15`, `DEFAULT_BUF_LENGTH (16*32*512)` = 262144 bytes;
  - `buf_len` must be a multiple of 512, or the default is used;
  - `BULK_TIMEOUT 0`; the async event loop uses a 1 s libusb timeout.
- Errors — [librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c):
  - `LIBUSB_TRANSFER_ERROR` increments `xfer_errors`; reaching `buf_num` errors, or `NO_DEVICE`, sets `dev_lost` and cancels async;
  - short transfers are passed through unflagged, and there is no dropped-sample detection;
  - on submit failure, librtlsdr logs a hint to raise `usbfs_memory_mb`.
- The RTL2832U drops samples above about 2.56 MS/s on many hosts, and up to 3.2 MS/s is unstable. Dropped samples break demodulation. — [rtl-sdr.com About](https://www.rtl-sdr.com/about-rtl-sdr/)
- On Linux, the DVB-T kernel drivers (dvb_usb_rtl28xxu) must be blacklisted. Otherwise they claim the device and enable the bias tee by default on the V4. — [RTL-SDR Blog V4 user guide](https://www.rtl-sdr.com/v4/)

### Inferences
**Buffer sizing**
- Use `rtlsdr_read_async(dev, cb, ctx, 12–16, 32768–65536)`. A 32 KiB buffer is 16384 IQ pairs, about 14 ms at 1.14 MS/s; 64 KiB is about 29 ms.
- That gives low, even latency. 12–16 buffers give about 200–450 ms of slack against scheduler stalls on the Pi 5 or Dragon Q6A.
- Keep the total under the default `usbfs_memory_mb` of 16 MB (a Linux default, not verified here); 16 × 64 KiB = 1 MiB is fine.
- Do no DSP in the USB callback. Copy into a ring and process in a worker thread. Optionally give the USB thread SCHED_FIFO priority.

**Dropped-sample detection (application level)**
- Count bytes per wall-clock second (CLOCK_MONOTONIC) and compare with 2×1.14e6. A sustained deficit means drops.
- Check that every `len` equals `buf_len`, and treat a short buffer as a discontinuity.
- Monitor callback inter-arrival gaps (more than 2× nominal means a stall).
- On a detected drop, reset the demodulator state (PLL, de-emphasis) and let the audio ring absorb it.
- The RTL2832U has no sequence counter, so drops inside the chip are invisible except through these rate checks.

**Dongle power**: the NESDR draws about 270–330 mA (plus up to 180 mA of bias tee on Blog dongles). Use a Pi 5 PSU rated for full USB current, otherwise brown-outs cause `dev_lost`.

### Gaps
- No published measurements were found for RTL-SDR drop behaviour on the Raspberry Pi 5's RP1 USB controller specifically.
- The default `usbfs_memory_mb` value and its interaction with librtlsdr were not verified from a primary source.

## librtlsdr forks: which to use

### Takeaway
Use the current osmocom rtl-sdr, v2.0.x or later, or the rtlsdrblog/rtl-sdr-blog fork. Both support the R828D-based Blog V4, which upstream merged in Aug 2023, plus the bias tee and direct sampling. The Blog fork adds the V4 HF upconverter/GPIO handling and EEPROM-forced bias tee. Older distro packages (0.6.x) and the keenerd fork lack V4 support, and the V4 then shows "no signals, wrong frequency, or corrupted signals."

### Cited Findings
- The Osmocom librtlsdr log has "add rtl-sdr blog v4 support" on 2023-08-23 by rtlsdrblog. — [osmocom cgit log](https://cgit.osmocom.org/rtl-sdr/log/src/librtlsdr.c)
- A forum post says V4 support has been in osmocom since 2.0.0 (latest 2.0.1 at the time), and that rtlsdrblog's repo tracks osmocom. This is a secondary source; verify it. — [libre.space forum](https://community.libre.space/t/upstream-submission-of-existing-rtl-sdr-patches/11388)
- V4 driver requirements and caveats — [RTL-SDR Blog V4 user guide](https://www.rtl-sdr.com/v4/):
  - V4 requires updated drivers; without them it shows no signals, wrong-frequency signals or corrupted signals;
  - the Blog drivers are backward compatible with V3 and generic dongles;
  - EEPROM strings must remain "RTLSDRBlog" / "Blog V4";
  - the bias tee is 4.5 V, up to 180 mA, controlled by `rtl_biast -b 1`, and can be forced on via `rtl_eeprom -b` (Blog drivers only);
  - direct sampling does nothing on the V4.
- The Blog fork's V4-specific code — [rtl-sdr-blog librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c), [tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c):
  - V4 detection via EEPROM;
  - a 28.8 MHz crystal for the Blog R828D;
  - HF upconversion (freq < 28.8 MHz → +28.8 MHz);
  - triplexer input selection and GPIO 5 for the upconverter switch;
  - notch control on the open-drain pin;
  - `rtlsdr_set_bias_tee` / `_gpio`;
  - on R820T/R828D, the offset-tuning call is repurposed to toggle the bias tee;
  - automatic direct sampling below 24 MHz on R820T only.
- Blog V4 is a limited-edition design because the R828D is out of production. — [RTL-SDR Blog V4 release](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)

### Inferences
- For AGL, build librtlsdr from rtlsdrblog/rtl-sdr-blog, or from osmocom ≥ 2.0.0, as a Yocto recipe, rather than relying on an older meta-oe package.
- Detect the tuner and model at runtime (`rtlsdr_get_tuner_type`, and the USB strings from `rtlsdr_get_usb_strings`) and log it.
- For FM, the V4's triplexer helps against out-of-band blockers, but its FM notch is OFF when tuned to FM. Expect V4 FM performance similar to the V3, minus about 2–3 dB of sensitivity.
- The keenerd fork (rtl_fm improvements) adds nothing needed here.

### Gaps
- I could not confirm the current osmocom release number or date from a primary release page, nor whether recent osmocom includes every Blog-fork V4 change (e.g. GPIO 5 upconverter switching on newer batches).
- The keenerd fork's current state was not researched.
