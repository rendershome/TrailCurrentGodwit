# Dedicated Automotive Broadcast Tuner Chips/Modules vs. SDR for AM/FM in TrailCurrent Godwit

Research date: 2026-11-21. Scope: whether a hardware broadcast tuner IC/module (NXP TEF668x, Skyworks Si473x / Si468x / Si479x, NXP SAF775x/SAF360x) would beat the current RTL-SDR for AM/FM, and what integration on AGL/ARM64 (Pi 5 / CM5, Radxa Dragon Q6A) would take.

Method note: datasheet figures for Si4734/35, Si4689 and the TEF668x brochure were pulled from the vendor PDFs with pdftotext. Several fetches failed: WTFDA forum (TLS certificate error), the Skyworks Si4735 product page (404), and the fmdx.org forums. So the community measured-comparison material is thinner than hoped. The Gaps sections say where.

---

## 1. Candidate chips and modules: bands, performance, features, interfaces, openness, availability

### Takeaway
The NXP TEF6686 (and its 6687/6688/6689 siblings) is the strongest hobbyist-accessible broadcast tuner. It is a real automotive DSP radio, AEC-Q100 qualified, with multipath suppression, channel equalization, noise blanking, soft mute, stereo high blend, RDS/RBDS, I2S out and I2C control. Shielded modules sell for about US$10–50. The Si4735/Si4732 is a cheaper, lower-performance consumer part. The Si4689 is the only practical single chip with on-chip NRSC-5 IBOC, but NRSC-5 IBOC carries Xperi licensing and certification. Si479x and SAF775x/SAF360x are tier-1 automotive parts aimed at OEM design-ins rather than hobbyists, and SAF-family enablement looks NDA-gated.

### Cited Findings

**NXP TEF668x (TEF6686 / TEF6687 / TEF6688 / TEF6689, "Lithio" family)**
- Bands: FM 65–108 MHz (OIRT, Japan, Europe, US). AM covers LW, MW and full SW — [NXP TEF668X product page](https://www.nxp.com/products/TEF668X)
- Architecture: "Alignment-free digital receiver with tuner and software-defined radio processing". Digital IF processing covers decimation, shift to baseband, AGC, I/Q correction, variable IF bandwidth filtering (PACS) and demodulation — [NXP TEF668x brochure (75017468.pdf)](https://cache.nxp.com/docs/en/brochure/75017468.pdf)
- Built-in reception aids: FM Improved Multipath Suppression (iMS), FM channel equalization, FM LNA with AGC, AM and FM noise blanking, signal quality detection, weak-signal processing, Soft Mute on Modulation, Stereo High Blend and FM stereo decoding — [NXP brochure](https://cache.nxp.com/docs/en/brochure/75017468.pdf); [NXP product page](https://www.nxp.com/products/TEF668X)
- RDS: "Advanced RDS and RBDS demodulation and decoding" on chip. An MPX output feeds a DARC demodulator — [NXP brochure](https://cache.nxp.com/docs/en/brochure/75017468.pdf)
- Audio: one I2S output and one I2S input, plus two mono audio DACs (analog L/R) — [NXP product page](https://www.nxp.com/products/TEF668X)
- Control: fast-mode I2C at 400 kHz through a command-based high-level interface. GPIOs are configurable for RDS interrupt and quality-status interrupt. Single 3.3 V supply, HVQFN32 5x5 mm package, AEC-Q100 qualified. Status is "Active" — [NXP product page](https://www.nxp.com/products/TEF668X)
- Variants: the TEF6688 adds a baseband I2S output that supports NRSC-5 IBOC and DRM when paired with an NXP coprocessor (SAF356x or SAF360x), plus a blending function for NRSC-5 IBOC. The TEF6686 has no NRSC-5 IBOC path — [NXP brochure](https://cache.nxp.com/docs/en/brochure/75017468.pdf)
- Public documentation: NXP's page lists only a 2013 brochure and package/packing sheets publicly. Full datasheet and user manual are not openly listed (a "secure files" area appears) — [NXP product page](https://www.nxp.com/products/TEF668X)
- Firmware/patch: the host MCU stores "all the firmware that operates the radio, which is sent to the TEF6686 after every boot up" — [IEEE Spectrum](https://spectrum.ieee.org/hacking-a-car-radio-chip). A PE5PVB discussion refers to a factory-installed "Lithio Patch V102" on Chinese-built radios — [TEF6686_ESP32 discussion #885](https://github.com/PE5PVB/TEF6686_ESP32/discussions/885)
- History: NXP introduced the chip in 2013. It is used mainly in car radios and is valued for low cost, audio fidelity and very high sensitivity and selectivity — [IEEE Spectrum](https://spectrum.ieee.org/hacking-a-car-radio-chip)
- Module and receiver prices (reader- and article-reported): AliExpress TEF6686 modules "can generally be found for around US $25". Reader reports range from US$9.81 (sale) to about US$50 for a TDQ-231V-286 module — [IEEE Spectrum](https://spectrum.ieee.org/hacking-a-car-radio-chip). A Chinese handheld TEF6686 receiver was listed at US$61.26 on sale — [RTL-SDR.com TEF6686 tag](https://www.rtl-sdr.com/tag/tef6686/)
- Open hardware: PE5PVB published ESP32 control-board and mainboard designs (touchscreen, Wi-Fi, Bluetooth audio) on PCBWay — [PCBWay control board](https://www.pcbway.com/project/shareproject/TEF6686_ESP32_controlboard.html); [PCBWay mainboard](https://www.pcbway.com/project/shareproject/Mainboard_TEF6686_FM_receiver_with_Touchscreen__WiFi_and_Bluetooth_Audio.html)

**Skyworks (ex-Silicon Labs) Si4734 / Si4735 (and Si4732, which uses the same die family)**
- Bands: FM 64–108 MHz, AM 520–1710 kHz, SW 2.3–21.85 MHz, LW 153–279 kHz. The RDS/RBDS processor is on the Si4735 only — [Si4734/35 data short](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf)
- FM performance (typical): sensitivity 2.2 µV EMF with headphone matching or 1.1 µV EMF with 50 Ω matching, at (S+N)/N = 26 dB. Input IP3 105 dBµV EMF. Adjacent channel selectivity (±200 kHz) 50 dB. Alternate channel selectivity (±400 kHz) 70 dB. Mono S/N 63 dB. Audio response 30 Hz to 15 kHz — [Si4734/35 data short](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf)
- AM performance: sensitivity 25 µV EMF at 26 dB (S+N)/N, with a further 56 dB figure on the same table (likely AM S/N) — [Si4734/35 data short](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf); [search summary of the same table](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf)
- Control: I2C-compatible 2-wire, SPI, or a legacy 3-wire interface. Analog audio out, plus a digital audio output (DOUT) shown on the block diagram — [Si4734/35 data short](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf)
- SSB: SSB on the Si4735-D60 and Si4732-A10 needs an "SSB extension PATCH" uploaded over I2C at each power-up. The patch came from Vadim Afonkin's Dropbox, is not shipped with the PU2CLR library, and the library author "does not encourage anyone to use the SSB patches content for commercial purposes" — [PU2CLR SI4735 library](https://github.com/pu2clr/SI4735/wiki); [PU2CLR search summary](https://pu2clr.github.io/SI4735/extras/apidoc/html/group__group17_html)

**Skyworks Si4689 (Si468x family: AM/FM/NRSC-5 IBOC/DAB/DAB+)**
- Bands: FM 76–108 MHz, AM 520–1710 kHz, DAB/DAB+ Band III 168–240 MHz. Advanced RDS/RBDS decoder — [Si4689-A10 data short](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-shorts/Si4689-A10-short.pdf)
- NRSC-5 IBOC: compatible with iBiquity Digital and NRSC-5 IBOC. Provides IBOC audio decoding, on-chip IBOC analog/digital blend, multicasting, and HD data services (PSD, Artist Experience, tagging and so on). The data short's feature list says FM NRSC-5 IBOC support with on-chip blend (paraphrased), and its spec table has an "AM IBOC" row (the extracted text was garbled) — [Si4689-A10 data short](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-shorts/Si4689-A10-short.pdf)
- Audio and control: I2S digital audio out with ASRC, an integrated 97 dB stereo audio DAC, concurrent I2S and L/R analog out, and SPI or I2C host control. Pins for external flash and NVM are present, so the firmware image is loaded from the host or from flash — [Si4689-A10 data short](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-shorts/Si4689-A10-short.pdf)
- Certification: Silicon Labs said the Si468x parts were pre-certified by iBiquity for AM/FM NRSC-5 IBOC reception, which shortens product certification — [Silicon Labs 2014 press release](https://news.silabs.com/2014-09-15-Silicon-Labs-Expands-Digital-Radio-Receiver-Family-to-Support-Worldwide-Radio-Reception?asPDF=1)
- A third-party "SI4684 Prog" programming document has circulated publicly — [mikrocontroller.net attachment](https://www.mikrocontroller.net/attachment/412698/SI4684_Prog.pdf)

**Skyworks Si479x (Si4795x / Si4796x / Si4797x automotive receivers)**
- Described as "high-performance automotive AM/FM radio receivers and digital radio tuners with the smallest external BOM" — [Skyworks Si4795x page](https://www.skyworksinc.com/en/Products/Audio-and-Radio/Si4795x-Automotive-Receivers)
- Si47961/62: dual AM/FM receivers with NRSC-5 IBOC, DAB/DAB+, DMB and DRM tuner support, plus analog FM phase diversity and NRSC-5 IBOC MRC with two tuners — [Si47961/62 data short](https://www.skyworksinc.com:443/-/media/SkyWorks/SL/documents/public/data-shorts/Si47961-62-short.pdf)
- Si47971/72: dual receivers with an audio DSP subsystem, FM phase diversity, NRSC-5 IBOC MRC and DAB MRC (DAB on the Si47972 only) — [Si47971/72 data short](https://www.skyworksinc.com/-/media/SkyWorks/SL/documents/public/data-shorts/Si47971-72-short.pdf)

**NXP SAF775x / SAF360x**
- SAF775x: "High-Performance Dual Tuner with Integrated Audio Processing and HiFi Core". SAF360x: a digital radio SoC for NRSC-5 IBOC, DAB/DAB+ and DRM with in-field software upgrade — [NXP SAF360x page](https://nxp.com/products/audio-and-radio/hybrid-radio-and-audio/digital-radio-and-processing-system-on-chip:SAF360X); [NXP SAF7771EL](https://www.nxp.com/part/SAF7771EL)
- A sibling NXP automotive part (SAF9000) states that hardware and enablement are "available for selected customers with an approved NDA". A community request for a SAF360x dev kit appears unanswered — [NXP SAF9000 block diagram](https://www.nxp.com/assets/block-diagram/en/SAF9000.pdf); [NXP community thread](https://community.nxp.com/t5/MBDT-for-RADAR/SDR-radio-dev-kit-SAF360X/m-p/737686)

### Inferences
- For a "factory car radio or better" goal, the TEF668x is effectively a factory car-radio tuner: it is the same class of AEC-Q100 part with the same automotive DSP aids (iMS, EQ, noise blanker, soft mute, high blend). The Si4735 is a consumer portable-radio part. Its 50 dB adjacent and 70 dB alternate selectivity is respectable but below a premium automotive tuner.
- The TEF6687 and TEF6689 are believed to be the higher-performance siblings (the 6687 is an improved 6686, and the 6689 is the HD/DRM-capable counterpart of the 6688). I could not confirm this from a primary source here; see Gaps.
- Si479x and SAF775x would give the best results (dual-tuner phase diversity), but they look OEM-only. They are not practical for an open-source hobbyist build.
- Every candidate needs a binary blob or patch loaded at boot: TEF patch/init tables, Si468x firmware image, and the Si4735 SSB patch. None is a fully open-documentation chip, so redistributing those blobs in an MIT repo is a licensing question in its own right.

### Gaps
- No primary-source TEF6686/6687 sensitivity, selectivity or IP3 numbers. NXP's datasheet is not public on the product page, and the brochure has none.
- Exact differences between TEF6686/6687/6688/6689 and their silicon revisions (V102, V205): not confirmed from NXP.
- Si4689 sensitivity and selectivity numbers did not extract cleanly. Whether AM IBOC is supported, or only FM IBOC, is ambiguous in the data short.
- Current distributor stock and pricing (Mouser/Digi-Key) for TEF6686HN, Si4735-D60 and Si4689 in late 2026 was not retrieved.
- SAF775x/SAF360x NDA status was not confirmed directly; it is inferred from a sibling part.

---

## 2. Measured and community-reported performance: TEF6686 vs SDRs; AM performance

### Takeaway
I found no rigorous, published, side-by-side lab measurement of TEF6686 against SDRs. The community view is that TEF-based receivers are among the best FM-DX receivers ever made: IEEE Spectrum calls the result "perhaps the highest-performing radio of its kind, ever". Experienced DXers note that general-purpose SDRs have a narrow instantaneous dynamic range, which hurts weak-next-to-strong reception. The cheap RTL-SDR (8-bit ADC) is the weakest of the SDRs in this respect.

### Cited Findings
- IEEE Spectrum credits the TEF6686's "extreme selectivity and sensitivity" to its use of SDR and DSP techniques to filter adjacent frequencies, and calls the PE5PVB-team receiver "perhaps the highest-performing radio of its kind, ever". The article includes no independent lab test — [IEEE Spectrum](https://spectrum.ieee.org/hacking-a-car-radio-chip)
- A WTFDA thread comparing TEF6686, TEF6687 and SDRs (RSPduo, Airspy HF+) reports that the SDRs look strong within their narrow dynamic range. Manufacturers make that range "seem large by moving that narrow window around a much larger claimed bandwidth", so SDRs suffer on a weak station close to a strong one. The adjacent-channel figures were "still to come" — [WTFDA forum thread 1815](https://forums.wtfda.org/forum/dx-equipment/1815-tef6687-vs-tef6686-vs-others) (via search snippet; the page failed to fetch because of a TLS error)
- A WTFDA commenter cautions that the TEF "is not an SDR receiver" and is limited by "the miniscule processing power inside the tef". Another says intermod and overload, not selectivity, are the bigger problems with "one chip wonders" — [WTFDA thread 930](https://forums.wtfda.org/forum/dx-equipment/930-tef-6866-can-t-wait-to-try-this?p=983) (search snippet)
- An Elektroda user reports finding no strict parameter comparison or subjective by-ear assessment of TEF6686 against alternatives — [Elektroda](https://www.elektroda.com/rtvforum/topic3838201.html) (search snippet)
- The worldwide FM-DX Webserver network of online receivers is "mostly TEF6686-based, with a few RTL-SDR units". The TEF6686 is described as "highly sensitive" — [RTL-SDR.com TEF6686 tag](https://www.rtl-sdr.com/tag/tef6686/)
- SDR strong-signal context: RTL-SDR.com found the Airspy generally better than the SDRplay RSP in two-tone IMD tests, and attributed some degradation to reciprocal mixing from LO phase noise — [RTL-SDR.com Airspy vs RSP vs HackRF](https://www.rtl-sdr.com/review-airspy-vs-sdrplay-rsp-vs-hackrf/3/). The Airspy HF+ was designed to resist overload from strong broadcast AM/FM signals — [RTL-SDR.com HF+ review](https://www.rtl-sdr.com/our-review-of-the-airspy-hf-compared-against-colibrinano-airspy-mini-rsp2-rtl-sdr/); [WRTH 2019 HF+ review](https://airspy.com/downloads/WRTH2019Features_AirspyHF+_review.pdf)
- An RTL-SDR.com comparison covers SDR degradation from nearby strong signals at broadcast FM frequencies — [RTL-SDR.com comparison](https://www.rtl-sdr.com/?p=5023)
- Si4735 datasheet strong-signal figures (FM): IIP3 105 dBµV EMF, ACS 50 dB, alternate 70 dB — [Si4734/35 data short](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf)

### Inferences
- For a mobile head unit, the hard problems are strong-signal overload near transmitters, multipath and picket-fencing while driving, and impulse noise from the vehicle's ignition and electronics. Those are exactly what the TEF's automotive DSP chain handles in hardware: AGC'd LNA, iMS, channel EQ, noise blanker, soft mute, high-cut/high-blend. An RTL-SDR plus a software demodulator (8-bit ADC, no automotive weak-signal logic unless someone writes it) is very likely to sound worse in a moving vehicle. This is reasoning, not a measured result.
- AM on an RTL-SDR in direct-sampling mode has no front-end filtering or AGC suited to MW. Every hobbyist broadcast tuner has a dedicated AM front end with a noise blanker. The TEF has an AM noise blanker; the Si4735 has 25 µV EMF AM sensitivity.

### Gaps
- No published lab numbers (SINAD, ACS, IP3, blocking) for TEF6686 vs RTL-SDR, Airspy or SDRplay under identical conditions.
- No measured AM (MW) performance comparison between TEF6686, Si4735 and an RTL-SDR in direct sampling.
- The fmdx.org forum and the WTFDA thread contents could not be fetched directly.

---

## 3. Integration into Linux / AGL (drivers, libraries, I2S into ALSA/PipeWire, licensing)

### Takeaway
No mainline Linux driver exists for the TEF6686, Si4735 or Si468x. Mainline `drivers/media/radio` has only older parts (tef6862, si470x, si4713, si476x, saa7706h, tea575x). The practical routes are (a) a userspace I2C daemon driving the tuner directly, with I2S audio into the SoC as an I2S-slave ALSA capture device, or (b) keeping an ESP32 or other MCU in the loop running PE5PVB-style firmware, exposing the XDR-GTK serial/TCP protocol, with audio via analog or I2S. Existing TEF code is GPL-3.0, so it cannot be merged into the MIT codebase but can run as a separate program or firmware.

### Cited Findings
- Mainline `drivers/media/radio` contains `si470x/`, `si4713/`, `radio-si476x.c`, `tef6862.c`, `saa7706h.c`, `tea575x.c`, `radio-tea5764.c` and `radio-tea5777.c`, plus legacy ISA/USB radios. There is no TEF668x, Si473x or Si468x driver — [Linux kernel drivers/media/radio](https://github.com/torvalds/linux/tree/master/drivers/media/radio)
- The kernel documents a Si4713 FM transmitter driver, not a Si4735 receiver driver — [kernel.org Si4713 doc](https://docs.kernel.org/5.15/admin-guide/media/si4713.html)
- Si4735 software: the PU2CLR Arduino library (C++, tested on RP2040) handles I2C control and SSB patch upload. A Raspberry Pi Python Si473x library exists only as an old work-in-progress gist — [PU2CLR SI4735](https://github.com/pu2clr/SI4735/wiki); [Jonathan Thorpe gist](https://gist.github.com/JonathanThorpe/f23480c781ca62a28647); [csdexter/Si4735](https://github.com/csdexter/Si4735)
- TEF software: PE5PVB/TEF6686_ESP32 is GPL-3.0 ("version 3 or later") ESP32 firmware for NXP TEF668x tuners — [PE5PVB/TEF6686_ESP32](https://github.com/PE5PVB/TEF6686_ESP32). It connects to RDS Spy, XDR-GTK, fm-dx-webserver and StationList over Wi-Fi — [TEF6686_ESP32 wiki](https://github.com/PE5PVB/TEF6686_ESP32/wiki). The v2.10 release updated the XDR-GTK protocol for fm-dx-webserver compatibility — [v2.10 release notes](https://newreleases.io/project/github/PE5PVB/TEF6686_ESP32/release/v2.10). Current firmware is v2.11 — [fmdx.org project page](https://fmdx.org/projects/tef6686-esp32.php)
- kkonradpl/fm-dx-tuner is alternative tuner firmware supporting the ESP32 (PE5PVB board), compatible with XDR-GTK and FM-DX Webserver — [fm-dx-tuner (ecosyste.ms)](https://awesome.ecosyste.ms/projects/github.com%2Fkkonradpl%2Ffm-dx-tuner). Forks include NoobishSVK/TEF6686-remastered and HyperDX/TEF_ESP — [TEF6686-remastered](https://github.com/NoobishSVK/TEF6686-remastered/releases/tag/v1.21); [HyperDX/TEF_ESP](https://github.com/HyperDX/TEF_ESP/releases)
- Pi 5 I2S: the I2S peripherals sit on the RP1 I/O controller, and only two of RP1's three I2S blocks reach usable pins on Pi 5 and CM5 — [Raspberry Pi I2S white paper](https://pip-assets.raspberrypi.com/categories/1259-audio-camera-and-display/documents/RP-009699-WP-1-Using%20the%20I2S%20peripherals%20on%20Raspberry%20Pi%20SBCs.pdf)
- A common pattern for an external I2S source: a device-tree overlay that puts the SoC I2S in slave mode (simple-audio-card with bitclock/frame master on the codec side), with the source chip controlled over I2C outside ALSA — [Radxa forum, I2S in/out](https://forum.radxa.com/t/basic-i2s-in-and-out-setup-on-cm3-and-or-io-board/11870); [Digi-Key forum, Pi I2S slave](https://forum.digikey.com/t/raspberry-pi-i2s-driver-slave/13320)
- Pi 5 I2S pitfalls reported: overlays failing to load, only one channel captured from TDM in slave mode, and ALSA hangs on full-duplex — [TI E2E TLV320AIC3110 on Pi 5](https://e2e.ti.com/support/audio-group/audio/f/audio-forum/1456663/tlv320aic3110-for-the-life-of-me-i-can-t-make-audio-chip-dtbo-loaded-or-alsa-working-in-raspberry-pi-5); [TI E2E TLV320ADC5140 on Pi 5](https://e2e.ti.com/support/audio-group/audio/f/audio-forum/1509257/tlv320adc5140-tvl320adc5140-master-raspberry-pi-model-5-tdm-4-channels-16bit-48kbit-12-288-mhz); [raspberrypi/linux #5747](https://github.com/raspberrypi/linux/issues/5747)

### Inferences
- Lowest-risk integration path: a TEF6686 module plus a small MCU (ESP32 or RP2040) running fm-dx-tuner or PE5PVB firmware, connected to the head unit over USB-serial using the XDR protocol. Audio would come from the module's analog DAC into a USB audio interface or codec, or from its I2S output into the Pi's I2S in slave mode. This keeps the GPL firmware a separate work from the MIT AGL application, and only the protocol client needs to live in Godwit. Licensing here is an inference; get legal review.
- A cleaner "native" path is a userspace C/Rust daemon on the SoC that speaks I2C to the TEF directly, uploads the patch/init tables at boot, and exposes tuning and RDS to the AGL radio service. It must be written clean-room or GPL-licensed, and it must deal with the provenance of the TEF patch blob, which is not publicly released by NXP.
- Making the tuner the I2S bit-clock master and the Pi the slave avoids ASRC and clock-domain issues. The Si4689 has an ASRC on its I2S out, which helps.
- Radxa Dragon Q6A (Qualcomm): I2S-slave capture support on its BSP was not researched. Expect more device-tree and driver work than on the Pi. USB audio is the portable fallback.

### Gaps
- No existing AGL radio-binding backend for TEF or Si473x was found. AGL's radio service has historically targeted RTL-SDR and kernel V4L2 radio; not verified this session.
- The legal status of redistributing the TEF patch/init blob, and its origin, is undocumented in the sources found.
- I2S slave capture on the Dragon Q6A was not researched.

---

## 4. NRSC-5 IBOC: legal and technical requirements; relevance in North America

### Takeaway
There are two technical paths. (1) A licensed chip with on-board IBOC decode: Si4689, Si479x, or TEF6688 plus an NXP SAF356x/SAF360x coprocessor. These need Xperi/iBiquity licensing and certification to ship as a product using the brand. (2) The open-source nrsc5 decoder on SDR I/Q samples. It works today with RTL-SDR, and a 2026 community plugin already pairs a TEF6686 (analog and scan) with an RTL-SDR V3 running nrsc5 (digital). nrsc5 is fine for hobby use, but Xperi licensing and trademark rules still govern any commercial claim using the brand name.

### Cited Findings
- nrsc5 receives NRSC-5 stations using an RTL-SDR (local or via rtl_tcp) or from I/Q files (cu8, cs16, cf32). It supports FM by default and AM with `--am`, programs 0–3, C and Python APIs, and AAS/the NRSC-5 audio codec dumps, and it builds on Linux — [theori-io/nrsc5](https://github.com/theori-io/nrsc5)
- Hybrid precedent (2026): Ivan (NO2CW) added NRSC-5 IBOC to FM-DX Webserver by pairing a TEF6686 with an RTL-SDR V3. The TEF scans for analog signals; when HD is detected, the RTL takes over and nrsc5 decodes the main and second program services. The plugin is by GitHub user seehed (NRSC5_HDRadio). The V3 worked better than the V4 with the required DLLs. His Miami receiver is the only HD-capable one on the network — [RTL-SDR.com TEF6686 tag](https://www.rtl-sdr.com/tag/tef6686/)
- The TEF6688 supports NRSC-5 IBOC and DRM only with an NXP SAF356x or SAF360x coprocessor via its baseband I2S output — [NXP brochure](https://cache.nxp.com/docs/en/brochure/75017468.pdf)
- Si4689: on-chip IBOC decoding and blend, HD data services. Datasheet disclaimer: "No license, whether express, implied, by estoppel or otherwise, is granted to any intellectual property rights by this document" — [Si4689-A10 data short](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-shorts/Si4689-A10-short.pdf)
- Xperi's 2023 NRSC IP letter confirms patent licensing under NRSC policy and keeps the 2005 and 2011 iBiquity letters in force — [Xperi 2023 NRSC-5-E IP letter](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-5-e/Xperi2023.pdf); [Xperi NRSC-5 patent list](https://xperi.com/hd-radio-patents)
- iBiquity's 2005 IP disclosure says the NRSC-5 audio codec object code is licensed separately from the rest of the NRSC-5 IBOC implementation, with conditions. This is an old document, and current terms may differ — [iBiquity IP disclosure 2005](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-5-b/ibiquity-ip-disclosure-041305.pdf)
- Xperi's OEM trademark guidelines require licensees to submit materials for review before production, as part of iBiquity certification — [Xperi trademark usage guidelines, OEM v4.0.0.5 (2025)](https://hdradio.com/wp-content/uploads/2025/08/REF_PR_2731-HD-Radio-Trademark-Usage-Guidelines-OEM-Version-4.0.0.5.pdf)
- Silicon Labs said the Si468x was pre-certified by iBiquity for AM/FM NRSC-5 IBOC reception — [Silicon Labs 2014 press release](https://news.silabs.com/2014-09-15-Silicon-Labs-Expands-Digital-Radio-Receiver-Family-to-Support-Worldwide-Radio-Reception?asPDF=1)

### Inferences
- In North America, NRSC-5 IBOC matters because many FM stations carry supplemental program service (SPS) multicasts with no analog equivalent, and digital FM/AM audio is cleaner in good coverage. A broadcast engineer will likely expect it.
- For an MIT open-source project, the realistic HD path is nrsc5 on the SDR, not a licensed chip. That argues for keeping an SDR available for broadcast HD even when a hardware tuner handles analog AM/FM. The NO2CW TEF6686 + RTL-SDR + nrsc5 setup is a working blueprint for "TEF for analog and blend-free scan, SDR for HD".
- A seamless analog-to-digital blend like a factory IBOC receiver (time-aligned) is not provided by nrsc5 plus a separate TEF. Time alignment would need custom work.
- Avoid using the system's trademarked brand name in Godwit's UI or marketing without a license. Describe it as "NRSC-5 digital decoding (nrsc5)".

### Gaps
- Current (2026) Xperi licensing terms and costs for receiver makers, and whether hobby/open-source use of nrsc5 has drawn any Xperi response, were not found.
- Si4689 AM IBOC support is not confirmed.

---

## 5. Architecture options: hardware tuner plus RTL-SDR vs SDR-only; antenna sharing

### Takeaway
A dedicated broadcast tuner (TEF6686/6687 module) for analog AM/FM/RBDS, plus the existing RTL-SDR freed up for ADS-B, AIS, NOAA WX and optional nrsc5 HD decoding, solves both problems: broadcast audio quality and the single-dongle sharing conflict. The TEF module is cheap (about US$10–50), handles automotive impairments in silicon, and outputs I2S or analog audio with RDS over I2C. SDR-only keeps the BOM minimal but cannot match a car-radio DSP chain without substantial DSP work, and it cannot run broadcast and ADS-B/AIS at the same time on one RTL-SDR.

### Cited Findings
- The FM-DX Webserver network is mostly TEF6686-based with a few RTL-SDRs, and a working TEF + RTL-SDR hybrid exists for NRSC-5 IBOC — [RTL-SDR.com TEF6686 tag](https://www.rtl-sdr.com/tag/tef6686/)
- TEF668x frequency coverage: FM 65–108 MHz and AM 144 kHz–27 MHz (PE5PVB firmware) — [fmdx.org TEF6686_ESP32 page](https://fmdx.org/projects/tef6686-esp32.php); [RTL-SDR.com](https://www.rtl-sdr.com/tag/tef6686/)
- Premium vehicles use multiple tuner ICs and antennas for FM phase diversity and digital-radio MRC — [EE Journal industry news](https://www.eejournal.com/industry_news/20150326-03); [Si47961/62 data short](https://www.skyworksinc.com:443/-/media/SkyWorks/SL/documents/public/data-shorts/Si47961-62-short.pdf)
- Si4734/35 supports a wide range of ferrite loop sticks and air loop antennas for AM — [Si4734/35 data short](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf)

### Inferences
- Recommended architecture: TEF6686 or TEF6687 module → I2C control (direct from the SoC or via an MCU) and I2S or analog audio into the head unit's audio graph (PipeWire). The RTL-SDR (or two) stays dedicated to 1090 MHz ADS-B, 162 MHz AIS, 162.4–162.55 MHz NOAA, and optionally nrsc5 for HD on demand.
- Antennas: the car AM/FM mast (or an active amplified glass antenna) feeds the TEF directly. ADS-B and AIS want their own band-specific antennas (1090 MHz, and a marine VHF whip on an RV). Splitting one broadband antenna between the TEF and an SDR costs at least 3 dB plus splitter loss and can couple SDR LO leakage into the tuner. If HD via nrsc5 is wanted, a passive 88–108 MHz splitter, or an FM-band LNA feeding a splitter, between the TEF and the SDR is reasonable. These are engineering inferences, not measured.
- Phase diversity needs two tuners and two antennas. Neither the TEF6686/6687 nor the Si4735 does it alone. Only Si479x/SAF775x-class parts do it, and those are not hobbyist-accessible.
- The Si4735/Si4732 is a fallback if extreme cost or size matters, or if SSB/SW is wanted. For "factory car radio or better", the TEF is the better choice.
- Europe: DAB/DAB+ would need a Si468x-class chip (Si4684/Si4689) or an SDR with welle.io-type software. The TEF6686 has no DAB.

### Gaps
- No measured data on splitter or LNA effects on TEF sensitivity in a vehicle.
- No confirmed off-the-shelf "TEF6686 USB tuner + I2S/USB audio" module designed for SBC head units; most products are standalone handhelds or ESP32 boards.
