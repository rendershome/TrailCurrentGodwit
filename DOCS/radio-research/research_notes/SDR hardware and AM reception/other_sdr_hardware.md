# Non-SDRplay SDR hardware for a high-quality AM/FM head unit (TrailCurrent Godwit)

Research date: 2026-11-21. Many independent measurements are from 2017–2022. No newer independent lab reports turned up, so treat the older ones as dated but still the best available. Manufacturer figures are labelled as such.

## Q1. Per-device specifications (ADC, bandwidth, coverage, MW support, front end, dynamic range, TCXO, power, price)

### Takeaway
On paper and in the published tests, the Airspy HF+ Discovery is the only hobby SDR here built for broadcast-band AM and FM. It has preselection for both bands, roughly 110 dB (HF) and 95 dB (VHF) blocking dynamic range, and native MW. Its trade-off is a usable bandwidth of about 660 kHz. The Airspy R2 and Mini are wideband (10 MHz and 6 MHz) 12-bit receivers built on the same R820T2 tuner family as the RTL-SDR. They have no native MW. The RTL-SDR Blog V4 is the best cheap all-rounder, with a triplexer, AM/FM/DAB notches and an HF upconverter.

### Cited Findings
**Airspy HF+ Discovery (manufacturer specs)**
- Coverage is 0.5 kHz–31 MHz and 64–260 MHz. The page says the extended VHF range up to 260 MHz uses a second Band-III tuner with reduced performance. It does not cover 1090 MHz. — [Airspy](https://airspy.com/airspy-hf-discovery/)
- It uses two sigma-delta ADCs at up to 36 MSPS. The 18-bit DDC output is scaled to 16-bit over USB, and Airspy claims "22-bit" resolution in a 3 kHz channel. No conventional ADC bit or ENOB figure is published. — [Airspy](https://airspy.com/airspy-hf-discovery/)
- Bandwidth: up to 660 kHz is alias- and image-free inside a 768 ksps IQ stream. — [Airspy](https://airspy.com/airspy-hf-discovery/)
- Dynamic range and IP3: 110 dB blocking dynamic range on HF and 95 dB on VHF. IP3 is +15 dBm on HF and +13 dBm on VHF, both at maximum gain. Airspy claims 150+ dB combined hardware and software selectivity. — [Airspy](https://airspy.com/airspy-hf-discovery/)
- MDS (500 Hz bandwidth): −140 dBm on HF (0.02 µV at 15 MHz), −141.5 dBm in the 64–118 MHz FM broadcast band, and −141 dBm at 118–260 MHz. — [Airspy](https://airspy.com/airspy-hf-discovery/)
- Front end: low-loss preselection and a high-linearity tunable RF filter. The HF preselector has 4 banks with corners near DC, 5, 10, 17 and 31 MHz. There is a separate VHF preselector and a separate FM preselector with JP, US, EU and OIRT variants, plus IF filters optimized for FM. — [Airspy](https://airspy.com/airspy-hf-discovery/)
- Clock: 0.5 ppm, low phase noise, with 1 ppb adjustment. — [Airspy](https://airspy.com/airspy-hf-discovery/)
- Price is about $169–$199 in 2026 roundups. — [HamDeals, Mar/Nov 2026](https://hamdeals.com/blog/sdr-dongles-ranked/)
- Power: 5 V over USB, with the current draw not listed. The original HF+ is listed at a maximum of 140 mA. — [RigPix Discovery](https://rigpix.com/miscrx/airspy_hfplusdiscovery.htm); [RigPix HF+](https://rigpix.com/miscrx/airspy_hfplus.htm)

**Airspy R2 (manufacturer specs)**
- Coverage is 24–1700 MHz natively, and down to DC only with the SpyVerter add-on. It has no native MW. — [Airspy R2](https://airspy.com/airspy-r2/)
- ADC: 12-bit at 20 MSPS, 10.4 ENOB. 10 MSPS IQ output gives up to 9 MHz alias- and image-free. A 2.5 MSPS mode exists for low-power hosts. — [Airspy R2](https://airspy.com/airspy-r2/)
- Specs: 70 dB SNR, 95 dB SFDR, 35 dBm IIP3 front end (manufacturer figure), and 3.5 dB noise figure from 42 to 1002 MHz. It has tracking RF filters, a 0.5 ppm clock and a 10–100 MHz external clock input. — [Airspy R2](https://airspy.com/airspy-r2/)

**Airspy Mini**
- It has a 12-bit ADC at 20 MSPS internally, with 10, 6 or 3 MSPS IQ output. It launched at $99, and a later listing showed $149. — [rtl-sdr.com Mini review](https://www.rtl-sdr.com/review-airspy-mini/); [rtl-sdr.com launch](https://www.rtl-sdr.com/airspy-mini-99-24-1800-mhz-12-bit-sdr/)
- Coverage is 24–1800 MHz, with a 4.5 V bias tee. No Mini-specific current draw was found. The original Airspy measured 320 mA at full rate with the bias tee off, but that is a different model. — [rtl-sdr.com](https://www.rtl-sdr.com/airspy-mini-99-24-1800-mhz-12-bit-sdr/); [Airspy RadioUser 2015 PDF](https://airspy.com/downloads/Airspy%20RadioUser%20March%202015.pdf)

**RTL-SDR Blog V4**
- It uses an R828D tuner with 3 inputs. A triplexer splits HF (0–28 MHz), VHF (28–250 MHz) and UHF (250 MHz–1.766 GHz). Switchable notch filters cover broadcast AM, FM and DAB with about 5–10 dB of attenuation. A notch is active only when tuned outside its band, so it does not attenuate FM when you are listening to FM. — [rtl-sdr.com V4 launch](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)
- Two-tone tests showed about 8 dB better in-band isolation and 28–43 dB better out-of-band isolation than the V3. — [rtl-sdr.com](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)
- An SA612 upconverter shifts HF up by 28.8 MHz, so it does not use direct sampling. That gives full gain control and removes Nyquist folding. The lower limit stays at 500 kHz, which cuts off the bottom of MW (530 kHz is just inside the range). HF MDS improved, but VHF and UHF MDS got slightly worse, with filter loss of about 2–3 dB. — [rtl-sdr.com](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)
- It has a 1 ppm TCXO, a metal case, a 4.5 V bias tee rated at 180 mA continuous, and draws slightly less current than the V3. — [rtl-sdr.com](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/); [rtl-sdr.com V4 guide](https://www.rtl-sdr.com/v4/)
- Price was $29.95 for the dongle alone at launch. It is about $35 in 2026 and held its price through 2026 sales. The R828D is from a limited stockpile, and one EU store sells it "while stock remains available." — [rtl-sdr.com](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/); [HamDeals 2026](https://hamdeals.com/blog/sdr-dongles-ranked/); [SDRstore.eu summary via search](https://www.sdrstore.eu/best-sdr-for-shortwave-listening-2026/)

**Nooelec NESDR SMArt v5 (current device)**
- It uses an RTL2832U and R820T2 with a ±0.5 ppm TCXO. Quadrature sampling covers 25–1750 MHz. Direct sampling (Q-branch) covers 0.1–25 MHz, although the datasheet's block diagram says 30 MHz. Sample rate is up to 3.2 MSPS, with 2.4 MHz typical. Tuner gain is 0–49.6 dB. Phase noise is −138 dBc/Hz at 1 kHz offset and −150 dBc/Hz at 10 kHz. The retail listing says "7 bit ADC", which reflects the low effective resolution of the 8-bit RTL2832U. — [Nooelec datasheet](https://nooelec.com/datasheets/100700); [manuals.plus manual](https://manuals.plus/nooelec/100701-nesdr-smart-v5-rtl-sdr-software-defined-radio-manual); [Nooelec support FAQ](https://support.nooelec.com/hc/en-us/articles/360005807694-NESDR-SMArt-Series); [Newegg listing](https://www.newegg.com/p/3C6-020A-00EU6)

**HackRF One / HackRF Pro (brief)**
- HackRF One: 1 MHz–6 GHz, 8-bit IQ, up to 20 MSPS, half-duplex transceiver. A HackRF Pro is on preorder, and its specs were not on the HackRF One page. — [Great Scott Gadgets](https://greatscottgadgets.com/hackrf/one/)

**LimeSDR Mini 2.0**
- The Crowd Supply page refused the fetch (403), so no specs were verified. Unverified recollection only: LMS7002M, 12-bit, 10 MHz–3.5 GHz, up to about 30.72 MSPS, full-duplex TRX, FPGA-based. Its lower limit of 10 MHz excludes MW. See Gaps.

### Inferences
- The only device that covers MW 530–1710 kHz natively with preselection is the HF+ Discovery. The Nooelec v5 uses unfiltered direct sampling. The V4 covers MW through its upconverter, but its 500 kHz floor sits right at the band edge. The R2 and Mini need a SpyVerter.
- The R2 and Mini share the R820T2 tuner and its approximately 2 MHz-class IF filtering with the RTL-SDR. Their gain is mainly ADC resolution: 12-bit with 10.4 ENOB against 8-bit with about 7 effective bits, plus better clocking. Real FM-band front-end selectivity is not dramatically different from an RTL-SDR's.
- HackRF (8-bit, wideband, no preselection) and LimeSDR Mini 2.0 (transceiver, no MW, higher power) are poor fits for a receive-only broadcast head unit.

### Gaps
- No published current draw for the HF+ Discovery, Airspy Mini or R2. The rtl-sdr.com power comparison has its figures only in a video. — [rtl-sdr.com](https://www.rtl-sdr.com/measuring-the-usb-power-consumption-of-various-software-defined-radios/)
- No independent ENOB or IP3 measurement for the V4 or Nooelec v5 was found.
- LimeSDR Mini 2.0 and HackRF Pro specs and 2026 prices were not verified.
- No current 2026 prices were verified for the Airspy R2, Mini or Nooelec v5.

## Q2. Airspy HF+ Discovery for FM broadcast and AM: is 660–768 kHz enough? Measured FM/MW performance; 162 MHz / 1090 MHz

### Takeaway
660 kHz of clean bandwidth is plenty for one FM channel with RDS (about ±100 kHz plus the 57 kHz RDS subcarrier inside the MPX) and roughly 6 overlapping FM channels at once. It cannot cover the whole MW band (1.18 MHz wide) in one capture, but it can cover any single AM channel or a segment of about 660 kHz. It covers 162 MHz for AIS and NOAA, but in its reduced-performance extended range, and it does not cover 1090 MHz. Reviews rate it best in class on FM and MW strong-signal handling. They also document a spur and clock-noise issue on FM that may need a hardware mod or careful shielding and powering.

### Cited Findings
- RadioDX.pl (FM-DX review, 28 Jan 2019, original HF+): the HF+ shows up to 660 kHz, which fits about six overlapping FM channels. Signal levels matched the Sony XDR-F1HD to within about 1 dB, with a slightly better noise figure in a limited test. When a station above about 77 dBf sits inside the displayed span, the HF+ attenuates (AGC). Strong stations outside the span are handled well. The review found a strong spur at 98.304 MHz, IMD in the 60–70 MHz range about 75 dB below a nearby high-power site, and recommends avoiding center tuning because of zero-IF DC artifacts. The XDR still wins on adaptive IF bandwidth and stereo noise reduction, which are software features. — [RadioDX.pl](https://radiodx.pl/?p=8726)
- RadioDX.pl (17 Jul 2022): every HF+ Discovery the group owned showed unexpected carriers and spurs on wideband FM. A lab test with an R&S SMC100A at 100 MHz confirmed it, while the older HF+ Dual Port was clean. The suspected cause was USB supply noise reaching the X2 clock generator. A 47 pF capacitor across R34, plus an aluminium case, greatly reduced it, but did not eliminate it. The author calls the HF+ Dual Port "far superior" for serious FM DX, but rates the Discovery well against RTL-SDR v3 and SDRplay. — [RadioDX.pl](https://radiodx.pl/?p=12806)
- rtl-sdr.com review (2017, pre-production HF+): the HF+ had "a very noticeable sensitivity edge on the broadcast FM spectrum" over the Airspy Mini+SpyVerter and RSP2. It handled strong 158 MHz pagers without visible overload, whereas the RSP2 showed severe IMD. On MW, "Overload is non-existent on broadcast AM." The Mini+SpyVerter overloaded from local AM and was noisier on daytime MW. — [rtl-sdr.com](https://www.rtl-sdr.com/our-review-of-the-airspy-hf-compared-against-colibrinano-airspy-mini-rsp2-rtl-sdr/)
- SM5BSZ (Leif) lab tests at 7.2 MHz (blocking and 2nd-order IMD): the Discovery placed 2nd behind the Perseus in most tests, about 3 dB behind it. The rest followed: HF+ Dual at −10 dB, AFEDRI at −15 dB, Airspy+SpyVerter at −18 dB and RSP1 at −23 dB. His unit was pre-production. — [rtl-sdr.com collection](https://www.rtl-sdr.com/airspy-hf-discovery-collection-of-tests-and-reviews/); [rtl-sdr.com search summary](https://www.rtl-sdr.com/?p=34843)
- Leif also found that an FM-blocking filter still helps the HF+ when strong FM is present and you are listening on other bands. — [rtl-sdr.com](https://www.rtl-sdr.com/leif-compares-the-airspy-hf-with-the-airspyspyverter-combo/)
- Guy Atkins (SWLing Post, Aug 2019): MW results from the Discovery and the $899 Elad FDM-DUOr were close to identical. Simon Brown (G4ELI, Jul 2019) called it "virtually immune to strong signals" on Band II. — [rtl-sdr.com collection](https://www.rtl-sdr.com/airspy-hf-discovery-collection-of-tests-and-reviews/)
- DK8OK's magazine review reported −141.5 dBm MDS at 500 Hz bandwidth in the FM band, consistent with the spec. — [WRTH2020 review PDF](https://airspy.com/downloads/WRTH2020Features_Airspy_Discovery_review.pdf)
- A video comparison against the RSP1A on FM showed no detectable difference on strong stations. The differences appeared on weak DX surrounded by strong locals. — [rtl-sdr.com](https://www.rtl-sdr.com/?p=17922)

### Inferences
- For an in-car broadcast receiver, 768 ksps is enough. FM needs one channel plus RDS, and MW needs one channel at a time. The narrow ADC/DDC bandwidth is actually an advantage here, because it keeps strong off-channel signals out of the digitized span.
- Band-scan or seek UIs would need to retune in roughly 600 kHz steps. That is about 34 steps across 88–108 MHz and 2 steps across MW.
- Vehicle 12 V-to-5 V USB supplies are noisy, so the documented clock-noise susceptibility of the Discovery is a real integration risk. Plan for a clean, filtered 5 V supply and a metal enclosure.
- The Discovery could take over NOAA and AIS at 162 MHz, but in its "reduced performance" extended range. ADS-B would still need the RTL-SDR.

### Gaps
- No published head-to-head between the HF+ Discovery and TEF6686-based tuners was found. FM-DX sources compare it with the Sony XDR-F1HD and HF+ Dual instead.
- No measured performance figures at 162 MHz were found.
- No 2024–2026 independent re-tests were found, and it is unknown whether newer production revisions fixed the 2022 spur issue.

## Q3. Airspy R2/Mini vs RTL-SDR on FM; MW requires SpyVerter

### Takeaway
The R2 and Mini give better ADC resolution and dynamic range than an 8-bit RTL-SDR on the same tuner family. Even so, the tests found show them noisier and more overload-prone than the HF+ on FM, and they need a SpyVerter for MW, where they overloaded. For AM/FM quality specifically, they are a weaker choice than the HF+ Discovery.

### Cited Findings
- R2: 12-bit with 10.4 ENOB, 95 dB SFDR, and 3.5 dB NF from 42 to 1002 MHz. It covers 24–1700 MHz and needs the SpyVerter for HF and MW. — [Airspy R2](https://airspy.com/airspy-r2/)
- In the 2017 test, the Mini+SpyVerter was "barely audible" on a 120 km FM station that the HF+ received "fairly clearly". Raising gain caused overload and a higher noise floor. It also overloaded from broadcast AM. — [rtl-sdr.com](https://www.rtl-sdr.com/our-review-of-the-airspy-hf-compared-against-colibrinano-airspy-mini-rsp2-rtl-sdr/)
- Leif's HF test placed Airspy+SpyVerter about 18 dB behind the Perseus, against about 3 dB for the Discovery. — [rtl-sdr.com](https://www.rtl-sdr.com/?p=34843)

### Inferences
- The R2 or Mini makes sense if wideband capture matters, for example 10 MHz for several FM stations or simultaneous decoding. For a single-channel, highest-quality AM/FM receiver it is not the best pick.

### Gaps
- No quantified side-by-side measurement of FM SNR or IMD between the Airspy R2 or Mini and an RTL-SDR V4 or Nooelec v5 was found.

## Q4. Drivers, licensing, ARM64, Yocto/OE recipes, SoapySDR

### Takeaway
Every candidate has open-source userspace drivers built on libusb that run on ARM64. The layer index lists recipes for libairspy and rtl-sdr in meta-sdr. libairspyhf has no recipe there and would need a small custom recipe. The RTL-SDR Blog V4 needs the rtl-sdr-blog driver fork or a recent Osmocom librtlsdr.

### Cited Findings
- meta-sdr (balister/meta-sdr) carries libairspy 1.0.9 and airspy-tools from zeus (3.0) through walnascar (5.2). rtl-sdr is 2.0.2 on master and 0.6.0+git on kirkstone and older. gr-osmosdr has airspy and rtl-sdr PACKAGECONFIGs. — [OE layer index meta-sdr](https://layers.openembedded.org/layerindex/branch/kirkstone/layer/meta-sdr); [libairspy recipe](https://layers.openembedded.org/layerindex/recipe/261041); [rtl-sdr recipe](https://layers.openembedded.org/layerindex/recipe/396607)
- The airspyhf repo contains BSD-3-Clause and GPL-2.0 licence files. The page does not say which applies to the library itself. It has no tagged releases, builds with CMake on Debian/Ubuntu/Raspbian, and has udev options (`-DINSTALL_UDEV_RULES=ON`, `-DUSE_UACCESS_RULES=ON`). — [GitHub airspy/airspyhf](https://github.com/airspy/airspyhf)
- The V4 needs RTL-SDR Blog drivers, which are backward compatible with V3 and generic dongles. Without them, signals are missing or at the wrong frequency. The driver detects a V4 by its EEPROM strings "RTLSDRBlog" and "Blog V4". A later update says Osmocom's default branch now supports the V4. — [rtl-sdr.com V4 guide](https://www.rtl-sdr.com/v4/); [rtl-sdr.com launch](https://www.rtl-sdr.com/rtl-sdr-blog-v4-dongle-initial-release/)

### Inferences
- MIT compatibility: the project would dynamically link these libraries, not copy their code. librtlsdr is GPL-2.0 (known licence, not re-verified here). If the project's own binary links it, the combined distribution must satisfy GPL-2.0, which is the case today. Separating the radio daemon as a process boundary or using SoapySDR modules reduces entanglement. Confirm the libairspyhf licence from file headers before shipping.
- The rtl-sdr 2.0.2 recipe on meta-sdr master probably post-dates Osmocom's V4 support, which makes it worth checking whether the AGL base (scarthgap/walnascar) picks it up.

### Gaps
- The libairspy licence (believed BSD-3) and the libairspyhf per-file licence were not verified.
- No OE recipe was found for libairspyhf or for SoapyAirspyHF. These probably need custom recipes, but that is unconfirmed.
- The meta-sdr branch matching AGL's current Yocto base was not verified against GitHub.

## Q5. Two-device architecture in a vehicle (USB power, antenna splitting, cost) and newer 2025–2026 hardware

### Takeaway
A two-device setup is reasonable: an HF+ Discovery for AM/FM (about $170–200) plus the existing RTL-SDR for ADS-B, AIS and NOAA. It stays within the Pi 5's 1.6 A USB budget, but only if the 5 A supply is negotiated or configured. Otherwise the Pi limits USB to 600 mA. No significant new hobby SDR hardware from Airspy or RTL-SDR Blog in 2025–2026 was found.

### Cited Findings
- Pi 5 USB: total USB current is 600 mA unless a 5 V/5 A supply is detected over USB-PD, `PSU_MAX_CURRENT=5000` is set in EEPROM, or `usb_max_current_enable=1` is set. Then it is 1.6 A. Check with `vcgencmd get_config usb_max_current_enable`. — [Raspberry Pi white paper RP-009856](https://pip-assets.raspberrypi.com/categories/685-app-notes-guides-whitepapers/documents/RP-009856-WP-1-USB%20Power%20delivery%20on%20Raspberry%20Pi%205.pdf)
- Reference draws: original HF+ maximum 140 mA. Original Airspy R-series 320 mA at full rate. The V4 bias tee can source up to 180 mA on top of the dongle's own draw. — [RigPix](https://rigpix.com/miscrx/airspy_hfplus.htm); [Airspy RadioUser 2015](https://airspy.com/downloads/Airspy%20RadioUser%20March%202015.pdf); [rtl-sdr.com V4](https://www.rtl-sdr.com/v4/)
- 2026 market: no new RTL-SDR Blog or Airspy receiver was announced. Roundups list the V4 at about $35 and the HF+ Discovery at about $169–199. The V4 is sold while R828D stock lasts. — [HamDeals 2026](https://hamdeals.com/blog/sdr-dongles-ranked/); [SDRstore.eu 2026](https://www.sdrstore.eu/best-sdr-for-shortwave-listening-2026/)
- A HackRF Pro is on preorder, but it is a transceiver and not broadcast-focused. — [Great Scott Gadgets](https://greatscottgadgets.com/hackrf/one/)

### Inferences
- Estimated budget: HF+ Discovery (about 150 mA, assumed similar to the HF+) plus an RTL-SDR (typically about 250–300 mA, not measured here) comes to roughly 0.5 A. That fits within 1.6 A, and may even fit within 600 mA without bias-tee loads. This needs a bench measurement.
- Antennas: the HF+ Discovery covers MW and FM on one SMA input, so one AM/FM car whip can feed it directly. A separate 1090 MHz ADS-B antenna and a 162 MHz marine/NOAA antenna, or a diplexer, can feed the RTL-SDR. Splitting one whip across both radios with a resistive splitter costs about 3.5 dB on FM. A diplexer (MW/FM versus above 150 MHz) is lossless in principle but needs a custom filter.
- Running both radios at once also removes the current "one mode at a time" limit, so AM/FM can play while ADS-B or AIS runs in the background.
- Car electrical noise and the Discovery's documented clock-noise sensitivity argue for a filtered 5 V rail and shielded USB cables.

### Gaps
- Real current draw of the HF+ Discovery and Nooelec v5 on a Pi 5 or Radxa Q6A has not been measured.
- No published reports of either device on the Radxa Dragon Q6A were found.
- No 2025–2026 SDR designed specifically for broadcast reception (for example, a TEF6686-plus-SDR hybrid) turned up in the searches. The TEF6686 comparison is covered by a separate thread.
