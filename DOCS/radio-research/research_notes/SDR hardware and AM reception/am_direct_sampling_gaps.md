# AM (MW 530–1710 kHz) on the NESDR SMArt v5 by Direct Sampling, and Remaining AM Receiver Design Gaps

Correction to the earlier study (`research_notes/SDR FM car radio receiver design/am_on_rtlsdr.md`): it said a stock R820T-class dongle such as the NESDR SMArt v5 "cannot tune MW without an external upconverter". That is wrong. Nooelec's own datasheet documents a built-in diplexer that routes 0.1–30 MHz straight to the RTL2832U Q-branch ADC input, and the RTL-SDR Blog driver fork switches R820T dongles into Q-branch direct sampling automatically below 24 MHz.

## NESDR SMArt v5 direct sampling: input path, filtering, sensitivity, aliasing, user reports, comparison with Blog V3/V4

### Takeaway
The SMArt v5 has a single 50 Ω SMA input feeding a diplexer. The LF/MF/HF leg (labelled 0.1–30 MHz) goes directly to the RTL2832 Q-branch ADC; the VHF/UHF leg goes to the tuner (marked R860 on the datasheet). No modification is needed, and the rated direct-sampling range is 0.1–25 MHz. Nooelec publishes no MW sensitivity, MDS or dynamic-range figure. The diplexer is the only filtering, so there is no MW preselection and no anti-alias filter below 30 MHz. With the ADC clocked at 28.8 MHz, the 27 MHz CB band folds onto upper MW, and the bottom of the 10 m band folds onto lower MW. In performance it should be grouped with RTL-SDR Blog V3 direct sampling (about -109 dBm MDS at 1 MHz per the V4 datasheet), not with the V4 upconverter (about -134 dBm). No measured SMArt v5 MW data was found.

### Cited Findings
- Datasheet features: "0.1 to 25MHz direct sampling tuning range", "25 to 1750MHz quadrature sampling tuning range", 0–49.6 dB tuner gain, "7 Bit ADC", up to 3.2 MS/s, 50 Ω SMA input, 0.5 ppm TCXO, ESD protection 23 kV (IEC 61000-4), supply current 270/300/330 mA (min/typ/max), maximum RF input +10 dBm. — [Nooelec NESDR SMArt v5 datasheet, rev 1, 2022-10-04](http://www.nooelec.com/datasheets/100700)
- Datasheet simplified schematic: SMA → diplexer. The "LF + MF + HF" leg is labelled "0.1 to 30MHz Direct Sampling (Q Branch)" and goes directly to the RTL2832. The "VHF + UHF" leg goes through the "R860" RF tuner as "0.25 to 1.75GHz Quadrature Sampling". The tuner and the RTL2832 share a single 28.8 MHz oscillator. There is one shared SMA input, so MW and FM come in on the same connector and no internal switching is needed. — [Nooelec datasheet p.1](http://www.nooelec.com/datasheets/100700)
- The electrical spec table quotes noise figure only "@ 1 GHz" (10 dB typ) and return loss only "@ 1 GHz" (-8/-12/-16 dB). Instantaneous bandwidth is 0.25 / 2.4 / 3.2 MHz (min/typ/max). Phase noise is ≤ -138 dBc/Hz at 1 kHz, -150 at 10 kHz and -152 at 100 kHz. There are no HF/MW sensitivity, NF or IP3 figures. — [Nooelec datasheet p.2](http://www.nooelec.com/datasheets/100700)
- Nooelec support advised enabling direct sampling by adding `direct_samp=Q` to the device string, for example in GQRX. A user reported that direct sampling bypasses the tuner's LNA and "pretty much guarantees poor results" on MF/HF. — [Gqrx group thread: "Cannot tune below 24MHz using Nooelec NESDR 5 on Raspberry Pi 4"](https://groups.google.com/g/gqrx/c/RBtRRHDVZTs) (summarised via search result; the thread was not read in full)
- An Adafruit project (Fruit Jam AM/FM/weather radio) uses the SMArt v5 and found the bundled telescopic antenna gave the best reception. The fetched page did not document direct-sampling parameters or MW reception quality. — [Adafruit Learn](https://learn.adafruit.com/usb-host-sdr-for-am-fm-weather-radio-on-fruit-jam?view=all)
- Comparison point, RTL-SDR Blog V3 direct sampling: covers about 500 kHz–28.8 MHz with no on-board filtering, and newer batches deliberately attenuate below 2 MHz, which makes MW worse but improves performance above 2 MHz. "It can still be overloaded by strong AM broadcast stations." — [RTL-SDR Blog V3 user guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/); [rtl-sdr.com direct sampling article](https://www.rtl-sdr.com/rtl-sdr-direct-sampling-mode/)
- Comparison point, V4 versus V3 MDS from the V4 datasheet plot: about -134 dBm (V4) versus about -109 dBm (V3 direct sampling) at 1 MHz. — [RTL-SDR Blog V4 datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf) (as recorded in the earlier study)
- Direct-sampling images: with a 28.8 MHz ADC clock, tuning a station at 1.0 MHz also hears signals at 27.8 MHz. — [rtl-sdr.com direct sampling article](https://www.rtl-sdr.com/rtl-sdr-direct-sampling-mode/)

### Inferences
- **Alias map (computed from the 28.8 MHz clock).** A signal at f appears at |f − n·28.8 MHz|.
  - MW 530–1710 kHz shares bins with 27.09–28.27 MHz. That range covers the whole 26.965–27.405 MHz CB band, which lands on 1395–1835 kHz, and the bottom of the 10 m amateur band (28.0–28.27 MHz), which lands on 530–800 kHz.
  - FM broadcast folds too: 88.0–88.1 MHz minus 3×28.8 MHz (86.4 MHz) lands on 1.6–1.7 MHz. The diplexer should attenuate it, but its rejection is unspecified.
  - In a vehicle with CB or 10 m transmitters nearby this is a real problem. A ~2 MHz low-pass (or a 500–1800 kHz band-pass) ahead of the SMA is effectively mandatory for broadcast-grade AM.
- **No preselection.** The whole 0.1–30 MHz span reaches an 8-bit ADC (marketed as "7 bit"). Total band power from strong MW locals plus HF sets the clipping point, so dynamic range is limited by the ADC, not by thermal noise. Without a front-end attenuator or filter, expect overload near 50 kW stations, as reported for the V3.
- **Sensitivity.** Direct sampling bypasses the R820T LNA and gain stages, so the tuner gain range (0–49.6 dB) does nothing in this mode. MW sensitivity therefore depends on the antenna and any external preamp or active antenna. For a car whip, a high-impedance active buffer (see the earlier study) is even more important with the SMArt v5 than with the V4.
- **Practical ranking for MW:** V4 (upconverter, triplexer, about 25 dB better MDS at 1 MHz) > SMArt v5 or V3 direct sampling with an external MW band-pass and active antenna > SMArt v5 direct sampling with a bare whip. The SMArt v5 does allow a zero-hardware-change AM prototype for strong local stations.

### Gaps
- No published MDS, noise figure, IP3 or dynamic range for SMArt v5 direct sampling at MW. Nooelec specifies NF and return loss only at 1 GHz. A bench measurement (signal generator at 1 MHz into SMA, measuring SINAD/MDS) is the only way to get a number.
- Diplexer crossover frequency, order and stopband rejection are not published, and it is unknown whether the LF leg attenuates below 2 MHz the way new V3 batches do.
- The Nooelec support FAQ page (support.nooelec.com) returned HTTP 403 and could not be read directly.
- No rigorous user reports of SMArt v5 MW reception quality, for example station counts or day/night DX logs, were found.

## librtlsdr direct-sampling mechanics (osmocom and rtl-sdr-blog forks)

### Takeaway
`rtlsdr_set_direct_sampling(dev, 2)` puts the R820T into standby, disables Zero-IF and spectrum inversion, enables only the in-phase ADC with the I/Q ADC inputs swapped (so the Q pin is sampled), and then retunes. In this mode the "centre frequency" is written to the RTL2832's digital IF/NCO register (22-bit, about 6.87 Hz steps at 28.8 MHz), so the chip's own DDC produces complex baseband from the real ADC stream. Turning it off re-runs the full tuner init. The rtl-sdr-blog fork switches to Q-branch automatically below 24 MHz for R820T dongles (which includes the SMArt v5), but not for the V4. The osmocom code read here contains no such automatic switch.

### Cited Findings
- Osmocom `rtlsdr_set_direct_sampling(dev, on)` when enabling:
  - Turns the tuner off (through the I2C repeater).
  - Disables Zero-IF mode and spectrum inversion.
  - "only enable In-phase ADC input".
  - Selects the input with `on > 1` ("swap I and Q ADC, this allows to select between two inputs").
  - Stores the mode and prints "Enabled direct sampling mode, input %i".
  - When disabling, re-initialises the tuner. For R820T/R828D it sets the IF to the R82XX IF and re-enables spectrum inversion, then resets the ADC datapath.
  - Both paths finish by calling `rtlsdr_set_center_freq` with the current frequency.
  - — [osmocom librtlsdr.c](https://raw.githubusercontent.com/osmocom/rtl-sdr/master/src/librtlsdr.c)
- Osmocom `rtlsdr_set_center_freq` in direct-sampling mode bypasses the tuner and passes the requested frequency to `rtlsdr_set_if_freq`. That function converts it to a signed value scaled by 2^22 / crystal frequency, negates it, and writes it to page 1 registers 0x19–0x1b (top byte masked to 6 bits). — [osmocom librtlsdr.c](https://raw.githubusercontent.com/osmocom/rtl-sdr/master/src/librtlsdr.c)
- Osmocom `rtlsdr_set_offset_tuning` returns -2 for R820T/R828D and -3 if direct sampling is active. — [osmocom librtlsdr.c](https://raw.githubusercontent.com/osmocom/rtl-sdr/master/src/librtlsdr.c)
- Osmocom `rtlsdr_set_sample_rate` rejects rates ≤ 225 kHz, rates in (300 kHz, 900 kHz], and rates > 3.2 MHz. The legal ranges are therefore 225.001–300 kHz and 900.001 kHz–3.2 MHz. — [osmocom librtlsdr.c](https://raw.githubusercontent.com/osmocom/rtl-sdr/master/src/librtlsdr.c)
- If no supported tuner is found, `rtlsdr_open` prints "No supported tuner found" and enables direct sampling mode 1. — [osmocom librtlsdr.c](https://raw.githubusercontent.com/osmocom/rtl-sdr/master/src/librtlsdr.c)
- rtl-sdr-blog fork auto-switching, inside `rtlsdr_set_center_freq` (only when the user has not forced a mode, `direct_sampling_mode == 0`): `dev->direct_sampling = (freq < 24000000 && dev->tuner_type == RTLSDR_TUNER_R820T && !rtlsdr_check_dongle_model(dev, "RTLSDRBlog", "Blog V4L")) ? 2 : 0;`. The threshold is 24 MHz, Q-branch (2) is selected, R828D (V4) is excluded, and the V4 Lite is excluded. — [rtl-sdr-blog librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)
- rtl-sdr-blog fork entering direct sampling: it calls `dev->tuner->exit(dev)`, which runs `r82xx_standby`, so the R820T is put into standby rather than left running. It then writes these registers:
  - 0xb1 = 0x1a (Zero-IF off)
  - 0x15 = 0x00 (inversion off)
  - 0x08 = 0x4d (I-ADC only)
  - 0x06 = 0x90 for Q-branch or 0x80 for I-branch (ADC swap)

  Leaving direct sampling calls `dev->tuner->init(dev)`, sets IF = R82XX_IF_FREQ and 0x15 = 0x01, and 0x06 = 0x80. On R820T/R828D, offset tuning is repurposed as the bias-tee toggle. — [rtl-sdr-blog librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)
- `rtlsdr_set_agc_mode` controls the RTL2832's internal digital AGC, separate from tuner gain: "AGC is for RTL chip, auto gain is for the tuner". librtlsdr has had it off by default since 2012. A 2013 patch combined direct sampling with RTL AGC in rtl_sdr. — [osmocom-sdr list, 2012](https://lists.osmocom.org/pipermail/osmocom-sdr/2012-July/000307.html); [osmocom-sdr patch, Sept 2013](https://lists.osmocom.org/pipermail/osmocom-sdr/2013-September/000966.html)
- Gqrx/gr-osmosdr device-string convention: `direct_samp=0` disables, 1 selects the I channel, 2 selects the Q channel. Some driver builds use 3. — [rtl-sdr.com direct sampling article](https://www.rtl-sdr.com/rtl-sdr-direct-sampling-mode/)

### Inferences
- **Tuning resolution.** 28.8 MHz / 2^22 ≈ 6.87 Hz per NCO step, which is fine for 10 kHz channels. Applying the 0.5 ppm TCXO correction through `rtlsdr_set_freq_correction` still matters for synchronous AM: 0.5 ppm at 1 MHz is only 0.5 Hz, which a SAM PLL easily absorbs.
- **Sample rate.** The ADC still runs at 28.8 MS/s, and the RTL2832 decimates it to the chosen rate. For MW, 2.4 MS/s centred near 1.12 MHz covers 530–1710 kHz in one capture (DDC output is complex, so ±1.2 MHz). Alternatively, use about 1.024–1.2 MS/s around the wanted part of the band. Rates between 300 and 900 kHz are illegal.
- **No tuner DC spike, but a different image risk.** The tuner is off, so the R820T's IF-related artifacts are absent. The weaknesses are the alias map from the previous section and the fact that RF gain control is effectively gone: only the RTL2832 digital AGC remains, and it does not prevent ADC clipping.
- **Mode-switch latency (AM ↔ FM).** Going into direct sampling is cheap: a few register writes plus R820T standby. Coming out means a full `r82xx_init`, a burst of I2C writes over USB, then a retune and PLL lock. Expect a cost similar to opening the device, plausibly tens of ms, plus the time to flush stale buffers from the async read queue (with default 15 × 256 KiB buffers at 2.4 MS/s, about 0.8 s of stale data if not cancelled or flushed). Unmeasured, but the design should cancel `rtlsdr_read_async`, switch mode, reset the buffer (`rtlsdr_reset_buffer`), then restart.
- **Driver choice for Godwit.** With the rtl-sdr-blog fork, `radio/src/main.cpp` can simply tune below 24 MHz and the library enters Q-branch automatically. For portability across osmocom builds, call `rtlsdr_set_direct_sampling(dev, 2)` explicitly before tuning MW and `(dev, 0)` before tuning FM. Note the blog fork's auto path only runs when `direct_sampling_mode == 0`.
- **R860 labelling.** The datasheet shows an "R860" tuner. The auto-switch keys on `RTLSDR_TUNER_R820T`. That the R860 reports as R820T is consistent with the v5 being sold as R820T2-based, but this was not verified from source.

### Gaps
- No measured retune or mode-switch latency for direct sampling ↔ quadrature on any RTL2832U.
- No osmocom changelog was checked for whether upstream added automatic direct sampling after V4 support was merged. The fetched osmocom function showed none.
- Whether the RTL2832 digital AGC helps or hurts MW direct sampling was not tested in any source found.

## NRSC-1-C pre-emphasis/de-emphasis and NRSC-2-C emission limits

### Takeaway
NRSC-1-C (April 2018) defines the "modified 75 µs" AM pre-emphasis as one zero at 2122 Hz (τ = 75 µs) and one pole at 8700 Hz (τ ≈ 18.3 µs), giving +10.00 dB at 10 kHz. The receiver de-emphasis is the exact complement: one pole at 2122 Hz and one zero at 8700 Hz, giving -10.00 dB at 10 kHz. It is specified as the receiver's net RF+IF+AF response, flat within +1.5/-3 dB (referenced to 400 Hz) from 50 Hz to the receiver's -3 dB point, and ≤ -20 dB at 10 kHz is "recommended". Transmitters must cut audio to -15 dB at 10 kHz and -30 dB at 10.5 kHz. NRSC-2-C limits RF emissions to 0 dBc within ±10.2 kHz, -25 dBc at 10.2–20 kHz, and -35 dBc at 20–30 kHz.

### Cited Findings
- NRSC-1-C §5.2: "The recommended preemphasis curve is a single zero curve with a break frequency at 2122 Hz. It is similar to the 75 µS curve used for FM broadcasting. To reduce the peak boost at high frequencies, a single pole with a break frequency of 8700 Hz is employed." The curve applies to audio up to 10 kHz. Stations shall broadcast as close as possible to it "without exceeding it". — [NRSC-1-C (April 2018)](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-1-C Table 1, pre-emphasis magnitude (dB) / phase (°):

  | Hz | dB | ° | Hz | dB | ° |
  |---|---|---|---|---|---|
  | 50 | 0.00 | 1.0 | 5000 | 6.92 | 37.1 |
  | 100 | 0.01 | 2.0 | 5500 | 7.41 | 36.6 |
  | 400 | 0.14 | 8.0 | 6000 | 7.85 | 35.9 |
  | 700 | 0.42 | 13.7 | 6500 | 8.24 | 35.2 |
  | 1000 | 0.81 | 18.7 | 7000 | 8.58 | 34.3 |
  | 1500 | 1.63 | 25.5 | 7500 | 8.89 | 33.4 |
  | 2000 | 2.54 | 30.4 | 8000 | 9.16 | 32.5 |
  | 2500 | 3.44 | 33.6 | 8500 | 9.41 | 31.6 |
  | 3000 | 4.28 | 35.7 | 9000 | 9.62 | 30.8 |
  | 3500 | 5.05 | 36.9 | 9500 | 9.82 | 29.9 |
  | 4000 | 5.75 | 37.4 | 10000 | 10.00 | 29.0 |
  | 4500 | 6.37 | 37.4 | | | |

  The de-emphasis table (Table 2) is identical with signs negated. — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-1-C §6.2: "AM receivers shall complement the recommended transmission preemphasis... The NRSC deemphasis curve is characterized by a single pole at 2122 Hz and a single zero at 8700 Hz. It is the precise complement." The net receiver response is "the combined RF, IF, and AF audio response". The pair yields a system "essentially flat to nearly 10 kHz". — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-1-C §6.3 receiver compliance:
  - Audio bandwidth is stated as 50 Hz to N Hz (the final -3 dB point).
  - Flatness is +1.5/-3 dB from 50 Hz to N Hz, referenced to 400 Hz.
  - Figure 3 marks 10 kHz at "≤ -20 dB recommended".
  - A receiver is compliant "if it meets the flatness specification across its audio bandwidth".
  - Dual-bandwidth receivers are measured in both positions.
  - — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-1-C §6.4: a notch filter for first-adjacent carriers is optional but "recommended". It should have as high a Q as practical, suppress the interfering carriers adequately, and not unduly degrade the wanted bandwidth. — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-1-C §7.2 transmit audio stopband, measured in 300 Hz RBW relative to 1 dB above a 200 Hz tone at 90% negative modulation:
  - -15 dB at 10 kHz, falling smoothly to -30 dB at 10.5 kHz
  - -30 dB from 10.5 to 11.0 kHz
  - -40 dB at 11.0 kHz, falling to -50 dB at 15 kHz
  - below -50 dB above 15 kHz
  - — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-1-C §3 and §7.3: with ±10 kHz occupancy and 10 kHz spacing there is "implicit spectral overlap between first adjacent stations". A 2006 study found "the vast majority of receivers have bandwidths less than 5 kHz". Hybrid AM IBOC stations limit analog audio to 5, 8 (standard MA-1) or 9.4 kHz (reduced digital bandwidth "modified MA-1") modes per NRSC-5-D. — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-1-C §8 informative receiver test conditions:
  - 10,000 µV RMS carrier; 1000 Hz tone at 30% modulation (-10.5 dB re 100%); test channels 600/1000/1500 kHz.
  - Pre-test conditions: S/N ≥ 30 dB and THD < 5%.
  - Response swept 30 Hz–10 kHz and compared with Figure 2; repeated at 3,000 and 30,000 µV.
  - — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- NRSC-2-C Table 1, RF mask (peak hold, 300 Hz RBW, ≥ 10 min, carrier reference):
  - ±0–10.2 kHz: 0 dB
  - 10.2–20 kHz: ≥ 25 dB
  - 20–30 kHz: ≥ 35 dB
  - 30–60 kHz: ≥ 5 + 1 dB/kHz
  - 60–75 kHz: ≥ 65 dB
  - above 75 kHz: ≥ 80 dB, or 43 + 10·log10(P_W) for 158–5000 W carriers, and 65 dB for under 158 W
  - The nominal occupied bandwidth is 20 kHz. Hybrid AM IBOC is governed by the more stringent NRSC-5 mask (47 CFR §73.404(a)); analog by 47 CFR §73.44.
  - — [NRSC-2-C (April 2018)](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-2-c.pdf)
- NRSC-2-C §5: a "splatter monitor" receiver must "employ the NRSC-1-C deemphasis characteristic" and detect both in-phase and quadrature components in the stopband. — [NRSC-2-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-2-c.pdf)

### Inferences
- **De-emphasis transfer function:** H(s) = (1 + s·τz) / (1 + s·τp), with τp = 1/(2π·2122) = 75.0 µs and τz = 1/(2π·8700) = 18.29 µs. Check: |H(10 kHz)| = √(1 + 1.149²) / √(1 + 4.713²) = 1.524 / 4.818 = 0.316, or -10.0 dB, which matches Table 2. Implement it as a first-order IIR via bilinear transform, prewarped at 2122 Hz, at the 48 kHz audio rate.
- **Interaction with IF bandwidth.** The standard defines de-emphasis as the net RF+IF+AF response. A DSP receiver with a flat FIR channel filter should apply the full curve only at its widest (10 kHz audio) setting. At 5 kHz or narrower, the channel filter already exceeds the high-frequency cut, and adding full de-emphasis makes audio dull. A reasonable approach is full NRSC curve at ≥ 9 kHz audio, partial or none at ≤ 5 kHz.
- **Receiver bandwidth choice from NRSC-2.** Emissions are 0 dBc to ±10.2 kHz and only -25 dBc out to ±20 kHz, so a first-adjacent station's sidebands overlap the wanted channel's 5–10 kHz audio region by design. That justifies selectable audio widths (for example 10 / 8 / 6 / 4.5 / 3 kHz) and a fixed or auto 10 kHz heterodyne notch.
- **Audio low-pass.** For the widest setting, put the audio LPF corner just below 10 kHz, steep enough to reach about -20 dB at 10 kHz to match Figure 3's recommendation and suppress the first-adjacent carrier beat.

### Gaps
- The NRSC-5-D AM hybrid RF mask (digital subcarrier levels in dBc per 300 Hz) was not retrieved; it would quantify digital-sideband power in adjacent channels.

## AM receiver DSP details: SAM PLL, AGC, sideband selection, bandwidth steps, notch, noise blanker, CPU cost

### Takeaway
SDRangel remains the best open-source SAM reference. It uses a second-order PLL with ζ = 0.707, a 10 ms frequency-stability lock window, and sync-AM AGC windows of fs/4 and fs/8 (about 250 ms / 125 ms at 48 kHz). As of 18 Nov 2026, SDR++ still has no synchronous AM mode. Its radio module ships only am, cw, dsb, lsb, nfm, raw, usb and wfm demodulators. No primary source gave measured PLL behaviour under skywave fading, noise-blanker parameters, or ARM CPU costs; those below are engineering estimates.

### Cited Findings
- SDRangel `amdemodsink.cpp` parameters:
  - PLL `computeCoefficients(0.05, 0.707, 1000)`; PLL post-filter `m_pllFilt.create(101, audioSampleRate, 200.0)`.
  - Volume AGC `resizeNew(audioSampleRate/4, 0.003)` with the PLL on, and `audioSampleRate/10` with it off.
  - Sync-AM AGC `m_syncAMAGC(12000, 0.1, 1e-2)`, resized to `(sampleRate/4, sampleRate/8, 0.1)`.
  - DSB filter `fftfilt(2·rfBW/fs, 2048)`; SSB filter (for SAM-USB/LSB) `fftfilt(0, rfBW/fs, 1024)`.
  - Channel interpolator 16 taps at rfBW/2.2; audio 301-tap LPF at afBW/2 or 300 Hz–afBW/2 band-pass; audio rate 48 kHz.
  - No de-emphasis and no noise blanker in this file.
  - — [SDRangel amdemodsink.cpp](https://raw.githubusercontent.com/f4exb/sdrangel/master/plugins/channelrx/demodam/amdemodsink.cpp)
- SDRangel `PhaseLockComplex::computeCoefficients(wn, zeta, K)`:
  - wn is the natural frequency, zeta the damping, K the loop gain, with t1 = K/wn², t2 = 2ζ/wn − 1/K.
  - Lock detection for order 1: the frequency step between samples must stay within ±0.01, and lock time = sampleRate/100 (10 ms).
  - For higher PSK orders, a ±6 Hz window is used.
  - — [SDRangel phaselockcomplex.cpp](https://raw.githubusercontent.com/f4exb/sdrangel/master/sdrbase/dsp/phaselockcomplex.cpp)
- SDR++ GitHub, 21 Nov 2026: `decoder_modules/radio/src/demodulators/` contains am.h, cw.h, dsb.h, lsb.h, nfm.h, raw.h, usb.h, wfm.h, so there is no SAM. The latest commits are dated 2026-11-18. The only "SAM" in the code is `MODE_SAM` in the rigctl protocol enum. The core AM demod is envelope-only with CARRIER/AUDIO AGC (per the earlier study). — GitHub API listing of [SDRPlusPlus repo](https://github.com/AlexandreRouma/SDRPlusPlus/tree/master/decoder_modules/radio/src/demodulators) (retrieved via `gh api`)
- Sync AM is supported in SDR Console, SDRuno and SDR Dx (wiki list, may be incomplete). SDR# added synchronous AM in 2014 after a user request. — [RadioReference wiki: Synchronous Detection](https://wiki.radioreference.com/index.php/Synchronous_Detection); [SWLing Post](https://swling.com/blog/wp-json/wp/v2/posts/10072)
- NRSC-1-C recommends a high-Q first-adjacent carrier notch, as covered in the previous section. — [NRSC-1-C §6.4](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)

### Inferences (engineering estimates, not sourced measurements)
- **SAM PLL loop bandwidth.** If SDRangel's wn = 0.05 is in rad/sample at a 48 kHz rate, the natural frequency is about 380 Hz (0.05/2π × 48k). That is wide enough to track carrier phase through fast skywave selective fading, but it also follows noise on weak carriers. Broadcast-oriented hardware SAM detectors are commonly described with loop bandwidths of tens of Hz up to a few hundred Hz. A practical design is a two-speed PLL: wide (about 200–400 Hz) for acquisition, narrowing to about 20–50 Hz once lock is declared (frequency error steady for about 10 ms, as in SDRangel).
- **Lock loss.** On a deep fade, freeze the NCO frequency (coast) rather than letting the loop chase noise, and fall back to envelope detection after about 100–300 ms without lock.
- **Selectable-sideband SAM.** After carrier lock, take USB or LSB with a complex band-pass (SDRangel uses an FFT filter of width rfBW). Add a "both, with auto-select by lower adjacent-channel energy" option.
- **Carrier AGC.** Derive gain from the post-PLL I-channel DC (the carrier) using:
  - attack about 2–10 ms
  - hang about 0.5–1 s
  - decay about 0.5–2 s, so that 10 Hz–1 kHz fading flutter is not reproduced as audio pumping.

  SDRangel's 125–250 ms windows are at the fast end.
- **Audio bandwidth steps.** 10 / 8 / 6 / 4.5 / 3 kHz, aligned with NRSC-1-C's 10 kHz maximum and the 5/8/9.4 kHz hybrid IBOC analog modes.
- **Heterodyne notch.** On a 10 kHz grid, first-adjacent carriers beat at about 10 kHz ± drift. Use an adaptive LMS notch or a carrier-tracking IIR notch (Q about 30–100) in the audio. Twice-removed (20 kHz) beats are outside the audio passband.
- **Noise blanker.** Run on the wideband IQ before the channel filter:
  - per-sample |x|² compared with a running mean (time constant about 10–50 ms)
  - threshold about 6–12 dB
  - blank or interpolate about 10–50 µs windows

  This suits ignition and LED-driver impulses. The 2.4 MS/s direct-sampling stream gives adequate time resolution.
- **CPU on ARM (order-of-magnitude estimates).**
  - One channel: frequency shift plus decimation from 2.4 MS/s to 48 kHz (polyphase FIR, about 50 taps per output stage) costs roughly 10–20 MMAC/s. PLL, filters and audio at 48 kHz add under 5 MMAC/s. That is a few percent of one Cortex-A76 core.
  - Full-band channeliser: 2.4 MS/s with 240 bins of 10 kHz, a polyphase FFT bank, and 118 envelope demods plus level meters costs about 2.4 MS/s × (taps/bin + log2(240) complex ops), roughly 100–200 MFLOP/s, plausibly 10–25% of one A76 core with NEON (VOLK/FFTW).

### Gaps
- No primary source was found for measured SAM loop-bandwidth trade-offs on fading MW skywave, or for commercial broadcast-receiver AGC time constants.
- No benchmark was found for AM channelisers on Pi 5 or Dragon Q6A.
- Gqrx, CuteSDR and SDR# SAM sources were not reviewed.

## AM NRSC-5 IBOC hybrid: effect on analog reception and nrsc5 AM decoding

### Takeaway
Hybrid AM IBOC (MA1) occupies about ±15 kHz, so its digital sidebands sit on both first-adjacent channels. On an analog receiver they sound like white noise, which is the main nighttime complaint, and many AM stations that adopted hybrid HD have since turned it off. When the hybrid station itself is the wanted signal, it restricts its own analog audio to 5, 8 or 9.4 kHz, so a receiver gains nothing from a 10 kHz setting on such a station. nrsc5 has an `--am` mode and RTL-SDR direct-sampling flags (`-D 1|2`) and works at an internal AM IQ rate of 46,512 S/s. Its cu8 file format runs at 1,488,375 S/s, and the README's file rates suggest a single device rate for both bands. No source confirmed successful AM IBOC decoding from a direct-sampling RTL-SDR.

### Cited Findings
- Hybrid MA1 "uses 30 kHz of bandwidth (±15 kHz)" and "overlaps adjacent channels on both sides of the station's assigned channel". "Some nighttime listeners have expressed concern this design harms reception of adjacent channels". On a conventional receiver tuned to an adjacent channel, the digital signal "sounds like white noise".
  - All-digital MA3: enhanced mode is 40.2 kbit/s in 20 kHz; core-only is 20.2 kbit/s in 10 kHz.
  - Audio codec is the NRSC-5 audio codec with SBR. Hybrid AM carries about 40–60 kbit/s, and most stations run a robust 40 kbit/s mode.
  - — [Wikipedia: NRSC-5 IBOC](https://en.wikipedia.org/wiki/HD_Radio)
- Hybrid IBOC analog audio bandwidth modes: 5 kHz and 8 kHz (standard MA-1) and 9.4 kHz (reduced-digital "modified MA-1"). — [NRSC-1-C §3, §7.3](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)
- Radio World reported concerns that skywave HD signals would cause problems for the secondary (nighttime) analog service of first- and second-adjacent stations. All-digital reduced-bandwidth operation narrows occupancy from 30 kHz (hybrid MA-1) to 10 kHz (MA-3). A NAB-related report said only about 200 U.S. stations used hybrid AM IBOC and many had turned it off over adjacent-channel interference. That figure is dated, and current counts were not verified. — [Radio World](https://www.radioworld.com/?p=84235); [Radio World: NAB tests address digital AM concerns](https://www.radioworld.com/news-and-business/nab-tests-address-digital-am-concerns) (summarised via search results; not read in full)
- nrsc5 README options: `--am` receive AM (default FM); `-D` direct sampling (1 = I-ADC, 2 = Q-ADC); `-T` bias tee; `-H` rtl_tcp host. IQ file rates: cu8 at 1,488,375 S/s; cs16/cf32 at 46,512 S/s for AM and 744,188 S/s for FM. — [theori-io/nrsc5](https://github.com/theori-io/nrsc5)
- Jeff Geerling noted that decoding AM IBOC is harder than FM. His "AM" success was actually an FM SPS 3 simulcast of an AM station, not AM IBOC. — [jeffgeerling.com (2019)](https://www.jeffgeerling.com/blog/2019/hospital-stay-and-mlb-blackouts-led-me-rtl-sdr-radio/)

### Inferences
- **Device sample rate.** nrsc5 appears to run the RTL-SDR at 1,488,375 S/s (the cu8 rate) for both modes and decimate internally: by 32 to 46,512 S/s for AM and by 2 to 744,188 S/s for FM. 1,488,375 S/s is within the legal 0.9–3.2 MS/s range. A Godwit full-MW capture at 2.4 MS/s would need resampling (2.4 M → 46,512 is a ratio of about 51.6) to feed nrsc5's AM path from a shared stream. Using nrsc5 directly would give it exclusive device control.
- **Wanted-signal considerations.** Hybrid sidebands at ±5–15 kHz from a strong first-adjacent station appear as wideband hiss inside a 10 kHz analog passband. Narrow bandwidth (≤ 5 kHz) or SAM on the opposite sideband is the receiver-side mitigation. That supports making SAM sideband selection and auto-bandwidth first-class features.
- **Sensitivity.** AM IBOC requires a much cleaner signal than analog. Given the SMArt v5's direct-sampling sensitivity and 8-bit dynamic range, expect HD lock only on strong local hybrid stations, if at all. Treat AM IBOC as optional and lower priority than FM IBOC.

### Gaps
- No source confirmed nrsc5 `--am` decoding from an RTL-SDR in direct-sampling mode or through an upconverter, and none gave a required SNR.
- No current (2026) count of U.S. hybrid or all-digital AM IBOC stations was found.
- The NRSC-5-D AM digital subcarrier levels (dBc) and exact sideband offsets (primary, secondary, tertiary) were not retrieved.
