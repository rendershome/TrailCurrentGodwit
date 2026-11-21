Title: Radio: proposed roadmap for AM/FM reception quality

Hi! I'm a long-time radio broadcast engineer (I opened #1 to get my bearings) and I'd like to help make Godwit's radio as good as a factory car radio, and better where an SDR allows. Before sending pull requests, I wanted to share where I'd like to go and ask how you'd prefer to receive this work.

I've read through `radio/` and the HMI radio code. The DSP core is in good shape: pilot-locked 38/57 kHz regeneration, matched L+R/L−R filtering and about 53 dB of pilot rejection. The main limits are in how the signal is measured and in a few details. Here's what I'd propose, in order:

**1. Measurement first**
- Tests that pin down the receiver's response: de-emphasis curve, stereo separation with the pilot a couple of Hz off, pilot rejection, a mono station carrying RDS.
- A small CI workflow running the four existing `radio/` ctest targets.
- An option for `godwit-radio` to record raw I/Q and play it back, so tuning changes can be checked against real off-air captures (strong, weak, multipath, ignition noise, adjacent channel).

**2. Small, self-contained fixes**
- De-emphasis: the one-pole filter at 57 kHz runs +0.4 dB at 10 kHz and +1.0 dB at 15 kHz above the 75 µs curve. Adding one zero brings it under 0.05 dB. τ would become a parameter (75/50 µs).
- Stereo separation: on a clean synthetic stereo signal through `fm::Receiver`, separation is about 32.7 dB at 1 kHz, whatever the level or noise. The cause is the discriminator: measuring frequency as the phase step between samples has a sinc(f/fs) response, so the 38 kHz subcarrier comes out at sinc(38/228) = 0.955 of the baseband level. Scaling L−R by 1/0.955 raised separation to 72.7 dB at 1 kHz and 55.5 dB at 14 kHz in the same test. A short inverse-sinc filter on the multiplex would flatten what's left.
- Stereo blend changes in 80 ms steps; a per-sample ramp removes the steps.
- RDS/RBDS:
  - complete the RBDS PTY table (24 Spanish Talk, 25 Spanish Music, 26 Hip Hop, 30 Emergency Test, 31 Emergency);
  - stop preferring the PI-derived call sign over the PS. NRSC-G300-C notes that many U.S. stations' PI codes aren't derived from their call letters, and recommends receivers use RT+ StationName.Short rather than back-calculating. Until RT+ is decoded, the PS, which NRSC-4-B notes may carry the call letters or a slogan, is the safer name to show;
  - complete the NRSC-4-B PI-to-call rules (the 3-letter table, the A___ and AF__ forms), and add Canada. ISED's PI-code page sets Canada's range as C000–CFFF (CBC networks B_02–B_09). The CBC-built calculator ISED hosts there uses a formula that can be reversed: C101–CEC3 map back to CF/CH/CI/CJ/CK call signs, including 3-letter calls. I checked it against off-air captures of Calgary stations (C185 → CFEX, CB42 → CJSW). Mexican (0xF___) codes have no published method, so no call sign should be computed for them;
  - decode the RDS character set, so accented letters show.
- The voice assistant accepts 87.5–108 MHz in 0.1 MHz steps, while `godwit-radio` only tunes 87.9–107.9 MHz on the 200 kHz raster, so "set the radio to 87.5" shows a frequency the tuner ignores. One band definition shared by all three would fix it.
- Some doc fixes (the librtlsdr licence in Legal.js, which apps share the dongle, supported dongles, FM antenna notes).

**3. Better signal-quality indicators (would like your input before starting)**
Today one figure, the steadiness of the envelope, drives stereo blend, soft mute, seek, scan and the signal bars. Envelope ripple comes from multipath and channel-filter effects as well as noise, so it behaves more like a multipath indicator than an SNR. The README notes that strong stations read only 15–20 dB, while a clean synthetic signal reads about 36 dB. Tuner chips use separate indicators instead. The NXP TEF668x reports level, ultrasonic noise, wideband AM (multipath), frequency offset and modulation. The Silicon Labs Si47xx reports RSSI, SNR, multipath and frequency offset. I'd like to add these and then build on them:
- blend, high blend and high cut, each with its own attack and release;
- a gentler soft mute;
- a noise blanker for ignition and LED-driver clicks;
- later, adaptive IF bandwidth.

This changes the `status` line, so I'd want to agree the fields with you first.

**4. Dongle and audio**
- Manual gain with a software loop driven by ADC clipping. `godwit-radio` uses the tuner's automatic gain today, and HB9AJG's 2013 measurements found the R820T's tuner AGC lowers the overload threshold by about 10 dB.
- Moving the DSP out of the USB callback.
- Compensating for the clock difference between the dongle and PipeWire. Today nothing does: depending on which clock runs faster, the 16 KB pipe to `pw-play` either fills (and `godwit-radio` drops what doesn't fit) or runs dry. The clean fix is a native PipeWire stream using `pw_stream_set_rate()`, which needs PipeWire 1.4 or later. Do you know which version the AGL image ships?

**5. Later**
- RDS: error correction and robustness, RT+ (artist/title), clock time, AF.
- FM band plan and de-emphasis following Settings › Country.
- NRSC-5 IBOC digital reception, using the open-source nrsc5 decoder as a separate, opt-in helper process (it's GPL-3.0). The FM capture can feed both the analog receiver and the decoder from one I/Q stream.
- Opening SDRs by serial number rather than always device 0, so a second receiver can be added. For example, a receiver with HF and FM-band preselection for broadcast, leaving the RTL-SDR free for Aircraft, Ships and Weather at the same time.
- Antenna and front-end filter guidance in the docs. A ~1 m passive whip into a 50 Ω SDR input delivers roughly 36 dB less signal at 530 kHz, and 26 dB less at 1700 kHz, than into a car radio's high-impedance input. Direct sampling also needs a medium-wave band-pass filter.
- A recommended roof antenna. A combination vehicle antenna such as the Taoglas MA1045 (permanent roof mount, IP67) could cover most of what Godwit receives from one mount:
  - amplified AM/FM/weather band, which addresses the AM loss above;
  - two GNSS feeds (GPS L1 and BeiDou B1) for navigation and the position the Aircraft, Ships and Weather apps use;
  - cellular (698–960 and 1710–2500 MHz) and SiriusXM.
  
  Its sibling, the MA1047 shark fin, swaps SiriusXM for two 5G/4G MIMO ports (617–5925 MHz) and two Wi-Fi ports, with one multi-constellation GNSS port. Neither covers 1090 MHz, so ADS-B would still need its own antenna. Neither datasheet publishes the AM/FM amplifier's gain or output impedance, so I'd bench-test them into the SDR first, to check AM level and FM overload, before suggesting it in the docs.
- AM. The README says AM would need an upconverter, but Nooelec's datasheet gives the NESDR SMArt v5 a 0.1–25 MHz direct-sampling range (Q branch), which bypasses the tuner, so medium wave should work with the current dongle. Quality will be limited by the lack of front-end filtering and the 8-bit converter; an upconverter, or a receiver with its own HF preselection, would do better. That would be its own discussion: synchronous AM, bandwidth choices, NRSC-1 de-emphasis, and switching the dongle between modes.

**Questions for you**
1. Is this direction welcome, and is anything here something you'd rather not change?
2. Would you prefer one issue per item, or PRs directly for the small fixes in section 2?
3. Any preferences on PR size, tests, or commit style?

If it helps, I'd start with the tests and the de-emphasis fix.

Thanks!

Guided by rendershome with hands on assistance by Claude Code
