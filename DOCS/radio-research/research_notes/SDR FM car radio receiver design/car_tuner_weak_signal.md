# How commercial car FM tuners handle weak and impaired signals (stereo blend, soft mute, high cut, multipath/USN detection, noise blanking)

Primary sources used (text extracted from the PDFs, not summaries):
- Skyworks/Silicon Labs **AN332** "Si47xx Programming Guide" (rev. dated Oct 29 2024): https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf
- Philips/NXP **AN02106** "Car Radio Integrated Signal Processor TEF6890H/92H/94H" application note: https://www.nxp.com/docs/en/application-note/AN02106.pdf
- NXP **TEF668X product data sheet** Rev. 4, 15 Jan 2014 (copy hosted on radiokot.ru): https://radiokot.ru/forum/download/file.php?id=381990
- PE5PVB **TEF6686_ESP32** firmware source (driver code that sets the TEF668x weak-signal parameters): https://github.com/PE5PVB/TEF6686_ESP32 (files src/TEF668X.cpp, src/Tuner_Drv_Lithio.cpp, src/Tuner_Drv_Lithio.h)

## Q1. Which signal quality indicators car tuners derive, and how

### Takeaway
Car tuners do not use a single "SNR" number. They derive separate indicators — RF level (RSSI), wideband AM on the level/envelope (WAM = multipath), ultrasonic noise in the MPX above ~100 kHz (USN = noise + adjacent channel + multipath), frequency offset, modulation (deviation) and pilot presence — and drive each weak-signal action from the worst ("max-of") of several of them, each with its own time constant.

### Cited Findings
- TEF668X lists its FM quality detectors as: RSSI/field strength (level), frequency offset, "Multipath and adjacent channel detection by USN detection", "Multipath detection by WAM detection", frequency deviation by modulation detection, and stereo pilot detection. — [NXP TEF668X data sheet Rev.4](https://radiokot.ru/forum/download/file.php?id=381990)
- TEF668X weak-signal handling: soft mute controlled by level, USN and WAM; HighCut/LowCut controlled by level, USN, WAM and modulation; stereo blend controlled by level, USN, WAM and modulation; stereo HighBlend controlled by level, USN, WAM and modulation. — [NXP TEF668X data sheet](https://radiokot.ru/forum/download/file.php?id=381990)
- TEF668X command set exposes per-indicator control curves: Set_SoftMute_Level/Noise/Mph/Max (start/slope in dBµV for level, % for noise/mph), Set_HighCut_Time/Mod/Level/Noise/Mph/Max/Min (Max/Min limits in Hz), Set_Stereo_Time/Mod/Level/Noise/Mph/Max/Min, Set_StHiBlend_Time/Mod/Level/Noise/Mph/Max/Min; every *_Time command has four parameters: slow_attack, slow_decay, fast_attack, fast_decay (ms). Get_Quality_Status returns level, usn, wam, offset, bandwidth, modulation. — [NXP TEF668X data sheet, command table](https://radiokot.ru/forum/download/file.php?id=381990)
- **WAM (wideband AM) definition (TEF689x):** "The digitized level signal is filtered by a digital 21 kHz bandpass filter to generate the WAM signal, which contains the AC component of the level signal. There is no WAM signal at good RF receiving conditions. At multipath conditions the level signal is amplitude modulated and the WAM signal indicates the magnitude of the multipath signal." Gain selectable in 4 steps. — [NXP AN02106 §4.7.2](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- **USN (ultrasonic noise) definition (TEF689x):** the MPX signal passes a 4-pole high-pass filter with 100 kHz cut-off; the output (USN) is used in the FM noise detector (spikes) and in weak-signal processing "for the detection of multipath and adjacent channel interferences". Its sensitivity is specified with a 150 kHz test tone on the MPX. — [NXP AN02106 §4.7.3](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- Level curve expected from the tuner: ~0.5 V with no RF, slope 800 mV / 20 dB; digitized by an 8-bit ADC. — [NXP AN02106 §4.7.1](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- WAM and USN are weighted in fast average detectors with fixed 1 ms attack and decay. The level detector has 8 selectable attack/decay sets: 3/3 s, 3/6 s, 1.5/1.5 s, 0.5/1.5 s, 0.5/0.5 s, 0.17/0.5 s, 0.06/0.17 s, 0.06/0.06 s. — [NXP AN02106 §4.7.4](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- Multipath (MPH) detector: lev, wam and usn are combined in a multi-valued "or" (the highest value controls); MPH attack/decay sets 0.5/12 s, 0.5/24 s, 0.5/6 s, 0.25/6 s. After a tune/preset the LEVEL and MPH detectors switch to a fast 60 ms attack and decay. — [NXP AN02106 §4.7.4.3–4.7.4.4](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- "Attack-bound" (ATTB) option: detector recovery is limited to the range actually used by weak-signal processing, so after a strong-signal period the detector sits near the activation threshold and reacts quickly to sudden degradation without needing a faster attack constant. — [NXP AN02106 §4.7.6](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- Silicon Labs Si47xx reports RSSI (dBµV), SNR (dB), MULT (multipath metric 0–100, "0 = no multipath; 100 = full multipath"), PILOT indicator, stereo blend %, and AFC rail (frequency error beyond FM_MAX_TUNE_ERROR, default 20 kHz). — [Skyworks AN332, FM_RSQ_STATUS and property 0x1108](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- PE5PVB firmware computes a display SNR from TEF level and USN: `snr = int(0.46222375*level/10 - 0.082495118*usn/10) + 10` (level in 0.1 dBµV, usn in 0.1 %), and scores AF candidates as `level - usn - wam`. — [PE5PVB TEF6686_ESP32 Tuner_Drv_Lithio.cpp / TEF668X.cpp](https://github.com/PE5PVB/TEF6686_ESP32)

### Inferences
- The Godwit "SNR" (envelope variance over 80 ms) is physically closer to a WAM/multipath indicator than to a noise indicator: envelope AM after the channel filter rises with multipath (NXP's definition of WAM) as well as with noise. This likely explains why strong stations read only 15–20 dB: residual envelope ripple from multipath, adjacent channels, channel-filter edge effects and 8-bit quantization caps the metric. Recommended: compute separate indicators — (a) level = mean |IQ|² in dBFS (corrected for RTL gain setting) with a 60 ms–1.5 s smoother; (b) WAM = band-limited AC of |IQ| normalized by its DC (modulation index, %), ~1 ms detector; (c) USN = RMS of MPX in an ultrasonic band, normalized; (d) offset = mean of demodulated MPX DC; (e) modulation = peak deviation.
- Using max-of ("worst indicator wins") combination per action, as NXP does, is simple to implement and avoids one bad indicator being masked by good ones.
- **SDR limitation:** at 228 kHz MPX the Nyquist limit is 114 kHz, so a 100 kHz-HPF/150 kHz-test USN band as on TEF689x is not available after decimation. Options: compute USN on the demodulated phase-difference signal before decimation (at 1.14 MS/s the discriminator output extends to ~570 kHz, limited by the channel filter), or use the 60–100 kHz region of the 228 kHz MPX (above RDS at 57±2.4 kHz). Caveat (from general knowledge, not sourced here): in North America SCA subcarriers at 67 and 92 kHz may occupy that band, so a USN band of ~100–112 kHz or one measured pre-decimation is safer.

### Gaps
- TEF668x default values of the *_Time (slow/fast attack/decay) and *_Level/Noise/Mph commands: the full TEF668X User Manual (rev 1.6) was not retrievable (usermanual.wiki redirected to a 403 page). Only the values the PE5PVB firmware writes are known (see Q2).
- Exact Si47xx MULT metric algorithm is not documented in AN332 (only 0–100 range).
- No source found for ST TDA7707/TDA7708 parameter values; Si479x (AN645) programming guide was not retrieved.

## Q2. Typical thresholds, time constants and curve shapes (blend, soft mute, high cut, stereo high blend)

### Takeaway
Industry defaults are strongly asymmetric: fast attack (degrade within ~3–16 ms) and slow release (recover over ~0.16–3.3 s, multipath up to seconds). Curves are linear-in-dB ramps between two thresholds. Blend starts around 49 dBµV RSSI / 27–30 dB SNR (full mono at 30 dBµV / 14 dB SNR); soft mute only engages at very low SNR (≈4 dB) with ~16 dB maximum; high cut ramps over SNR 24→15 dB with the corner limited to 2–11 kHz.

### Cited Findings
**Silicon Labs Si47xx (AN332) — stereo blend**
- FM_BLEND_RSSI_STEREO_THRESHOLD default 49 dBµV (full stereo above), FM_BLEND_RSSI_MONO_THRESHOLD default 30 dBµV (full mono below); stereo % is linear between them (Figure 3). — [AN332 props 0x1800/0x1801](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- FM_BLEND_RSSI_ATTACK_RATE default 4000 ≈ 16 ms (stereo→mono); FM_BLEND_RSSI_RELEASE_RATE default 400 ≈ 164 ms (mono→stereo); rate = 65536 / transition time in ms. — [AN332 0x1802/0x1803](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- FM_BLEND_SNR_STEREO_THRESHOLD default 27 dB, FM_BLEND_SNR_MONO_THRESHOLD default 14 dB; SNR blend attack ≈16 ms, release ≈164 ms. (Figure 4 in the same document labels the default as x=30, y=14 — minor internal inconsistency.) — [AN332 0x1804–0x1807](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- FM_BLEND_MULTIPATH_STEREO_THRESHOLD default 20 % (full stereo below), MONO_THRESHOLD default 60 % (full mono above); multipath attack ≈16 ms, multipath **release default 40 ≈ 1.64 s** (10× slower than RSSI/SNR release). — [AN332 0x1808–0x180B](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)

**Silicon Labs Si47xx — soft mute**
- FM_SOFT_MUTE_SNR_THRESHOLD default 4 dB (range 0–15); FM_SOFT_MUTE_MAX_ATTENUATION default 16 dB (0–31; 0 disables); FM_SOFT_MUTE_SLOPE default 2 dB/dB; attenuation = min(SLOPE × (THRESHOLD − SNR), MAX_ATTEN); recommended SLOPE = ceil(MAX_ATTEN / THRESHOLD). — [AN332 0x1301–0x1303](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- FM_SOFT_MUTE_ATTACK_RATE and RELEASE_RATE default 8192 ≈ 8000 dB/s (rate dB/s = value/1.024, range 1–32767). — [AN332 0x1304/0x1305](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)

**Silicon Labs Si47xx — hi-cut**
- FM_HICUT_SNR_HIGH_THRESHOLD default 24 dB (hi-cut begins), FM_HICUT_SNR_LOW_THRESHOLD default 15 dB (maximum band-limiting). — [AN332 0x1A00/0x1A01](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- FM_HICUT_ATTACK_RATE default 20000 ≈ 3 ms (lowering corner); FM_HICUT_RELEASE_RATE default 20 ≈ 3.3 s (raising corner). — [AN332 0x1A02/0x1A03](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- FM_HICUT_MULTIPATH_TRIGGER_THRESHOLD default 20 %, FM_HICUT_MULTIPATH_END_THRESHOLD default 60 %. — [AN332 0x1A04/0x1A05](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- FM_HICUT_CUTOFF_FREQUENCY selects the minimum hi-cut transition frequency: 2, 3, 4, 5, 6, 8 or 11 kHz (0 = disabled, default); a separate field sets maximum audio bandwidth with the same choices. — [AN332 0x1A06](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)

**NXP TEF689x (AN02106) — control routing and ranges**
- Three outputs: SNC (stereo blend), HCC (high cut), SM (soft mute). SNC input = max(slow mph, fast wam, fast usn); HCC input = slow lev (HCMP=0) or slow mph (HCMP=1); SM input = max(slow lev, fast usn), with the usn-caused mute depth separately limited in 4 steps (level-caused mute not limited). — [AN02106 §4.7.5](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- High cut is a stand-alone low-pass filter, "not realized by shifting of the de-emphasis filter". Fixed high cut: 8 settings from unlimited down to 1.5 kHz; suggested FM range unlimited to 6.8 kHz, AM 6.8–3.3 kHz, weather band 2.2/1.5 kHz. Dynamic HCC: 8 start values, 4 slopes, minimum bandwidth selectable 2 kHz or 3 kHz (HCSF); attenuation at 10 kHz used as the HCC indicator. — [AN02106 §4.5, §4.7.5.2](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- Stereo blend: 16 start values and 4 slopes (separation vs equivalent level voltage). Soft mute: 8 start values (defined as level where mute = 3 dB), 4 slopes, and 4 limits on USN-caused mute depth. — [AN02106 §4.7.5.1, §4.7.5.3](https://www.nxp.com/docs/en/application-note/AN02106.pdf)

**NXP TEF668x — values used in practice (PE5PVB firmware)**
- Stereo blend: Set_Stereo_Level(mode, start = user value ×10 [0.1 dBµV], slope 60 [= 6 dB]); Set_Stereo_Noise(start 240, slope 200) and Set_Stereo_Mph(start 240, slope 200), i.e. 24 % start, 20 % span on USN/WAM. — [PE5PVB TEF668X.cpp setStereoLevel](https://github.com/PE5PVB/TEF6686_ESP32)
- High cut: Set_Highcut_Level(start = user ×10, slope 300 [30 dB]); Set_Highcut_Noise(360, 300) and Set_Highcut_Mph(360, 300) (36 % start, 30 % span); Set_Highcut_Max(limit ×100 Hz). — [PE5PVB TEF668X.cpp setHighCutOffset/setHighCutLevel](https://github.com/PE5PVB/TEF6686_ESP32)
- Stereo high blend: Set_StHiBlend_Level(start ×10, slope 300), Set_StHiBlend_Noise/Mph(360, 300), Set_StHiBlend_Max(limit ×100 Hz). — [PE5PVB TEF668X.cpp setStHiBlend*](https://github.com/PE5PVB/TEF6686_ESP32)
- TEF668X datasheet: conventional receivers blend stereo→mono at "medium signal levels of 40 dBµV"; FMSI (TEF6687/89, a multi-band stereo improvement, exposed as StBandBlend Time/Gain/Bias commands 90–92) extends blend down to ~10 dBµV. — [NXP TEF668X data sheet](https://radiokot.ru/forum/download/file.php?id=381990); [PE5PVB Tuner_Drv_Lithio.h command list](https://github.com/PE5PVB/TEF6686_ESP32)

### Inferences
- Godwit's blend ramp (SNR 10→16 dB, release-and-attack both 0.5/80 ms block ≈ symmetric ~115 ms) is both too narrow (6 dB span vs Si 13 dB SNR / 19 dB RSSI span) and symmetric. Suggested: attack ~15–20 ms, release ~150–300 ms for level/SNR, 1–2 s release for multipath; ramp span 10–15 dB; recalibrate thresholds against the metric's observed ceiling (if strong stations read 15–20 dB, full-stereo threshold must sit below ~14 dB, or—better—drive blend from a proper RSSI-like level plus a separate WAM/USN indicator).
- Godwit soft mute (6→12 dB SNR, floor 0.03 ≈ −30 dB) is far more aggressive than Si defaults (engage below 4 dB, 16 dB max). Car tuners keep soft mute shallow (~10–20 dB) and let blend + high cut do most of the work; suggested max attenuation 12–20 dB.
- Implement high cut as a separate first-order or second-order low-pass on L/R audio (not by changing de-emphasis), corner swept log-linearly from ~15 kHz (or "off") down to a floor of ~2–3 kHz, driven by max(level-derived, multipath-derived) control with ~3 ms attack and ~1–3 s release.
- Stereo high blend: apply the blend only to the upper portion of L−R (low-pass the L−R channel with a moving corner, e.g. from 15 kHz down to ~2–3 kHz as quality drops) before full blend kicks in; this keeps low-frequency separation where noise is least audible.

### Gaps
- No NXP defaults for slow/fast attack/decay in TEF668x Time commands were confirmed (user manual unavailable). AN02106 detector time constants above are for the older TEF689x and are the best NXP-documented values found.
- Si479x (AN645) and ST TDA770x parameter tables not retrieved.

## Q3. Multipath detection in software

### Takeaway
The standard detector is AM on the receive envelope after the channel filter ("WAM"): band-pass the envelope around ~20 kHz (NXP: 21 kHz BPF on the level signal) with a ~1 ms detector, and in parallel watch ultrasonic MPX energy (USN), which rises with both multipath and adjacent channels. The action is stereo blend (fast attack, very slow release) and optionally high cut; TEF668x adds channel equalization and "Improved Multipath Suppression".

### Cited Findings
- WAM = 21 kHz band-pass of digitized level (AC component); zero under good conditions, proportional to multipath magnitude otherwise. — [NXP AN02106 §4.7.2](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- USN (MPX > 100 kHz, 4-pole HPF) is used for multipath and adjacent-channel detection. — [NXP AN02106 §4.7.3](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- Multipath drives stereo blend (SNC) quickly via the 1 ms wam/usn path and slowly via MPH (attack 0.25–0.5 s, decay 6–24 s); optionally drives high cut (HCMP=1). — [NXP AN02106 §4.7.4–4.7.5](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- Si47xx: multipath (0–100 %) drives both blend (20→60 %, release 1.64 s) and hi-cut (20→60 %). — [AN332](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- TEF668X provides a Channel Equalizer ("improves multipath reception as well as weak signal reception by means of adaptive filtering") and Improved Multipath Suppression ("reduces the audibility of multipath distortions"); channel bandwidth dynamically controlled by PACS (Precision Adjacent Channel Suppression) depending on adjacent-channel conditions, modulation and signal strength. — [NXP TEF668X data sheet](https://radiokot.ru/forum/download/file.php?id=381990)
- PE5PVB exposes these as user toggles: setiMS → Set_MphSuppression, Set_ChannelEqualizer. — [PE5PVB TEF668X.cpp](https://github.com/PE5PVB/TEF6686_ESP32)

### Inferences
- Software recipe on Godwit: env = |IQ| after channel filter (1.14 MS/s or after first decimation); m = env/mean(env) − 1; WAM = RMS of m band-passed ~10–40 kHz (or around 21 kHz like NXP) with ~1 ms detector, expressed as % AM. Distinguish from noise: noise raises USN and lowers level/SNR without strong low-frequency envelope AM; multipath gives strong envelope AM at moderate-to-high level. Rule: high level + high WAM → multipath (blend fast, release slowly, mild high cut, do not soft-mute much); low level + high USN → noise (blend + high cut + soft mute).
- A CMA (constant-modulus) equalizer on I/Q is the software analogue of NXP's channel equalizer and is feasible at 228–285 kS/s; worth listing as a later-stage option.
- Note the 8-bit RTL envelope has quantization ripple and R820T AGC steps; turn off tuner AGC or ignore samples around gain changes when computing WAM.

### Gaps
- No published numeric mapping from envelope AM % to "multipath %" for Si47xx; thresholds will need empirical calibration on Godwit recordings.

## Q4. Noise blankers for FM (impulse / ignition noise)

### Takeaway
Car FM noise blankers detect impulses in the ultrasonic part of the MPX (HPF ≥ 90–100 kHz) and/or as spikes on the level (envelope) signal, with adaptive thresholds that track the average noise, then blank the MPX/audio with sample-and-hold or interpolation for a short window, using a delay so the gate aligns with the impulse.

### Cited Findings
- TEF689x FM noise detector: trigger from MPX (4-pole 100 kHz high-pass) and from spikes on the LEVEL signal, combined for reliable triggering across RF levels; "Adaptive thresholds are created by means of an AGC circuit in the MPX and LEVEL part to control the sensitivity in relation to the average noise"; sensitivity 4 steps each (MPX 100/50/20/10 mV peak; LEVEL 200/150/120/110 mV peak). — [NXP AN02106 §2.2.5, §4.4](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- In FM mode the blanker "operates as a sample and hold circuit" on the stereo-decoder output, with a low-pass filter included; in AM the audio is muted during the pulse. — [NXP AN02106 §2.2.4](https://www.nxp.com/docs/en/application-note/AN02106.pdf)
- TEF668x FM noise blanker exposed as Set_NoiseBlanker(mode, start); PE5PVB writes start = user value ×10 (disabled with mode 0, start 1000). — [PE5PVB TEF668X.cpp setFMNoiseBlanker](https://github.com/PE5PVB/TEF6686_ESP32)
- Patent US4327446: noise-detection high-pass filter above 90 kHz so program content is blocked; threshold tracks average noise level. — [US4327446 (USPTO)](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/4327446)
- Patent US4975953: ultrasonic HPF + AGC + averaging + comparator; notes the HPF can mistake adjacent-channel energy for noise, and that de-emphasis stretches pulses so the blank interval must be longer if blanking after de-emphasis. — [US4975953](https://patents.google.com/patent/US4975953)
- Patent US5036543: HPF detector → one-shot pulse shaper → gate before MPX decoder, with a delay line so the audio reaches the gate when detection completes, plus a level-hold circuit. — [US5036543](https://patents.google.com/patent/US5036543)
- Patent US4755120 / 7805120 ("Noise blanker circuit and method for removing noise and correcting a signal"): detect, remove pulse, correct by interpolation; separates L+R and L−R in the process. — [USPTO 7805120](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/7805120)
- EP2624466: template-based detection of noise pulses (including sub-sampled phase variants and overlapping pulses) in an FM stereo decoder pipeline. — [EP2624466](https://patents.google.com/patent/EP2624466)

### Inferences
- Software design: blank on the MPX *before* stereo decoding and de-emphasis (pulses are shortest there). Detector: |HPF(MPX)| or the pre-decimation discriminator output above ~100 kHz, compared to k × running mean (k ≈ 4–10, mean time constant ~10–50 ms); also flag sudden dips/spikes of |IQ|. Delay the MPX by the detector latency (a few samples at 228 kHz); replace a window of ~10–50 µs (≈2–12 samples at 228 kS/s) by linear/cubic interpolation, keeping the 19 kHz pilot phase intact (or blank L+R/L−R after pilot removal). Rate-limit triggers; if the trigger rate exceeds a ceiling, treat it as continuous noise (USN) and disable blanking rather than chopping the audio.
- SDR disadvantage: hardware blankers see the IF/level at full bandwidth with analog detectors; the RTL-SDR has 8-bit resolution and the 1.14 MS/s channel limits ultrasonic visibility to below ~570 kHz pre-decimation and 114 kHz post-decimation. Ignition pulses are still visible pre-decimation, so place the detector before the 1.14 MS/s→228 kS/s decimation.

### Gaps
- No public numeric defaults (blank width, threshold ratio) for TEF668x or Si479x FM impulse blankers were found.

## Q5. Smooth control (zipper noise), and open-source implementations

### Takeaway
Hardware tuners specify control changes as rates (ms per transition, dB/s) applied continuously, not as block steps; software should compute targets per block but ramp gains/blend/filter corner per sample. Mainstream open-source SDR FM demodulators do not implement car-radio weak-signal processing; the TEF6686 firmware projects expose the NXP chip's processing rather than implementing it.

### Cited Findings
- Si47xx express transitions as rate values (blend 65536/time-ms; soft mute dB/s up to ~32000 dB/s), i.e. continuous slew, not block stepping. — [AN332](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/application-notes/aN332.pdf)
- SDRangel Broadcast FM sink: pilot PLL, stereo/LSB-stereo selection and an RF squelch with attack/decay counter; no stereo blend, soft mute or high cut in the sink code. — [SDRangel bfmdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodbfm/bfmdemodsink.cpp)
- SDR++ WFM demodulator exposes only Stereo on/off and a Low Pass checkbox. — [SDR++ wfm.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/decoder_modules/radio/src/demodulators/wfm.h)
- PE5PVB TEF6686_ESP32 exposes TEF668x controls: soft mute max, FM noise blanker, stereo level, high cut offset/level, stereo hi-blend offset/level, iMS, channel EQ, FMSI bias, and reads processing status (highcut, stereo, sthiblend, 4 FMSI band values). — [PE5PVB TEF6686_ESP32](https://github.com/PE5PVB/TEF6686_ESP32)
- Patent US8023918 describes blend driven by comparing L−R and L+R energy (k from 1 to 0) rather than only signal strength/SNR. — [US8023918](https://patents.google.com/patent/US8023918)
- The "no-sdr" project lists Soft Mute and FM Hi-Blend features (not verified in code). — [no-sdr v1.1.0 release notes](https://newreleases.io/project/github/gbozo/no-sdr/release/v1.1.0)

### Inferences
- Godwit's 80 ms stepped blend will produce audible steps in separation and noise floor; fix by linearly interpolating blend (and gain, and high-cut coefficient) across each output block, or a one-pole per-sample smoother with separate attack/release coefficients (e.g. τ_attack ≈ 5 ms, τ_release ≈ 150 ms–1.5 s). For a moving high-cut corner, update a one-pole/biquad coefficient per sample or per 32–64-sample sub-block to avoid zipper noise.
- Suggested Godwit chain (all software): level, WAM, USN, offset detectors → per-action max-of control with asymmetric time constants and attack-bound → stereo high blend (L−R LPF) → stereo blend → high cut (2/3–15 kHz) → soft mute (max ~15 dB), plus pre-decimation impulse blanker. Fast (~60 ms) constants for ~0.5 s after retune.

### Gaps
- Did not check NGSoftFM, Gqrx or FM-DX-Webserver source; no evidence found of open SDR implementations of WAM/USN-driven blend or FM noise blanking.
