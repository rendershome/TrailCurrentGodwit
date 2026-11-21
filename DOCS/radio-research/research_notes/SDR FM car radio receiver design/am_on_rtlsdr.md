# AM (Medium Wave, 530–1710 kHz) Reception on RTL-SDR for the TrailCurrent Godwit Head Unit

## Hardware options and their quality limits (V3 direct sampling, V4 upconverter, external upconverters, R820T dongles)

### Takeaway
The RTL-SDR Blog V4 (R828D plus a built-in SA612 upconverter on a 28.8 MHz LO, with a triplexer and a switchable AM/FM/DAB notch) is the clear single-dongle choice for MW. Its published MDS at 1 MHz is about -134 dBm, against about -109 dBm for V3 direct sampling, roughly 25 dB better. It also has no Nyquist folding at 14.4 MHz. A stock R820T dongle such as the NESDR SMArt v5 cannot tune MW without an external upconverter (SpyVerter R2 or Ham It Up), and V3 direct sampling is unfiltered and noticeably deaf below 2 MHz.

### Cited Findings
- V4 datasheet specs: R828D tuner, 8-bit RTL2832U ADC, 500 kHz to 1.766 GHz, 2.56 MHz stable bandwidth (up to 3.2 MHz with drops), 50 Ω input, 250–270 mA draw, "Upconverter with 28.8 MHz LO" for HF, 1 PPM TCXO, 4.5 V / 180 mA software-switchable bias tee, "Triplexor with switchable notch" front end. — [RTL-SDR Blog V4 Datasheet V1.0](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- V4 HF path: the built-in upconverter replaces direct sampling, so there is "no more Nyquist folding of signals around 14.4 MHz", better sensitivity, and adjustable HF gain. The lower tuning limit stays at 500 kHz, and "very strong reception may still require front end attenuation/filtering." — [V4 Datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- V4 MDS graph, 3 dB MDS read from the plot: about -134 dBm at 1 MHz for V4 versus about -109 dBm for V3. About -143 dBm (V4) versus -123 dBm (V3) at 2 MHz. The difference plot shows V4 about 25 dB better at 1 MHz, falling to about 15–20 dB across HF. V4 is a few dB worse in parts of VHF/UHF. — [V4 Datasheet p.3](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf). Note that MW (0.53–1.7 MHz) sits on the steep low-frequency end of both curves, so sensitivity degrades toward 530 kHz.
- V4 triplexer splits the input into HF (0–28 MHz), VHF (28–250 MHz) and UHF (250 MHz–1.766 GHz). The R828D open-drain pin switches a notch for broadcast AM, FM and DAB, adding about 5–10 dB of attenuation. The notch is ON when tuned outside those bands and OFF when tuned inside them, so it does not attenuate MW while you are listening to MW. — [rtl-sdr.com V4 launch article](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/); the datasheet says the notches "only attenuate by a few dB" — [V4 Datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- V4 two-tone desensitisation test (blocker at 95 MHz): isolation improved about 8 dB within the same triplexer band and 28–43 dB out of band compared with V3. This is relevant because strong local FM would otherwise desensitise the dongle. — [V4 Datasheet p.4–5](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- V4 needs the RTL-SDR Blog drivers. Without them you may get no signals, signals on the wrong frequency, or corrupted signals. The drivers are backward compatible with V3. Direct sampling on V4 "yields no results"; you simply tune HF directly. — [RTL-SDR Blog V4 user guide](https://www.rtl-sdr.com/V4/). An update says the default Osmocom branch now supports V4 — [V4 launch article](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/). The datasheet says Linux system drivers must be updated, and that SDR++, SDR#, SDR-Console V3 and GQRX ship with up-to-date drivers — [V4 Datasheet p.2](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- V4 launch price: US$39.95 with the antenna set, US$29.95 for the dongle alone. A later comment attributes a price increase to a component change and dwindling R828D stock. — [V4 launch article](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)
- V3 direct sampling (Q-branch) covers about 500 kHz–28.8 MHz. The RTL-SDR Blog driver switches to it automatically below 28.8 MHz. Other drivers need the Q-branch selected manually (`direct_samp=2`). — [RTL-SDR Blog V3 user guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/)
- V3 aliasing: the ADC samples at 28.8 MHz, so signals at 0–14.4 MHz appear as images when tuning 14.4–28.8 MHz and vice versa. There is no on-board filtering, so an external LPF/HPF or a band filter is needed. — [V3 user guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/)
- Newer V3 batches deliberately add attenuation below 2 MHz. This reduces MW reception but helps AM-overload problems higher up. Strong local AM can still overload the receiver. MW reception with a proper HF antenna is "usually fine" because the stations are high power and local. — [V3 user guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/)
- V3 Batch 1 bias tee added a spur in direct sampling mode; Batch 2 fixed it. — [V3 user guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/)
- Field reports on V4 MW are mixed. An Elektor reviewer received numerous European MW stations at night on an SW antenna. A RadioReference user living near two strong AM stations reported "barely" getting 980 kHz with the rest of the band flat; a dipole helped, but mostly for locals. A commenter on the launch article called the V4 "nearly deaf" below 3.5 MHz, which the article does not address. — [Elektor review](https://www.elektormagazine.com/review/rtl-sdr-blog-v4-better-than-v3-review); [RadioReference thread](https://forums.radioreference.com/threads/zero-success-with-rtl-sdr-v4-on-medium-wave.492023/latest); [V4 launch article](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)
- SpyVerter R2: 1 kHz–60 MHz input, 120 MHz LO, H-mode mixer rather than the ADE-1 DBM used by most Ham It Up versions, about 10 mA from a bias tee, launch price US$49. A retailer lists 5.2 dB typical conversion loss and +35 dBm IIP3. — [Airspy SpyVerter R2](https://airspy.com/spyverter-r2/); [rtl-sdr.com SpyVerter R2 review](https://www.rtl-sdr.com/a-review-of-the-spyverter-r2/)
- NooElec Ham It Up Plus: extends coverage down to about 300 Hz, needs a receiver tuning 125–190 MHz for full 300 Hz–65 MHz coverage, has a 125 MHz TCXO and an added noise-source circuit. The older Ham It Up v1.3 was US$41.95 without enclosure or TCXO (c. 2016). — [NooElec Ham It Up Plus datasheet](https://www.mikrocontroller.net/attachment/492866/Ham_It_Up_Plus.pdf); [rtl-sdr.com Ham It Up Nano note](https://www.rtl-sdr.com/?p=41417)

### Inferences
- For Godwit, swapping the NESDR SMArt v5 for an RTL-SDR Blog V4 is the lowest-complexity way to add AM. It keeps one dongle, the frequency map is unchanged (drivers handle the 28.8 MHz LO), and it gains the triplexer's protection from strong FM. The cost is a small VHF/UHF MDS penalty (about 2–3 dB on some bands), which matters for ADS-B and AIS margins.
- Keeping the NESDR SMArt v5 with an external upconverter (SpyVerter R2 or Ham It Up) works for MW. However, the upconverter sits in series with the antenna, and the passive SpyVerter bypass/upconvert selection is a manual switch. That breaks the single-dongle multi-service model unless you add an RF switch, so it is less attractive than V4.
- V3 direct sampling should be ruled out for a broadcast-quality goal. It has no anti-alias filter and attenuates below 2 MHz, and its about -109 dBm MDS near 1 MHz is roughly 25 dB worse than V4.
- With an 8-bit ADC, a strong local AM carrier (tens of mV at the antenna near a 50 kW site) plus the rest of the MW band all reach the mixer at once. A preselector or tracking filter for MW, plus careful RF/IF gain staging (not maximum gain), will matter more for quality than nominal MDS. This is engineering judgement and has not been measured for V4.

### Gaps
- No published third-party lab measurement of V4 dynamic range, IP3 or blocking specifically across 530–1710 kHz; the MDS points come from the vendor datasheet only.
- Current 2026 retail prices for V4, SpyVerter R2 and Ham It Up Plus were not verified (launch prices only).
- The UDXF V4 review PDF (https://udxf.nl/RTL-SDR-V4-Review.pdf) was too large to fetch and may contain MW/LW measurements.
- No measured data on how a Nooelec NESDR SMArt v5 behaves on MW through an upconverter, compared with V4.

## In-vehicle MW reception: antenna, vehicle noise, why AM is being dropped, and the AM Radio for Every Vehicle Act

### Takeaway
A car whip is electrically tiny at MW (high-impedance and capacitive), so feeding it straight into a 50 Ω SDR wastes most of the signal. It needs a high-impedance FET buffer (an active antenna) powered over coax, for example from the V4's 4.5 V bias tee. EV propulsion electronics are the main reason several automakers dropped AM. As of mid-September 2026 the AM Radio for Every Vehicle Act had passed the House by voice vote, and the Senate companion (S. 315) was pending.

### Cited Findings
- An electrically short whip has a very high source impedance; one example is a 1 m whip at 10 kHz near 2 MΩ, and connecting it straight to 50 Ω would lose about 114 dB. The standard fix is a JFET source follower to sense the whip voltage, followed by an emitter follower to drive the 50 Ω coax. Input capacitance (for example about 29 pF in one ARRL design) matters as well. — [Search-summary of ARRL TIS active antenna article](https://www.arrl.org/files/file/Technology/tis/info/pdf/0109031.pdf); [PA3FWM mini-whip design](https://pa3fwm.nl/projects/miniwhip)
- Hi-Z-to-50 Ω matching transformers have been used with telescopic whips on SDRs to improve HF/SW reception. — [rtl-sdr.com Hi-Z transformer article](https://rtl-sdr.com/a-hi-z-to-50-ohm-impedance-matching-transformer-for-improving-hf-sw-reception)
- V3/V4 bias tee is 4.5 V, up to 180 mA continuous, software switchable (`rtl_biast -b 1`) or forced on via EEPROM (`rtl_eeprom -b 1`). Do not use it into a DC-shorted antenna. — [V4 user guide](https://www.rtl-sdr.com/V4/); [V3 user guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/)
- EV interference sources are motors, inverters and HV batteries. Mitigation options are shielded cables, filters, active noise cancellation and component placement, each adding cost or weight. — [Ward's / Automotive Dive](https://www.wardsauto.com/news/archive-auto-installing-am-radio-evs-cost-automakers-38b-bmw-ford-tesla/699172/); [Radio World](https://www.radioworld.com/?p=122690)
- Center for Automotive Research (2023) estimated $3.8 B over seven years for EMI mitigation. An unnamed automaker cited $35–50 per vehicle for shielding and $15–20 for filtering. Critics say the figure lumps several modules together, and the CBO reportedly estimated far less. — [Yahoo Autos](https://autos.yahoo.com/automaker-group-predicts-keeping-am-005600204.html); [Radio World](https://www.radioworld.com/?p=122690)
- Eight automakers, including BMW, Ford and Tesla, dropped AM; Ford later reversed. VW cited shielding weight as a drag on range. The NAB says there is little public measurement data on EV interference with AM. — [Ward's](https://www.wardsauto.com/news/archive-auto-installing-am-radio-evs-cost-automakers-38b-bmw-ford-tesla/699172/); [Carscoops](https://www.carscoops.com/2023/11/mandating-analog-am-radio-in-evs-could-cost-automakers-billions-study-claims/); [IEEE Spectrum](https://spectrum.ieee.org/am-radio-ev-interference)
- An AM radio RFI technical report has been published (Radio World, 2023). — [AM Radio RFI Technical Report](https://www.radioworld.com/wp-content/uploads/2023/11/AM-Radio-RFI-Technical-Report.pdf) (not reviewed in detail)
- AM Radio for Every Vehicle Act: the House passed it (H.R. 979, as amended) by voice vote on 15 Sept 2026. It directs DOT to require AM reception, including "digital audio AM stations", as standard in new passenger vehicles at no extra charge, with an eight-year sunset (cut from 10). It still needs identical Senate passage (S. 315, Markey/Cruz) and signature; the President has said he supports it. CTA opposes it. — [Radio World, 15 Sept 2026](https://www.radioworld.com/news-and-business/headlines/u-s-house-passes-bill-to-require-am-radio-in-new-cars); [SBE](https://sbe.org/house-am-radio-every-vehicle-act/); [NRB](https://nrb.org/the-am-radio-act-finally-gets-its-vote/)

### Inferences
- Recommended Godwit antenna path for MW: a short active whip or the vehicle's existing AM/FM mast, feeding a JFET high-Z buffer near the antenna base, powered by the bias tee or a separate filtered 12 V feed. An MW band-pass (about 500–1800 kHz) or a separate MW/HF port should come before the dongle. Because the V4 has one SMA input feeding its triplexer, a diplexed AM/FM antenna amplifier (common in OEM glass/shark-fin systems) could feed both bands on one cable.
- Noise in ICE vehicles (ignition, alternator whine), LED lighting drivers, and switch-mode USB/DC-DC converters is mostly impulsive or comb-like. This argues for a noise blanker and for running the head unit's own SMPS (Pi 5 / Q6A supply) at a switching frequency outside MW or with spread-spectrum disabled, with careful layout. This is general engineering practice and not sourced here.
- The House text mentions "digital audio AM stations", which implies all-digital or hybrid HD AM support may become part of a compliant receiver. For a hobby/open-source head unit this has no regulatory force, but it frames user expectations.

### Gaps
- No Senate action after 15 Sept 2026 was found; the final status as of Nov 2026 is unverified (check congress.gov for H.R. 979 / S. 315).
- No measured MW noise-floor data for specific vehicles (ICE vs EV) was found.
- No published SDR-specific car-whip active-antenna design with measurements was found.

## AM receiver DSP design (detection, bandwidth, de-emphasis, AGC, noise blanker, notch, channel steps, C-QUAM, NRSC-5 IBOC)

### Takeaway
Implement synchronous AM (a PLL-locked carrier) with DSB/USB/LSB selection as the primary detector and envelope detection as a fallback. Pair it with carrier-based AGC, variable IF bandwidth (about 4–10 kHz audio, matching NRSC-1's 10 kHz audio limit), NRSC-1 de-emphasis, a noise blanker and an automatic heterodyne notch. SDRangel's AM demodulator is a working open-source reference for SAM with sideband selection. SDR++'s core AM demodulator is envelope-only, with selectable carrier or audio AGC. nrsc5 decodes AM NRSC-5 IBOC (`--am`) from RTL-SDR.

### Cited Findings
- NRSC-1 (1988; revisions 1-A 2007, 1-B 2012, 1-C April 2018) specifies AM pre-emphasis and de-emphasis and a 10 kHz analog AM audio bandwidth. The pre-emphasis is a "modified 75 µs" curve with a second time constant limiting boost to 10 dB at 10 kHz. The receiver de-emphasis is the complementary curve (Section 6 of 1-C). — [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf); [NRSC archive](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/archive.asp)
- NRSC-2 (current revision 2-C, April 2018) limits AM to a nominal 20 kHz occupied RF bandwidth. NRSC-2-compliant hardware is the primary means for stations to meet 47 CFR §73.44. — [NRSC-2-A archive](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-2-a.pdf); [FCC 89-118](https://docs.fcc.gov/public/attachments/FCC-89-118A1.pdf)
- SDRangel AM demod (`amdemodsink.cpp`):
  - The default path is an envelope detector, `sqrt(magsq)`, with a slow volume AGC that both removes DC and normalises gain (window about 100 ms, or 250 ms with the PLL on).
  - The optional synchronous AM path pre-filters the signal, then runs a PLL (`computeCoefficients(0.05, 0.707, 1000)`) and rotates the signal by the recovered carrier.
  - It then applies a DSB filter (width 2×RF BW) or an SSB filter for SAM-USB/LSB (width RF BW), followed by a separate sync-AM AGC (sampleRate/4, sampleRate/8).
  - Audio is a 301-tap LPF at AF BW/2, or an optional 300 Hz–AF BW/2 bandpass. The squelch has a 50 ms smootherstep fade.
  - The fetched summary flagged a possibly fragile buffer-index reset in the sync path.
  - — [SDRangel amdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodam/amdemodsink.cpp)
- SDR++ core AM demod (`core/src/dsp/demod/am.h`) is envelope-only (`volk_32fc_magnitude_32f`), with no PLL. It offers selectable CARRIER AGC (normalises the complex IQ before detection) or AUDIO AGC (after detection and DC block), a DC blocker, and an FIR LPF at bandwidth/2 with a 10% transition. Attack and decay are user-settable. — [SDR++ am.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/demod/am.h)
- nrsc5 receives NRSC-5 IBOC from an RTL-SDR or I/Q files. `--am` selects AM mode (default is FM). AM I/Q file rate is 46512 SPS for cs16/cf32 (1488375 SPS for cu8). Options include `-T` for bias tee and `-D` for direct sampling (1 = I, 2 = Q). — [nrsc5 README](https://github.com/theori-io/nrsc5)

### Inferences
- Detector: SAM gives lower distortion under selective fading (skywave at night) than an envelope detector, and SAM-USB/LSB lets the listener reject a first-adjacent splatter on one side. That fits a broadcast engineer's expectation. Use envelope detection as a fallback when the PLL cannot lock on weak signals; SDRangel's structure is a good template. A PLL loop bandwidth of tens of Hz is typical for broadcast carriers; SDRangel's parameters are a starting point, not a recommendation.
- AGC: derive gain from the carrier level (post-PLL I channel or a narrow carrier filter) rather than from audio. This preserves modulation dynamics and avoids pumping. Use a fast attack (a few ms) and slow decay (hundreds of ms), with hang. SDR++'s CARRIER mode is the same idea.
- Bandwidth: offer 10 kHz audio (20 kHz RF, matching NRSC-1/2) for strong locals and narrower settings (about 6, 4.5, 3 kHz audio) for night/adjacent interference. Auto-select based on adjacent-channel energy measured from the wideband IQ.
- De-emphasis: apply the NRSC-1 de-emphasis (the inverse of the modified 75 µs curve) for full-band settings. At narrow bandwidths the IF already rolls off highs, so de-emphasis should be reduced or switched off. Exact pole/zero values must come from the NRSC-1-C PDF (see Gaps).
- Heterodyne notch: in North America, co-channel/adjacent carriers produce whistles at 10 kHz (and offsets of a few Hz from carrier drift). In 9 kHz regions the beats are at 9 kHz. An automatic adaptive notch (LMS) or a fixed 10/9 kHz notch in the audio path, plus the 10 kHz edge of NRSC-1 LPF, addresses this.
- Channel steps: 10 kHz from 530–1700 kHz (Americas); 9 kHz from 531–1602 kHz elsewhere. Make the step region-configurable. These are widely known values, not sourced here.
- Noise blanker: run it pre-demodulation on wideband IQ, before channel filtering so impulses are not stretched. Use a threshold relative to a running average and blank or interpolate samples. This is crucial for ignition and LED-driver impulses.
- C-QUAM AM stereo: few US stations still transmit it. A software C-QUAM decoder (L+R from envelope, L-R from the quadrature component corrected by the cosine of the phase angle, 25 Hz pilot detection) is feasible on top of the SAM PLL. It is low priority.
- NRSC-5 IBOC AM hybrid: digital carriers occupy about ±5–15 kHz around the analog carrier. For analog listening, a 10 kHz audio bandwidth will include some hybrid digital-sideband noise; narrower IF or SAM sideband selection mitigates this. nrsc5 `--am` can be integrated as an optional decoder. Note that NRSC-5 IBOC is licensed by Xperi, and nrsc5 is a reverse-engineered open-source implementation.

### Gaps
- Exact NRSC-1-C de-emphasis time constants and audio LPF attenuation breakpoints could not be extracted (the PDF was binary and no PDF tool was available locally). Read Section 6 of [NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf) directly.
- No sourced paper on synchronous AM PLL design for broadcast was retrieved. The CuteSDR (Moe Wheatley) SAM implementation and Gqrx/rtl_fm AM code were not reviewed in this pass.
- C-QUAM station counts and the status of an open-source C-QUAM decoder were not verified.
- No source was retrieved on nrsc5 AM performance with V4 or an upconverter.

## Single-dongle constraint: switching FM/AM/other services, latency, and wideband multi-station capture

### Takeaway
On a V4, AM and FM are just retunes of the same device; the driver handles the HF upconverter path, and there is no direct-sampling toggle. Switching costs a retune plus buffer flush, but the dongle can only serve one band at a time, so AM listening blocks ADS-B/AIS/NOAA just as FM does. A 2.4–2.56 MHz capture can cover most or all of the MW band (530–1710 kHz is 1.18 MHz wide), allowing simultaneous demodulation of every MW station and an instant-tune "band scope".

### Cited Findings
- V4 tunes HF directly with no direct sampling or offset; the drivers manage the 28.8 MHz LO upconversion. — [V4 launch article](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/); [V4 user guide](https://www.rtl-sdr.com/V4/)
- V4 stable bandwidth is 2.56 MHz (up to 3.2 MHz with drops). — [V4 Datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- With the RTL-SDR Blog driver, V3 automatically enters direct sampling below 28.8 MHz. Other drivers need `direct_samp=2` set, a mode change rather than a pure retune. — [V3 user guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/)
- The V4 notch switches automatically depending on whether the tuned frequency is in the AM/FM/DAB bands. — [V4 launch article](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)

### Inferences
- A single 2.4 MS/s capture centred at about 1.12 MHz covers 530–1710 kHz entirely, leaving DC offset/spur handling to be managed (place the LO so DC falls between channels, for example on a 5 kHz offset). A polyphase channelizer (10 kHz grid, 118 channels) can demodulate all stations at once on a Pi 5 or Dragon Q6A. That enables instant channel changes, a live signal-strength list, background seek, and recording.
- With an 8-bit ADC, capturing the whole MW band means all locals share the ADC range. If strong locals exist, gain must be reduced, which raises the noise floor for weak stations. A narrower capture (for example 1 MS/s) around the wanted station, with tuner IF filtering, can give better weak-signal performance. Offer a "full-band" mode and a "DX" mode.
- Retuning between FM (about 100 MHz) and MW on V4 is a frequency change through the R828D's input/triplexer path. Expect latency similar to an FM-to-FM retune plus PLL settle and a buffer flush (tens of ms); this has not been measured. Upconverter-based setups (SpyVerter/Ham It Up on R820T) need a frequency offset in software and a bypass switch, adding complexity.
- If AM listening must coexist with continuous ADS-B/AIS monitoring, a second dongle is the only real option. A V4 dedicated to AM/HF plus the existing NESDR for VHF/UHF services would also let each antenna be optimised.

### Gaps
- No measured retune latency for V4 between FM and MW was found.
- No source was retrieved on CPU load for full-MW-band channelizers on Pi 5 or Dragon Q6A.

## Open-source references

### Takeaway
The most useful code references are SDRangel's AM demod for SAM with sideband selection, SDR++ for a clean envelope plus carrier-AGC design, and nrsc5 for AM IBOC.

### Cited Findings
- SDRangel AM demodulator sink (envelope + PLL synchronous AM, DSB/USB/LSB): [plugins/channelrx/demodam/amdemodsink.cpp](https://github.com/f4exb/sdrangel/blob/master/plugins/channelrx/demodam/amdemodsink.cpp)
- SDR++ AM demodulator (envelope, carrier/audio AGC, DC block, FIR LPF): [core/src/dsp/demod/am.h](https://github.com/AlexandreRouma/SDRPlusPlus/blob/master/core/src/dsp/demod/am.h)
- nrsc5 NRSC-5 IBOC decoder with `--am` mode for RTL-SDR: [theori-io/nrsc5](https://github.com/theori-io/nrsc5)
- RTL-SDR Blog drivers (required for V4, include `rtl_biast`): see the [V4 user guide](https://www.rtl-sdr.com/V4/)

### Inferences
- Gqrx (GNU Radio based) and rtl_fm (`-M am`) offer only basic envelope AM. CuteSDR historically included a SAM detector. These are known from general SDR community knowledge and were not verified in this pass.

### Gaps
- Gqrx, CuteSDR, rtl_fm and SDR# SAM source code were not fetched or reviewed, and it was not verified whether SDR++ has added a SAM mode in a 2025–2026 release or plugin.
