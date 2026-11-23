# Godwit radio plan

Our working plan for improving the AM/FM radio. Local only (excluded from git).
Full reasoning, sources and parameters: [reports/SDR FM car radio receiver design.md](reports/SDR%20FM%20car%20radio%20receiver%20design.md).
Item numbers match the report's work-plan tables.

**How we work**
- One item at a time, each on its own branch from an up-to-date `main` (`git pull upstream main`).
- **PR** = small, verifiable fix we can open directly. **Issue** = changes behaviour or design; propose it to the maintainers with the report's parameters before writing code.
- Every DSP change gets a test in `radio/tests/fmreceiver_test.cpp` that fails before and passes after.
- Don't stack too many open PRs on the maintainers at once: aim for 2-3 in review.
- Follow the platform's contribution guide ([TrailCurrentDocumentation `07_Development/CONTRIBUTING.md`](https://github.com/trailcurrentoss/TrailCurrentDocumentation/blob/main/07_Development/CONTRIBUTING.md)): MIT licence for contributions; 100% open source (no proprietary components or closed-source dependencies, which is why SDRplay can't ship in the image; the RSPduo and RSP1B are owner-installed alternatives through SoapySDR, 7.6c); offline first; "ask in an issue first" when unsure. Its "branch from `develop`" doesn't apply here: Godwit only has `main`.
- PR descriptions use its template sections: **Summary**, **Why This Change?**, **Design Considerations**, **Testing**, **Offline Testing**, **Documentation**, then our footer line.
- Related: **Playbill** (in-rig entertainment head, Radxa Q6A, [TrailCurrentPlaybill](https://github.com/trailcurrentoss/TrailCurrentPlaybill)) has its own RTL-SDR radio, FM/AM/scanner via `rtl_fm` piped to `aplay` (mono, no RDS). Separate product, no shared code with Godwit today; `godwit-radio` could serve it later.

**Status key:** `[ ]` to do · `[~]` in progress · `[x]` done · `[-]` dropped

---

## Done
- [x] Fork, clone, simulator build in WSL (`run-sim.ps1`)
- [x] PR trailcurrentoss/TrailCurrentGodwit#1: simulator key hint (awaiting review)
- [x] Code review of radio/ and HMI radio side
- [x] Research report and docs review
- [x] Hardware, filtering, antenna, AM and NRSC-5 research: [reports/SDR hardware and AM reception.md](reports/SDR%20hardware%20and%20AM%20reception.md)

## Step A: introduce the plan to the maintainers
- [ ] A.1 Open a "Radio roadmap" issue: who you are, the phases below in brief, ask how they want radio contributions (one issue per item? PR size? test expectations?). Link nothing private; cite standards, not the report file.
- [ ] A.2 Confirm the PipeWire version in the AGL release the image uses (decides 4.3's design). Check `os/` for the AGL branch and the pipewire recipe version.

## Phase 0: measurement foundation (do first)
- [ ] 0.2 **PR** Receiver measurement tests: de-emphasis response at 1/5/10/15 kHz; separation at 1/5/10/14 kHz (today ~32.7 dB, see 1.2); pilot rejection > 50 dB (measured 52.7 dB); mono station with RDS. *S*
- [ ] 0.3 **PR** GitHub Actions workflow running the four radio ctest targets (Qt-free). *S*
- [ ] 0.1 **Issue** IQ record/playback in godwit-radio (`--record`, `--iq-file`), then capture real-air test cases: strong, weak, multipath, ignition noise, adjacent channel. *M* - needs a dongle + FM antenna

## Phase 1: small fixes (first PRs)
- [ ] 1.1 **PR** De-emphasis one-pole + one-zero, τ selectable (75/50 µs). p = exp(-1/(fs·τ)), q = -0.1166 at 57 kHz. Target ≤ 0.05 dB error to 15 kHz. Compute q from the audio rate, not a constant: 8.2 may change the FM rates. *S*
- [ ] 1.2 **PR** Correct the discriminator's sinc droop on L−R (verified 2026-11-21): separation today is 32.7 dB on a clean signal, independent of level/noise; L−R × 1/sinc(38k/228k) = ×1.0472 gives 72.7 dB at 1 kHz, 59.1 dB at 10 kHz, 55.5 dB at 14 kHz. Better: short inverse-sinc FIR on the MPX for flat separation. Compute from the actual MPX rate (8.2 may change it). Add a separation test at 1/5/10/14 kHz. *S*
- [-] ~~1.2 (old) Pilot biquad phase in the PLL path~~ dropped: the 0.6°/Hz slope is real, but a phase error only scales L−R by cos φ, so ±2 Hz costs nothing measurable (simulated 32.7 → 32.5 dB; theory ~67 dB limit). The report's "≈34 dB" used the wrong formula.
- [ ] 1.3 **PR** Per-sample stereo blend ramp (no 80 ms steps). *S*
- [ ] 1.4 **PR** Complete RBDS PTY table: 24 Spanish Talk, 25 Spanish Music, 26 Hip Hop, 30 Test, 31 Alert. *S*
- [ ] 1.5 **PR** Stop preferring the PI-derived call sign over PS. Verified: NRSC-G300-C §5.1 recommends RT+ StationName.Short instead of back-calculating from PI (not PS); NRSC-4-B §D.7.1 says call letters/slogan may be sent in PS. So: PS now, RT+ StationName.Short later (5.6). *S*
- [ ] 1.6 **PR** Complete PI → call sign: US (A___, AF__, 3-letter table, B/D/E networks; port from redsea, MIT, attribute) **plus Canada** from the ISED/CBC algorithm (ISED "Program information codes" page, node/587, and the "RBDS PI Code Algorithm" spreadsheet). Reverse for 0xC101-0xCEC3 with low byte ≠ 0: k = (hi-0xC1)·255 + (lo-1); prefix = CF/CH/CI/CJ/CK[k div 702]; r = k mod 702; 3rd letter = r div 27; 4th = r mod 27 (0 = none, 1-26 = A-Z). Verified round-trip on all 3,510 calls and on Calgary off-air logs (C185 CFEX, CB42 CJSW). CBC B_02-B_05 = Radio One / CBC Music / ICI Première / ICI Musique. No call for VF low-power (non-reversible table), repeaters show the parent's call, Mexico (0xF) has no method. Reimplement from the formula, don't copy the spreadsheet. *S*
- [ ] 1.7 **PR** RDS character set (IEC 62106 E.1) → UTF-8, so accented letters show. *S-M*
- [ ] 1.8 **PR** One band definition: voice, HMI and helper agree on 87.9-107.9 MHz, 200 kHz raster; snap or refuse otherwise. *S*
- [ ] 1.9 **PR** Knob-hold preset save no longer overwrites preset 1 after each boot. *S*
- [ ] 1.10 **PR** Docs fixes: librtlsdr licence in Legal.js, dongle-sharing comments, supported dongles and udev IDs, permission troubleshooting, env vars, FM antenna section, left-knob "Next Station", mark v1 mockup superseded, and correct radio/README.md:26-27 (AM is possible by direct sampling on dongles that support it). *S, can be split*
- [ ] 1.11 **PR** Clip and level telemetry on the `status` line (0/255 fraction, IQ RMS dBFS, DC). *S*

## Phase 2: signal-quality indicators (keystone, issue first)
- [ ] 2.1 **Issue** Replace the envelope "SNR" with level, multipath (WAM), ultrasonic noise (USN) and frequency offset (the TEF668x set; verified from its datasheet). A pilot-SNR estimate from our PLL is our own addition, not a chip feature. *M-L*
- [ ] 2.2 **Issue** New `status` fields and HMI migration; seek and scan on level + USN, confirmed by pilot or RDS. *M*
- [ ] 2.3 **Issue** Automatic ppm correction from measured offset (non-TCXO dongles). *S-M*

## Phase 3: weak-signal processing (after Phase 2)
- [ ] 3.1 **Issue** Stereo blend from worst of level/USN/pilot SNR/WAM; 10-15 dB span; attack 15-20 ms, release 150-300 ms (multipath 1-2 s). *M*
- [ ] 3.2 **Issue** Stereo high blend (low-pass on L-R, 15 kHz → 2-3 kHz). *M*
- [ ] 3.3 **Issue** High cut (15 kHz → 2-3 kHz floor; attack ~3 ms, release 1-3 s). *M*
- [ ] 3.4 **Issue** Shallower soft mute: max 12-20 dB, only near the noise threshold. *S*
- [ ] 3.5 **Issue** FM noise blanker at 1.14 MS/s before decimation; blank 10-50 µs by interpolation. *M*
- [ ] 3.6 **Issue** Adaptive IF bandwidth: ±114/90/70/50 kHz from guard-band energy. *L*

## Phase 4: dongle, USB and audio clock
- [ ] 4.1 **Issue** Manual gain + software AGC from clip fraction and RMS (auto gain overloads ~10 dB sooner). *M*
- [ ] 4.2 **Issue** Move DSP out of the USB callback; detect dropped samples. *M*
- [ ] 4.3 **Issue** Native PipeWire stream with drift control (`pw_stream_set_rate`, PipeWire ≥ 1.4), replacing `pw-play`. Depends on A.2. *L*
- [ ] 4.4 **Issue** Pin librtlsdr (osmocom ≥ 2.0 or Blog fork), log tuner type, extend udev IDs for V4. *S*

## Phase 5: RDS robustness and features
- [ ] 5.1 Three-block sync + 1-2-bit burst error correction (redsea approach). *M*
- [ ] 5.2 **Issue** Costas carrier loop + proper symbol timing (helps mono stations with RDS). *M*
- [ ] 5.3 **Issue** PS confirmation, dynamic-PS detection, station names keyed by (frequency, PI). *M*
- [ ] 5.4 **Issue** FM region from Settings > Country: band, step, 50/75 µs, RDS vs RBDS PTY, call signs on/off. *M*
- [ ] 5.5 **Issue** PTY 31 Alert banner (and "TEST" for 30). *S-M*
- [ ] 5.6 **Issue** RT+ (artist/title, StationName.Short), CT, TP, AF method A. *L*

## Phase 6: AM (revised from the hardware report)
Details: [reports/SDR hardware and AM reception.md](reports/SDR%20hardware%20and%20AM%20reception.md), Phase 6 table.
- [x] 6.0 Correction: the NESDR SMArt v5 *does* do AM by direct sampling (Q branch, 0.1-25 MHz, diplexer at the SMA input). radio/README.md:26-27 is wrong (fix in 1.10).
- [ ] 6.1 **Issue** Bench-measure AM on the SMArt v5: Q-branch at 1,488,375 S/s centred 1125 kHz; MDS at 600/1000/1500 kHz; alias check at 27.1 and 88.0 MHz; strong-local overload, with and without the MW filter (7.2). *S-M*
- [ ] 6.2 **Issue** Direct-sampling mode switch in godwit-radio (explicit `rtlsdr_set_direct_sampling(dev,2)` / `(dev,0)`; AM→FM re-inits the tuner, measure latency). Depends on 4.4. *M*
- [ ] 6.3 **Issue** AM core: channel filter, envelope detector, carrier AGC (2-10 ms attack, 0.5-1 s hang, 0.5-2 s decay); widths 10/8/6/4.5/3 kHz; 10 kHz raster 530-1700. *M*
- [ ] 6.4 **PR** NRSC-1-C de-emphasis (pole 2122 Hz, zero 8700 Hz): at 48 kHz p = 0.75747, q = 0.32020, g = 0.35677. *S*
- [ ] 6.5 **Issue** Synchronous AM with sideband select (DSB/USB/LSB, auto by adjacent energy), envelope fallback. *M*
- [ ] 6.6 **Issue** 10 kHz heterodyne notch and AM noise blanker on wideband IQ. *M*
- [ ] 6.7 **Issue** HMI band switch, per-band presets, voice range 530-1700. *M*
- [ ] 6.8 **Issue (defer)** MW channeliser and live station list from one capture. *L*

## Phase 7: RF hardware, antennas, receiver
**Decided architecture (revised 2026-11-23): one NESDR SMArt v5 and a GPIO-switched antenna selector.** Godwit is built around one dongle that the apps take turns on: [radio/README.md](radio/README.md) ("There is one dongle, so it does one at a time"), [hmi/README.md](hmi/README.md) (`tunerreceiver`), and the `radioOn`/`aircraftOn`/`shipsOn`/`weatherOn` bindings in `hmi/qml/Hmi.qml`, where each runs only while none of the others holds the dongle. Weather Radio runs only while its app is open and decodes no alerts. So a second dongle isn't needed; only the two antenna feeds are:
- **Feed 1, AM/FM/weather band.** FM and weather band (162.400-162.550 MHz) through the tuner; AM by direct sampling on the Q branch, which shares the SMA input through the dongle's diplexer.
- **Feed 2, ADS-B 1090 MHz and AIS 162 MHz.**
- **Antenna selector (7.9):** an RF switch in front of the SMArt picks feed 1 for Radio and Weather, feed 2 for Aircraft and Ships. The HMI sets it when the dongle changes hands, before the next app opens it.

One dongle instead of two: half the heat (~1.5 W), one USB port and 270-330 mA instead of two, and no device-by-serial mapping or second-dongle sharing rules. **Alternative receiver: SDRplay through SoapySDR (7.6c)**, owner-installed: the RSPduo takes both feeds on its two tuners and needs no selector; the RSP1B sits behind the same selector as the SMArt. The two-dongle layout (one SMArt per feed, decided 2026-11-21) is kept as a fallback only if background reception is ever wanted (7.6b).

The Airspy HF+ Discovery recommendation in the hardware report is set aside: it adds a larger receiver that still covers only AM/FM. Its better front end remains the reference for what the SMArt gives up (preselection, 110/95 dB BDR vs an 8-bit converter), so front-end filtering and gain control (Phases 3-4, 7.2) matter more. TEF6686 stays an optional experiment.
- [ ] 7.1 **PR** Antenna and front-end guide in the docs (per-service filters, 50 Ω vs high-Z AM loss, 12 V phantom, coax losses, RV noise, why a 200 MHz LPF doesn't help). *S*
- [ ] 7.2 **Issue** Open-hardware MW band-pass: 520-1710 kHz, IL ≤ 2 dB, ≥ 60 dB at 26-30 MHz, ≥ 70 dB at 54-230 MHz. *M* **With one shared AM/FM/WB feed it can't sit in line** (it would block FM and weather band): it needs its own diplexer, MW band-pass on the HF arm and FM/weather on the VHF arm, recombined before the antenna selector (7.9), or a GPIO-switched bypass that could share the selector's control. Check what the dongle's own diplexer already rejects first (6.1).
- [ ] 7.3 **Issue** AM active antenna / JFET buffer (> 100 kΩ, < 10 pF in, 50 Ω out) + GPIO-switched DC injector. Recovers ~30 dB. *M*
- [ ] 7.4 **Issue** SoapySDR as the default device layer (revised 2026-11-23). All four helpers (radio, ADS-B, AIS, weather) open the receiver through SoapySDR instead of calling librtlsdr directly; the image ships SoapySDR and its RTL-SDR plugin, so the SMArt works out of the box exactly as today. *M-L*
  - **Can it be the default? Yes, technically and by licence:** SoapySDR (BSL-1.0) and SoapyRTLSDR (MIT) are open source, so shipping them meets CONTRIBUTING; librtlsdr stays underneath as today. What's closed is only SDRplay's API, which the image never contains: an owner who fits an RSP1B installs it and the MIT SoapySDRPlay3 plugin themselves, and the same helpers find it. Whether to take the dependency is the maintainers' call, so this goes to them as an issue first.
  - Device choice: first SoapySDR device found by default, or a `driver=`/`serial=` setting to pin one. With one receiver no serial mapping is needed (the old two-dongle reason for 7.4).
  - Gains: one shared way to find and open the receiver; other hardware without new capture code (RSP1B, Airspy, HackRF, LimeSDR); one interface for frequency, rate, gain stages, correction, antenna port and RTL modes such as direct sampling and bias-tee (helps 6.2 and 4.1; the RTL plugin can still deliver raw 8-bit samples for clip detection).
    - Doesn't fix: the 1.14 MS/s design (other devices need a resampler), audio clock drift (4.3), or any DSP quality item.
    - Costs: porting all four helpers off librtlsdr and retesting each; packaging SoapySDR and its RTL plugin for the AGL image; two new rows in `THIRD_PARTY_LICENSES.md`.
    - **Confirmed 2026-11-22:**
      - Licences (GitHub licence files): SoapySDR **BSL-1.0** (Boost), SoapyRemote BSL-1.0; SoapyRTLSDR, SoapySDRPlay3, SoapyAirspy, SoapyAirspyHF, SoapyHackRF all **MIT**. All maintained (pushed Oct-Nov 2026). Latest tags: SoapySDR `soapy-sdr-0.9.0`, SoapyRTLSDR `soapy-rtlsdr-0.3.4`. librtlsdr itself stays GPL-2.0+ underneath, as today.
      - SoapyRTLSDR source: opens by `serial=` device argument; stream formats CS8 (native, raw 8-bit, so clip detection works), CS16, CF32; settings `direct_samp`, `offset_tune`, `iq_swap`, `digital_agc`, `testmode`, `biastee`, `dithering`. **Note:** its `direct_samp` "0" means **Auto** (Q branch below 24 MHz on R820T), "1" I-ADC, "2" Q-ADC; in stock librtlsdr 0 means off. 6.2 must set it explicitly.
      - Recipes: Godwit's layer is Yocto **wrynose** and gets `rtl-sdr` from AGL's `meta-agl-demo`. **meta-sdr** (github.com/balister/meta-sdr, maintained, pushed 2026-11-20) supports wrynose and has `soapysdr_git.bb` (BSL-1.0, PV 0.8.1+git) and `soapyrtlsdr_git.bb` (MIT, PV 0.3.0+git), both pinned to older commits than the latest tags. meta-amateurradio has them too but only for kirkstone/scarthgap.
      - Packaging route: copy the two short recipes into `meta-godwit` (bumped to current tags) rather than adding all of meta-sdr, which pulls in meta-python, networking and filesystems layers and has its **own `rtl-sdr` recipe that would clash with AGL's**.
    - The maintainers decide whether the dependency is worth it.
- [ ] 7.5 **PR + bench** USB power (`usb_max_current_enable=1`), filtered 5 V, measure draw per device. *S*
- [ ] ~~7.6 Airspy HF+ Discovery backend~~ dropped with the two-SMArt decision; notes kept in the hardware report.
- [ ] 7.6c **Alternative receiver: SDRplay through SoapySDR** (owner-installed; depends on 7.4). Two models fit: *M*
  - **RSPduo (preferred alternative): no antenna selector needed.** Two tuners with their own inputs: feed 1 on Tuner 1 (and its Hi-Z port as a possible AM input, cf. 7.3), feed 2 on Tuner 2. Since only one app runs at a time, single-tuner mode is enough: the helper picks the tuner/antenna through SoapySDR when it opens the device, which replaces the GPIO switch (7.9) and its isolation question. To confirm: how SoapySDRPlay3 names the RSPduo's antennas and modes (single-tuner vs dual-tuner, which lowers the bandwidth per tuner), the isolation between Tuner 1 and Tuner 2 with strong FM on feed 1, its size in the chassis, and its draw and heat. Dual-tuner mode would also make the two-dongle fallback's background reception (7.6b) possible in one box.
  - **RSP1B: single input**, so it sits behind the antenna selector (7.9) like the SMArt.
  - **Why it can't replace the switch for everyone:** the RSPduo needs SDRplay's closed API, which the image can't ship. The default build stays SMArt + selector (7.9), so a stock head unit works with only open source; the RSPduo is the upgrade an owner chooses.
  - RSP1B figures (vendor listings): 1 kHz-2 GHz with no gaps, so AM needs no direct sampling; 14-bit ADC (native 14-bit from 2 to 6.048 MS/s, fewer bits above), built-in preselection filters, 0.5 ppm TCXO; about 185 mA typical without bias-T (vendor listings), less than a SMArt's 270-330 mA, so likely cooler.
  - Licence: SDRplay's API is closed with no redistribution right, so it is never in the image or a project dependency (CONTRIBUTING: 100% open source). The owner installs SDRplay API 3.15+ (Linux ARM64 build listed by SDRplay) and SoapySDRPlay3 (MIT), and Godwit's helpers find it through SoapySDR.
  - Rates: run it at 2.28 MS/s (2 × the RTL's 1.14 MS/s) or another rate the helpers can decimate from; the helpers need a resampler stage for rates that aren't a multiple (7.4 "Doesn't fix").
  - To confirm on the bench: API 3.15 on the Pi 5 image (SDRplay tests only on Ubuntu LTS, so AGL is untested), USB draw and heat beside the SMArt, AM/FM quality against the SMArt (6.1).
- [ ] ~~7.6a Sharing SMArt B between Aircraft and Ships~~ not needed with one dongle: the existing `tunerHeld` hand-over already covers it (only comes back with the two-dongle fallback).
- [ ] 7.6b **(Deferred: two-dongle fallback only.** Godwit doesn't listen in the background, so this would be a new feature, not a fix; raise it with the maintainers only if they want background weather alerts.) AIS and weather band together on SMArt B, so weather can be watched while SMArt A plays the radio. Both AIS channels (161.975, 162.025 MHz) and all seven NOAA channels (162.400-162.550 MHz) fit in 575 kHz. Today `godwit-ais` and `godwit-wxradio` each tune their own 288 kS/s block. A combined helper would capture 1.152 MS/s (4 × 288k) centred near 162.2625 MHz, so the DC spike falls between channels (137.5 kHz from 162.400, 237.5 kHz from 162.025), then mix and decimate by 4 into the existing AIS and weather DSP unchanged. Weather would then also need feed 2's antenna to hear 162.4-162.55 MHz (an AIS antenna does). *M*
  - Only while Ships holds SMArt B; Aircraft (1090 MHz) still takes turns with it (7.6a).
  - Watching for alerts needs alert decoding the weather helper doesn't have yet: the 1050 Hz warning tone and SAME headers (event code, area, time). Its own item when we get there. *M*
  - Antenna: the 162 MHz part of feed 2 should be a marine/weather whip or the low band of a dual-band antenna; check the FM band-stop in 7.7 still passes 162.4-162.55.
- [ ] 7.7 **Issue** 162 MHz and 1090 MHz paths: FM band-stop or 155-164 MHz BPF; SAW-filtered 1090 LNA at the antenna; LMR-240. *S-M*
- [ ] 7.9 **Issue** Antenna selector: open-hardware SPDT RF switch (DC-1.2 GHz at least, ≥ 40 dB isolation at 100 MHz and 1090 MHz so strong FM doesn't leak into the ADS-B feed, low loss), one GPIO from the Pi; the HMI sets it before the next app opens the dongle (feed 1: Radio, Weather; feed 2: Aircraft, Ships). Keep bias-T/LNA power for feed 2 routed so it reaches the 1090 LNA only. Default position on boot and on fault: feed 1. *M*
- [ ] 7.8 **Issue (experimental)** TEF6686 tuner backend via an MCU running the existing GPL firmware as a separate work; Godwit implements a protocol client only. *L*
- Antenna note: **Taoglas MA1045** (permanent roof mount: Ø19 mm centre hole + two Ø7.2 mm holes 55 mm apart) covers AM/FM/weather band (amplified, specs unpublished), 2× GNSS (L1 + BeiDou B1), SiriusXM, LTE on one port (no band 71/41, no C-band, no MIMO); **not 1090 MHz**. Bench-test its amplified AM/FM output into the SDR before choosing it.
- Antenna note: **Taoglas MA1047** (7-in-1 shark fin, 96.5 × 80 × 60 mm, datasheet SPE-24-8-285-C): 2× 5G/4G MIMO 617-5925 MHz (band 71, 41, C-band; second port weak, 6-7 % at 3.3-5 GHz), 2× Wi-Fi 2.4/5 GHz, 1× active GNSS (L1/G1/E1/B1 + SBAS, 30 dB LNA), active AM/FM/WB (no specs), DAB (not used in North America); no SiriusXM; **not 1090 MHz** (cellular gap 960-1427 MHz). Out of stock until May 2027 (per your note, not confirmed). Same AM/FM bench test applies.

## Phase 8: NRSC-5 IBOC digital radio
Naming rule: say "NRSC-5" / "IBOC" / "supplemental program services (SPS)"; never the trademarked brand name or brand-style channel labels, in code, UI, docs, issues or PRs.
- [ ] 8.1 **Issue (first)** Packaging, naming and legal posture: opt-in nrsc5 (GPL-3.0, pinned tag) as a separate helper; no patent licence implied. *S*
- [ ] 8.2 **Issue** FM capture at 1,488,375 S/s, decimate by 2 to 744,187.5 S/s, feeding both the analog demod and nrsc5. **Note:** changes the FM rates, so the de-emphasis coefficients (1.1) and tests (0.2) must be parameterised by rate, not hard-coded to 57 kHz. *M*
- [ ] 8.3 **Issue** `godwit-nrsc5d` GPL helper: I/Q over a Unix socket, PCM + JSON events out. *M-L*
- [ ] 8.4 **Issue** IBOC presence detector (sideband power at ±129-198 kHz). *S-M*
- [ ] 8.5 **Issue** Analog/digital blend: ~6 s delay buffer, cross-correlation alignment cached per station, level match, hysteresis crossfades. Depends on 2.1 and 4.3. *L*
- [ ] 8.6 **Issue** HMI for SPS ("101.1 · 2"), title/artist, album art (sandboxed). *M*
- [ ] 8.7 **Issue (defer)** AM IBOC MA1, experimental. *M*

---

## Open questions to settle
- PipeWire version in the AGL image (A.2)
- ±2 Hz pilot tolerance: confirm in ITU-R BS.450
- ~~NRSC-1-C de-emphasis constants~~ settled: pole 2122 Hz, zero 8700 Hz (6.4)
- SMArt v5 MW sensitivity and alias levels (6.1, bench)
- SMArt v5 on the vehicle 5 V rail: draw (270-330 mA per Nooelec) and spurs (7.5); RSP1B for comparison (~185 mA typical)
- **SMArt v5 heat in the chassis** (first unit received 2026-11-23: FM works, runs very hot). About 1.5 W; the aluminium case is the heatsink. Risks in a closed head unit: heat build-up beside the Pi 5, oscillator drift (matters more for weather band and AIS than FM/AM), and tuner PLL unlock when hot (mostly at 1090 MHz, for Aircraft). Bench procedure:
  1. Thermocouple on the case for an hour, first in open air, then in an enclosure close to the chassis volume.
  2. Current draw on a USB meter, bias-tee off and on.
  3. Frequency offset of a known FM carrier (or the 19 kHz pilot) over the warm-up, and a PLL-lock check at 1090 MHz when hot.
  - Mounting to try: clamp the dongle to the chassis wall with a thermal pad and keep it away from the Pi's SoC on a short USB extension. Grounding the case to the chassis may also help RF noise (see the bench RF-noise note).
  - If the SMArt can't be made to run cool and stable enough, that triggers the SDRplay alternative in 7.6c.
- Antenna for feed 2: one dual-band 162/1090 MHz antenna, or two antennas through a combiner? Neither Taoglas combo covers 1090 MHz.
- TEF6686 vs SDR on the road: nobody has measured it (7.8)
- Taoglas MA1045 AM/FM amplifier gain and output impedance (ask Taoglas or bench-test)
- Which hardware you want to buy first (see the report's Good/Better/Best tiers)
- RT+ application ID (expected 0x4BD7): confirm before 5.6
- ~~Full IEC 62106 E.1 character table source~~ settled: IEC 62106-4:2018 Table 4 (1.7)
- ~~Hardware on hand~~ settled: **test bench = the project's Raspberry Pi 5 build** (video 2026-11-20; `os/boards/rpi5-dsi`): Pi 5 8 GB on NVMe, Waveshare 10.1" DSI panel, 2-CH CAN FD HAT, two ESP32-S3 knob dials, Sound Blaster Play! 3 USB audio, 12 V bench supply; plus the NESDR SMArt v5 (first unit received 2026-11-23, FM checked) and a Taoglas combination antenna on order, and your own test equipment. Radio notes for it:
  - **USB ports:** the Pi 5 has four; the two dials and the sound card take three, and the SMArt takes the fourth.
  - **USB current:** the image sets `usb_max_current_enable=1` (1.6 A total for the USB ports, assuming a 5 A supply). The SMArt is 0.27-0.33 A by Nooelec's figures, plus the dials and sound card: measure the total (7.5).
  - **Audio clock:** audio leaves through the Play! 3's own clock, so the dongle-vs-audio drift (4.3) is real on this bench. Good place to measure it.
  - **RF noise:** Pi 5, DSI panel and switch-mode supplies sit right beside the dongle. Expect MW (AM) to suffer most. Put the SMArt on a USB extension away from the stack, ferrites on the leads, and capture a noise baseline with the antenna disconnected (useful for 6.1 and the noise blanker, 3.x).
