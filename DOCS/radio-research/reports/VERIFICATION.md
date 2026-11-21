# Verification log

Checked 2026-11-21 against upstream `trailcurrentoss/TrailCurrentGodwit` at commit 438e20b, primary sources, and measurements with the repo's own `fm::Receiver` (radio/src/fmreceiver.cpp) built in Debian (WSL2), g++ 14.

**Where this log disagrees with a report, this log wins.** Statuses: ✅ verified · ⚠️ partly right (corrected wording given) · ❌ wrong (corrected) · ⏳ open.

Terminology: the digital radio system is referred to only as NRSC-5 / NRSC-5 IBOC / IBOC digital radio, and its multicast channels as supplemental program services (SPS). Its consumer brand name is a trademark of Xperi and is not used in these documents.

## 1. Code claims (read from source)

| Claim | Status | Evidence |
|---|---|---|
| Voice assistant accepts FM 87.5–108 MHz in 0.1 MHz steps | ✅ | hmi/qml/Intents.js:211, 228 |
| godwit-radio tunes only 87.9–107.9 MHz, 200 kHz raster, ignores other `tune` values | ✅ | radio/src/main.cpp:48-50, 341 |
| HMI accepts any frequency (rounds to 100 kHz) | ✅ | hmi/src/radio.cpp:21-24, 104-118 |
| Legal.js lists librtlsdr as GPL-2.0-only while its own text says "GPL-2.0 or later" | ✅ | hmi/qml/Legal.js:64-65 |
| radio.h says only the Aircraft app shares the dongle | ✅ | hmi/src/radio.h:11, 35-37 |
| RBDS PTY table blank for 24–28, 30, 31 | ✅ | hmi/src/radio.cpp:36-41 |
| HMI prefers the PI-derived call sign over PS | ✅ | hmi/src/radio.cpp:405-409 |
| All DSP runs inside the USB callback | ✅ | radio/src/main.cpp:107-120, 315 |
| Stereo blend updated once per 80 ms block | ✅ | radio/src/fmreceiver.cpp:494 (block = kMpxRate·80 ms) |
| Audio pipe 16 KB, non-blocking, excess dropped | ✅ | radio/src/main.cpp:79-105, 295-296 |
| README: strong stations read 15–20 dB; AM "would take an upconverter" | ✅ (README text) / ❌ (AM claim is wrong for the SMArt v5, see §4) | radio/README.md:26-27, 57-61 |
| Four radio ctest targets | ✅ | radio/CMakeLists.txt:31-41 |

## 2. DSP measurements (repo receiver, synthetic signals)

| Claim | Status | Measured |
|---|---|---|
| De-emphasis error vs analogue 75 µs | ✅ | +0.44 dB at 10 kHz, +1.01 dB at 15 kHz |
| One added zero (q = −0.1166 at 57 kHz) fixes it | ✅ | max error 0.027 dB, 20 Hz–15 kHz |
| 19 kHz rejection of the 189-tap 17 kHz audio filter | ✅ | −52.7 dB (−6.0 dB at 17 kHz, flat to 15 kHz) |
| Pilot biquad phase slope | ✅ | −0.600°/Hz at 19 kHz |
| "±2 Hz pilot offset limits separation to ~34 dB" | ❌ | Separation 32.7 / 32.6 / 32.5 / 32.5 dB at 19000 / 19001 / 19002 / 18998 Hz. A 38 kHz phase error φ only scales L−R by cos φ: separation = 40·log10(cot(φ/2)) ≈ 67 dB at ±2 Hz. The report used 20·log10. |
| **New:** what limits separation | ✅ | 32.7 dB regardless of level (0.2–1.0) or noise ⇒ systematic L−R gain error. Cause: the phase-difference discriminator's sinc(f/fs) response; at 38 kHz/228 kHz, sinc(1/6) = 0.955. Scaling L−R by 1.0472 gave 72.7 / 65.6 / 59.1 / 55.5 dB at 1 / 5 / 10 / 14 kHz. |
| Mono station with RDS decodes | ✅ | PS + call decoded; 80 groups at 0 Hz, 56 at a 3 Hz 57 kHz offset |
| Envelope "SNR" on clean synthetic signal | ✅ | 35.8 dB (tones); 27.8 dB (dense clipped programme) |

Probe sources: kept with this log (`probe.cpp`, `verify.cpp`, `sinc.sh`).

## 3. Standards (primary sources)

| Claim | Status | Source and wording |
|---|---|---|
| FM pilot 19 kHz ± 2 Hz | ✅ | 47 CFR 73.322(a)(2) |
| RBDS PTY 24 Spanish Talk, 25 Spanish Music, 26 Hip-Hop, 27–28 unassigned, 29 Weather, 30 Emergency Test ("Test"), 31 Emergency ("ALERT !") | ✅ | NRSC-4-B Annex F Table F.2. Note: NRSC-4-B is archived; IEC 62106-9:2021 replaces it. |
| "NRSC-G300-C recommends showing PS ahead of the PI call sign" | ⚠️ | G300-C §5.1 recommends decoding **RT+ StationName.Short** instead of back-calculating call letters from PI, because "many stations ... do not send call letters through PI codes". NRSC-4-B §D.7.1 notes call letters or a slogan may be sent in the PS. |
| PI → call: K 0x1000–0x54A7, W 0x54A8–0x994F, 3-letter table 0x9950–0x9EFF, A___ and AF__ forms | ✅ | NRSC-4-B §D.7, Table D.7 |
| "No call sign can be computed for Canadian/Mexican PIs" | ❌ for Canada / ✅ for Mexico | **Canada:** BPR-3 (Issue 7, May 2024) §3.13.2 requires RBDS to follow the NRSC standard but says nothing about PI. ISED's "Program information codes for radio broadcasting stations" page (node/587, B-TAC, 2007) requires a unique PI per station, sets Canada's range as C000–CFFF and CBC networks as B_02–B_09, and hosts the CBC-built "RBDS PI Code Algorithm" spreadsheet. Its formula (n = p·702 + b·27 + c + 257; PI = 0xC000 + n + ⌊(n−257)/255⌋, CF/CH/CI/CJ/CK) is reversible over C101–CEC3. Round-trip checked on all 3,510 calls; off-air Calgary logs match (C185 CFEX, C37C CFXL, C456 CHFM, C95C CJAY, CB42 CJSW, CE23 CKUA, CD59 CKMP); two re-checked by hand. Not reversible: VF low-power table; repeaters reuse the parent PI. No CRTC rule found. **Mexico (0xF):** no published allocation method found (IFT). |
| Tuner chips expose separate quality indicators incl. "pilot SNR" | ⚠️/❌ | TEF668x: level, USN, WAM, frequency offset, modulation, IF bandwidth, stereo-pilot flag (no SNR, no pilot SNR). Si47xx (AN332): RSSI, SNR, multipath (newer revisions), frequency offset, pilot-present bit, blend %. Neither has a pilot SNR. |
| NRSC-1-C de-emphasis: pole 2122 Hz, zero 8700 Hz, −10.00 dB at 10 kHz | ✅ | NRSC-1-C §6.2, Table 2 |
| ~30 dB AM loss from a whip into 50 Ω | ⚠️ | First-principles: 36 dB less at 530 kHz, 26 dB less at 1700 kHz than into a car radio's ~80 pF input (10–20 pF whip). "~30 dB" holds only mid-band. |

## 4. Hardware and software

| Claim | Status | Source and wording |
|---|---|---|
| `pw_stream_set_rate()` needs PipeWire ≥ 1.4.0 | ✅ | PipeWire docs and 1.4.0 stream.h ("Since 1.4.0"); returns −ENOTSUP with no adaptive resampler |
| nrsc5: GPL-3.0-or-later; v3.2.0 (2026-06-02); FM and AM (`--am`); cu8 input 1,488,375 S/s | ✅ (README says "AM", not "hybrid AM") | github.com/theori-io/nrsc5 |
| NESDR SMArt v5: RTL2832U + "R820T2/R860" (datasheet shows R860), 0.5 ppm TCXO, direct sampling 0.1–25 MHz (Q branch), no bias tee | ✅ (tuner name corrected) | Nooelec datasheet 100700 rev 1 (2022-10-04) and product page |
| "NESDR SMArTee has a bias tee" | ⚠️ | It is the SMArTee **v2** (older, 25–1750 MHz), 4.5 V 250 mA always-on bias tee |
| Airspy HF+ Discovery: 0.5 kHz–31 MHz, **64**–260 MHz, 660 kHz of 768 ksps, 110/95 dB BDR, $169 (list $199) | ✅ (VHF lower edge corrected from 60) | airspy.com; v3.airspy.us |
| HF+ has "MW and FM preselection" | ⚠️ | HF preselector with 4 filter banks (corners ~DC, 5, 10, 17, 31 MHz) + FM-band preselector; no dedicated MW filter |
| libairspyhf licence | ✅ | library BSD-3-Clause; command-line tools GPL-2.0-or-later |
| SDRplay API: closed, EULA grants no redistribution; ARM64 Linux build exists | ✅ | EULA (effective 12 Jan 2021) read from a mirror of API 3.15; official licence URL 404s |
| RTL-SDR Blog V4 end-of-life; V4L (R828S) replaces it without the extra filtering | ✅ | rtl-sdr.com "RTL-SDR Blog V4 end of line" |
| V4 notch switched off inside the notched bands | ✅ | rtl-sdr-blog tuner_r82xx.c: off at ≤ 2.2, 85–112, 172–242 MHz (Blog fork only) |
| R820T tuner AGC overloads ~10 dB earlier | ✅ | HB9AJG (Aug 2013): "degrades the overload limit by about 10dBs, independent of the gain setting", at 145 and 435 MHz (AGC on vs off) |
| librtlsdr: direct sampling puts the R820T in standby; osmocom needs mode 2 for Q; Blog fork auto-switches below 24 MHz on R820T-family tuners (not V4/V4L) | ✅ | librtlsdr.c in both trees |
| Taoglas MA1045 is a single-hole mount | ❌ | Permanent roof mount: Ø19 mm centre hole + two Ø7.2 mm holes 55 mm apart (datasheet p.28) |
| Taoglas MA1045 bands | ✅ | Datasheet SPE-19-8-095-A: cellular 698–803/824–894/880–960/1710–1880/1850–1990/1920–2170/2300–2500 MHz; GPS L1 + BeiDou B1 ×2; SiriusXM 2320–2345; AM/FM/WB amplified (no published figures); 300 mm RG-174 |

## 5. Corrections applied

- Issue draft: pilot-PLL bullet replaced by the measured discriminator-droop fix; RDS wording per §3; tuner-chip indicators per §3; AM-loss figures per §3; AGC claim attributed to HB9AJG; "bypasses the tuner"; no V4 recommendation; Taoglas "permanent roof mount"; HF+ "HF and FM-band preselection"; audio-pipe wording covers both overflow and underrun.
- RADIO-PLAN.md: items 0.2, 1.2, 1.5, 2.1, Phase 7 HF+ and Taoglas notes.
- Reports: correction notes added; brand name scrubbed from all notes and reports (only nrsc5's literal `--dump-hdc` flag remains).

## 6. Open

- ✅ Canadian PI code assignment: resolved in §3 (ISED/CBC algorithm; no CRTC rule).
- Not measured anywhere: SMArt v5 MW sensitivity; HF+ Discovery spurs on a vehicle 5 V rail and its current draw; TEF6686 vs SDR on the road; Taoglas MA1045 AM/FM amplifier gain and output impedance; PipeWire version in the AGL image.
