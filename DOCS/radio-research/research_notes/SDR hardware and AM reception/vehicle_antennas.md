# Vehicle Antennas for an SDR Head Unit (AM / FM-RDS / NOAA / AIS / ADS-B, plus GNSS/LTE/Wi-Fi)

Research scope: antennas, feedlines, connectors and front-end practice for TrailCurrent Godwit (ARM64/AGL head unit, RTL2832U-class SDR today) in cars, RVs, campervans and overland rigs. North America focus, as of Nov 2026. Research budget was limited (~17 tool calls); several product datasheets (Winegard RV FM antennas, Taoglas MA1045 page, Morad) could not be retrieved and are listed as gaps. Numbers labeled "calc" are my own arithmetic from cited inputs, not sourced figures.

## 1. How factory car-radio AM/FM antennas work, and what that means for a 50-ohm SDR input

### Takeaway
A factory/aftermarket car antenna is two different antennas in one: on AM it is a small capacitive voltage source (whip ~10-25 pF) feeding a deliberately high-impedance, low-capacitance input (~80 pF total system capacitance including cable); on FM it is a ~75-150 ohm source. A 50-ohm SDR input is fine on FM (a few tenths of a dB mismatch) but is a severe mismatch on AM (roughly 30-50 dB worse than a car radio's high-Z input), so AM needs an active/buffered antenna or a high-Z buffer at the receiver, not an adapter.

### Cited Findings
- On AM the vehicle antenna element "becomes a high-impedance (capacitive) antenna"; the received voltage is divided between the antenna-to-ground capacitance and the cable capacitance — [US Patent 5,359,335 (automobile antenna)](https://patentimages.storage.googleapis.com/14/54/30/f2aef5bcd9a18c/US5359335.pdf)
- Car AM tuning systems are designed around ~80 pF seen from the feeder plug (antenna + cable); the patent uses a series capacitor to reach ~80 pF; another patent gives 80 pF as the matching capacitance "in correspondence with the impedance matching with the radio set" — [US5359335](https://patentimages.storage.googleapis.com/14/54/30/f2aef5bcd9a18c/US5359335.pdf); [US5258728 antenna circuit for multi-band antenna](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/5258728)
- Standard coax feeder has ~67 pF/m; car designs specify low-capacitance cable (<30 pF/m), and one glass-antenna patent limits the AM-side cable to ≤10 pF total after an impedance-conversion transformer — [US5359335](https://patentimages.storage.googleapis.com/14/54/30/f2aef5bcd9a18c/US5359335.pdf); [US6229493 glass antenna device](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/6229493)
- Usenet repair-group figure: RG-58U ~21 pF/ft vs RG-62U (traditional car-antenna/"AM-FM cable", 93 ohm) ~9.5 pF/ft — [sci.electronics.repair](https://groups.google.com/g/sci.electronics.repair/c/PNT_pCc-a3A) (informal source)
- FM antenna impedance in vehicle designs is "roughly 75 to 150 ohms"; using coax of the wrong impedance causes FM mismatch and reduced sensitivity — [US5359335](https://patentimages.storage.googleapis.com/14/54/30/f2aef5bcd9a18c/US5359335.pdf)
- Amplified factory antennas are powered by "phantom power": "a 12v power supply that is sent up the centre wire of the antenna cable"; aftermarket Fakra→DIN adapters inject it from the head unit's 12 V antenna/remote wire — [Incartec 21-123](https://incartec.co.uk/product/Fakra-to-Male-DIN-aerial-antenna-adapter-cable-With-12v-phantom-power-21-123); [Incartec 21-128 twin Fakra](https://incartec.co.uk/product/Twin-Fakra-to-DIN-aerial-adapter-cable-with-12v-phantom-power-bullet-connection-21-128)
- Metra 40-EU55 (single Fakra → Motorola male) is "amplified and requires accessory power"; Metra 40-EU56 (dual Fakra → Motorola) has a male Motorola plug plus a power wire — [Metra 40-EU55 at Zoro](https://www.zoro.com/metra-european-antenna-adapter-fakra-input-to-motorola-standard-male-output-amplified-40eu55/i/G8793929); [Metra 40-EU56](https://www.metraonline.com/product/40-EU56)
- Dual-Fakra cars use two antennas for FM diversity; cheap adapters (Aerpro APA70) "combine antenna signal into one" and do not retain diversity tuning — [Supercheap Auto APA70](https://www.supercheapauto.com.au/p/aerpro-antenna-adaptor-double-fakra-to-m-din-phantom-power-12v-both-antenna/SPO14990.html)
- Retail adapter listings do not publish the phantom voltage/current or pinouts — noted as a limitation in the search ([Metra 40-EU55](https://www.zoro.com/metra-european-antenna-adapter-fakra-input-to-motorola-standard-male-output-amplified-40eu55/i/G8793929))
- Nooelec NESDR SMArt v5: 100 kHz–1750 MHz, RTL2832U + R820T2/R860, 0.5 ppm TCXO, female SMA, **no bias tee**; HF/MW via Q-branch direct sampling. Bias tee versions are the SMArTee (4.5 V, 250 mA, always on, 25–1750 MHz) and SMArTee XTR — [Nooelec SMArt series FAQ](https://support.nooelec.com/hc/en-us/articles/360005807694-NESDR-SMArt-Series); [Nooelec datasheet](https://nooelec.com/datasheets/100700); [NESDR SMArTee](https://www.nooelec.com/store/sdr/nesdr-smartee.html)
- RTL-SDR Blog V4: software-switchable 4.5 V bias tee, up to 180 mA continuous; HF via built-in upconverter (28.8 MHz LO) rather than direct sampling, giving full gain control, no 14.4 MHz Nyquist folding, and "significantly improved" HF MDS, with minor VHF/UHF degradation vs V3; V4 now listed EOL, replaced by V4L (R828S tuner). A DC-shorted antenna can damage the bias-tee regulator — [RTL-SDR V4 datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf); [V4L datasheet](https://www.rtl-sdr.com/wp-content/uploads/2026/08/RTLSDR_V4L_Datasheet4.pdf); [RTL-SDR store](https://rtl-sdr.com/store); [V4L users guide](https://www.rtl-sdr.com/v4L)

### Inferences
- calc: FM mismatch 75→50 ohm is VSWR 1.5:1 → ~0.18 dB mismatch loss; 150→50 ohm is 3:1 → ~1.25 dB. A simple Motorola/DIN or Fakra → SMA adapter is acceptable on FM; no matching transformer needed. Use 75-ohm RG-6/RG-59 or the car's own cable freely on FM.
- calc (AM): a car whip of ~15 pF has Xc ≈ 10.6 kΩ at 1 MHz. Loaded by 50 ohm, the delivered voltage is ~50/10,600 ≈ −46 dB of the open-circuit EMF (−52 dB at 530 kHz, −41 dB at 1700 kHz). In a factory radio the same 15 pF into ~65 pF of cable+input gives a capacitive divider of 15/80 ≈ −14.6 dB, flat across the band. So feeding a passive car whip straight into a 50-ohm SDR via Motorola→SMA loses roughly **27–37 dB** relative to a car radio. Cable capacitance does not hurt the 50-ohm case further (it's swamped by 50 ohm) but nothing recovers it either.
- Since the RTL2832U direct-sampling path has a high noise floor and no front-end gain, that 30+ dB loss typically puts weak/moderate AM stations below the noise; the V4/V4L upconverter path (with gain) helps, but the fix is still an active/buffered antenna or a high-Z JFET buffer placed at the receiver end of a short, low-capacitance AM lead.
- An amplified factory antenna (Fakra with phantom 12 V) can be used with an SDR if 12 V is injected via a DC-block/bias-tee — an RTL-SDR 4.5 V bias tee will generally not be enough for antennas designed for 12 V (unverified per model; see gaps).

### Gaps
- No datasheet found giving the standard phantom voltage/current range (commonly stated 12 V, ~50–150 mA, but not verified) or the HSD connector role (HSD is used for LVDS/video/USB, not AM/FM RF — not verified here).
- No measured car-whip capacitance datasheet found; 10–25 pF is my estimate consistent with the patents' 80 pF system figure.
- TEF6686 antenna input impedance/AM front-end (it has a dedicated AM input intended for an active antenna/LNA in car designs) was not researched in this budget.

## 2. AM reception: active/buffered whips, powering, vehicle/RV noise and RV-specific antennas

### Takeaway
For AM on a 50-ohm SDR, use an E-field active antenna (JFET high-Z buffer driving 50 ohm, PA0RDT-mini-whip style or a commercial active whip) powered over the coax. Noise from vehicle/RV electronics (especially MPPT solar controllers, DC-DC converters and inverters) is the dominant real-world limit; placement away from the noise source, short ground paths, and ferrite chokes on noisy wiring are the practical mitigations, and ferrites do not always work when the source radiates from the board.

### Cited Findings
- ARRL: a 1 m whip at 10 kHz has ~2 MΩ source impedance; loading it directly with 50 ohm costs ~114 dB — [ARRL TIS article (active antennas)](https://www.arrl.org/files/file/Technology/tis/info/pdf/0109031.pdf)
- Worked example: a JFET buffer with ~5 pF input on a ~25 pF whip loses only ~1.6 dB (8.3 µV from 10 µV) — [PE2BZ active receiving antenna](https://pe2bz.philpem.me.uk/Comm/-%20Antenna/-%20AntennaDesign/ActiveReceivingAntenna/activeantenna1.htm)
- Buffer amplifiers present >100 kΩ input and ≤50 ohm output to drive coax — [WA1ION buffer amplifier links](https://qsl.net/w/wa1ion/buffers/bufamps.htm); [WA1ION buf-whip](https://www.qsl.net/wa1ion/buffers/buf-whip.txt)
- G8JNJ active monopole design adds a 10 dB output attenuator for stability with a reactive short whip — [G8JNJ active antennas PDF](https://qsl.net/l/lw7dfm/Antenas/Antena%20Mini%20Whip/MARTIN%20-%20G8JNJ%20-%20Active%20antennas.pdf)
- PA0RDT mini-whip: small metal plate + buffer amplifier, LW/MW/SW, ~100 mm × 40 mm in drain-pipe housing, BNC output, powered up the coax; at LF an active whip "behaves as a capacitance coupled to the electric field"; max output ~ −20 dBm to avoid receiver overload — [F8VOL PA0RDT mini-whip](https://www.qsl.net/f8vol/miniwhip.html); [PA2OHH](https://www.qsl.net/pa2ohh/22whip.htm); [PA0RDT original PDF](https://www.mikrocontroller.net/attachment/98670/pa0rdt-Mini-Whip.pdf)
- PA3FWM "simple and better" mini-whip circuit used at the Twente WebSDR (JFET + PNP, e.g. BFQ149/2N5583) — [PA3FWM miniwhip](https://pa3fwm.nl/projects/miniwhip)
- A battery-powered mini-whip variant ("Mobile-Whip") exists for portable SDR use — [rtl-sdr.com Mobile-Whip](https://www.rtl-sdr.com/mobile-whip-a-compact-battery-powered-mini-whip-style-active-hf-antenna-for-handhelds-and-sdrs/)
- RTL-SDR Blog notes additional HF filtering may be needed to attenuate strong MW AM for best HF performance (V3-era guidance) — [rtl-sdr.com new units](https://www.rtl-sdr.com/new-rtl-sdr-blog-units-now-available-in-store-hf-via-direct-sampling-software-switchable-bias-tee-less-noisespurs/)
- MPPT solar controller RFI: an Austrian ham group fixed Victron 100/50 noise with 15 turns on a large ferrite core (TX102/66/25-3C1) on each PV input line; another user cut noise ~80% by adding ferrites to all controller wiring — [Victron community: RF interference and MPPT](https://communityarchive.victronenergy.com/questions/1432/rf-interference-and-mppt-charge-controller.html); [Victron: RF noise from 150/60](https://communityarchive.victronenergy.com/questions/6205/rf-noise-from-150-60-solar-charger%EF%B9%96childToView=6342.html)
- Counter-report: many ferrites on in/out leads did nothing for a 20 A Renogy controller (noise radiated from the board); a purpose-built low-RFI controller was a "dramatic improvement" — [Good Sam forum: low-RFI solar controller](https://community.goodsam.com/discussions/rvmusthaves/low-rfi-solar-charge-controller-for-hams/2237872)
- Field observation: controller noise strong within ~3 ft, gone by ~7 ft; short ground path to vehicle frame recommended; practical workaround is switching the controller off (e.g. via Bluetooth) while listening — [Victron community thread](https://communityarchive.victronenergy.com/questions/6205/rf-noise-from-150-60-solar-charger%EF%B9%96childToView=22163.html)
- MPPT controllers reported "wiping out" FM reception as well — [Solar-electric forum](https://forum.solar-electric.com/discussion/comment/95710)
- Jeep (JL) owners report solar controller whine/RF interference fixed with ferrites — [JL Wrangler Forums](https://www.jlwranglerforums.com/forum/threads/cascadia-solar-controller-whine-rf-interference-snake-oil-works.72804/post-1518640)
- Winegard RV antennas with FM: Air 360+ (AR-360B) covers VHF 54–216, UHF 470–700, FM 87.5–108 MHz, 18 dB gain, 1 dB noise, 75 ohm, 12 V DC — [Winegard AR-360B manual](https://manualmachine.com/winegard/ar360b/19912253-user-manual). Winegard also sells an amplified omnidirectional HDTV+FM RV antenna (AR2V2S) — [CamperID AR2V2S](https://camperid.com/winegard/amplified-omnidirectional-hdtv-and-fm-radio-rv-antenna-mpn-ar2v2s.html)
- Winegard TV preamps often include FM traps (AP-8700: selectable −15 dB 88–108 MHz; HDA-200: switchable 20 dB FM trap) — which would remove FM if the TV antenna output is used for radio — [3 Star AP-8700](https://3starinc.com/winegard_ap-8700_antenna_pre-amplifier_with_fm_trap.html?manf=40); [Abt HDA-200](https://www.abt.com/Winegard-Distribution-Amplifier-HDA200/p/32594.html)

### Inferences
- Godwit's AM path should be: active E-field antenna (mini-whip or active whip, mounted on the roof away from solar controller/inverter/DC-DC and LED drivers) → 50-ohm coax → bias-tee power (V4/V4L 4.5 V, SMArTee 4.5 V, or external 5–12 V injector) → SDR. A mini-whip needs a reasonable counterpoise/ground via the coax shield; on a metal roof the vehicle body serves, but on fiberglass bodies (many RVs) the mini-whip's "ground" is the coax shield itself and it will pick up common-mode noise conducted along the coax — so a common-mode choke/ferrite at the antenna base and at the entry point is essential (inference consistent with the mini-whip/G8JNJ literature; not measured here).
- An alternative that preserves factory-radio behaviour: keep the passive car whip, and put a JFET high-Z buffer (>100 kΩ, low input C) inside the head-unit enclosure on a dedicated AM input, keeping the whip lead short and low-capacitance (RG-62-style). This approximates the car radio's ~80 pF environment.
- Winegard batwing/omni RV antennas are TV antennas with FM coverage via a 75-ohm 12 V amp; their AM performance is not specified and the FM amplifier is wideband (18 dB, VHF/UHF/FM) — it can overload an RTL-SDR in strong-signal areas; FM traps in TV preamps/distribution amps must be switched off if the radio is fed from the TV system.

### Gaps
- No measured comparison found between commercial active vehicle AM antennas and mini-whips on a vehicle.
- No quantified noise data (dBµV) for alternators, ignition, LED drivers or inverters on AM found within budget.
- Winegard RV FM-only antennas (e.g. "RFM" series) datasheets not found.

## 3. FM: best options for RVs/overlanders, amplifiers vs overload, RDS with multipath, diversity

### Takeaway
A passive, vertically oriented roof mast/whip (~75 cm quarter wave on a metal roof) remains the most robust FM antenna for an SDR; amplification only helps when the cable is long/lossy or the antenna is electrically poor (glass, fiberglass-body), and wideband amplifiers can overload an 8-bit RTL-SDR. Factory cars use dual-antenna diversity to fight multipath (which is what breaks RDS when moving); an SDR can replicate this with a phase-coherent dual-tuner (SDRplay RSPduo) using maximum-ratio combining.

### Cited Findings
- SDRplay RSPduo has two independent tuners that are phase- and time-synchronous; the datasheet lists simultaneous processing from 2 antennas for diversity and noise reduction — [RSPduo datasheet](https://www.sdrplay.com/docs/RSPduoDatasheetV0.5.pdf)
- SDRuno diversity uses Maximum Ratio Combining; needs two antennas seeing the same signal with some diversity; combining yields "up to 3 dB" improvement or noise removal by subtraction — [rtl-sdr.com: two-antenna diversity for RSPduo](https://www.rtl-sdr.com/two-antenna-diversity-for-the-sdrplay-rspduo-now-available-in-sdruno/)
- FM DX use of RSPduo phase control to null co-channel stations; null depth limited by the worse of the two antennas' directivity; two dipoles work — [Radio World](https://www.radioworld.com/?p=139889)
- Antenna spacing guidance: quarter wavelength for HF; on the order of a wavelength sometimes sufficient for diversity — [rtl-sdr.com diversity tag](https://www.rtl-sdr.com/tag/diversity/)
- Factory dual-Fakra diversity is lost when an adapter combines the two antenna signals into one — [Supercheap Auto APA70](https://www.supercheapauto.com.au/p/aerpro-antenna-adaptor-double-fakra-to-m-din-phantom-power-12v-both-antenna/SPO14990.html)
- Winegard AR-360B FM path gain 18 dB, 1 dB NF, 75 ohm — [AR-360B manual](https://manualmachine.com/winegard/ar360b/19912253-user-manual)
- MPPT noise "wiping out" FM is reported by RV/solar users — [Solar-electric forum](https://forum.solar-electric.com/discussion/comment/95710)

### Inferences
- calc: FM quarter wave ≈ 71–85 cm across 88–108 MHz (λ/4 ≈ 75 cm at 100 MHz).
- An FM LNA in front of an RTL-SDR helps only if the total noise figure is dominated by feedline loss; R820T2 already has its own LNA. In a strong-signal area (urban, or near transmitters on a mountain top common in overlanding) an 18 dB wideband preamp can drive the 8-bit ADC into clipping/intermod; use the SDR's AGC/gain control first and only add gain with a 88–108 MHz bandpass filter. A higher-dynamic-range SDR (14-bit SDRplay, Airspy) or a TEF6686 dedicated tuner is a better path to "factory quality" than adding gain.
- Diversity: the RSPduo's tuners can run with two FM antennas (e.g. a roof whip + a rear/glass antenna, separated ≥ λ/2 ≈ 1.5 m) and MRC combine; two unsynchronized RTL-SDR dongles cannot do coherent MRC without a shared clock and sample alignment, but could do switched (selection) diversity on RSSI/multipath metrics, which is how many factory "phase diversity" tuners started. RDS robustness when moving benefits more from diversity than from gain.

### Gaps
- No measured RDS block-error-rate vs. multipath/diversity data found.
- Whether the RSPduo dual-tuner mode supports enough bandwidth per tuner for FM-stereo+RDS concurrently with MRC in non-SDRuno software (e.g. SoapySDR on Linux/AGL) was not verified.

## 4. NOAA (162.40–162.55 MHz), AIS (161.975/162.025 MHz) and ADS-B (1090 MHz): antennas, cable loss, one antenna vs several, combiners

### Takeaway
NOAA and AIS are only ~0.5 MHz apart, so one vertical 162 MHz antenna (marine VHF/AIS whip or a ~46 cm quarter-wave on a ground plane) serves both. ADS-B needs its own 1090 MHz vertical (e.g. 5.5 dBi collinear) and low-loss coax: over a 20 ft RV run, RG-58 loses ~5 dB at 1090 MHz vs ~1.7 dB for LMR-240 and ~0.8 dB for LMR-400. Combining 162 MHz and 1090 MHz onto one SDR input is straightforward with a diplexer, but with one RTL-SDR only one service is decoded at a time.

### Cited Findings
- NWR transmits on seven frequencies 162.400–162.550 MHz; wavelength ~1.85 m; signal is vertically polarized so a vertical antenna outperforms a horizontal one; NWS publishes a 162.5 MHz ground-plane design — [NWS Columbia antenna page](https://www.weather.gov/cae/antenna.html); [NWS reception tips](https://www.weather.gov/cae/reception.html); [NWS NWR receivers](https://www.weather.gov/nwr/nwr_receivers)
- Commercial option: 160–164 MHz half-wave end-fed J-pole tuned for NOAA/railroad band — [J-Pole antenna NOAA](https://www.jpole-antenna.com/shop/railroad-band-noaa-weather-160-162-mhz-j-pole-antenna/)
- Midland sells a window-mount NOAA antenna (18-259W) for vehicles — [Adorama Midland 18-259W](https://www.adorama.com/mi18259w.html)
- Shakespeare 5215-AIS: 36 in (0.9 m) stainless whip, end-fed 1/2-wave, 3 dB gain, nominal SWR 1.5:1 at 162 MHz, 50 ohm, SO-239 — [Shakespeare 5215-AIS PDF](https://www.k-po.com/files/uploads/product-downloads/16136/5215-ais.pdf); [Defender listing](https://defender.com/en_us/shakespeare-masthead-squat-body-antenna-5215-ais)
- FlightAware 1090 MHz antenna: ~64–66 cm, ~2 cm diameter, N-female, 5.5 dBi toward horizon, brackets/U-bolts; ~US$44.95 at review time; poor SWR (~10) at 978 MHz UAT; now listed as discontinued at some retailers — [rtl-sdr.com review](https://www.rtl-sdr.com/review-flightaware-ads-b-antenna-and-filter/); [The Pi Hut (discontinued)](https://thepihut.com/products/flightaware-1090mhz-ads-b-antenna-66cm-26in). Alternatives: ADSBexchange 5.5 dBi 1090/978 N-female antenna — [ADSBexchange store](https://store.adsbexchange.com/products/5-5dbi-1090-978-antenna); 0.6 m 5.5 dBi and 1.2 m 8.5 dBi ADS-B antennas — [Pimoroni 0.6 m](https://shop.pimoroni.com/products/ads-b-1090-mhz-antenna-0-6m-5-5dbi); [Pimoroni 1.2 m](https://shop.pimoroni.com/en-us/products/ads-b-1090-mhz-antenna-1-2m-8-5dbi)
- One forum user suspected the FlightAware antenna's connector is 75 ohm; unconfirmed — [The Pi Hut listing/forum note via search](https://discussions.flightaware.com/t/new-inexpensive-1090mhz-antenna-for-sale/16931?page=9)
- Coax loss (dB/100 ft): at 150 MHz — LMR-400 ~1.5, LMR-240 3.0, RG-58A/U ~6.1–6.2; at ~1000 MHz — LMR-400 ~4.1, LMR-240 7.6 @900 / 9.9 @1500 MHz, RG-58A/U ~24 — [SCC ARES-RACES cable chart](https://scc-ares-races.org/operations/equipment/antenna/cable); [LMR-240 datasheet](https://cdn.shopify.com/s/files/1/1182/1008/files/LMR-240.pdf); [Universal Radio coax performance](https://www.universal-radio.com/catalog/cable/coaxperf.html); [W7RDR coax handout](https://www.qsl.net/w7rdr/documents/coax.pdf); [Raveon coax attenuation table](https://www.raveon.com/data_radio_info/table-coax-cable-attenuation-1036/)

### Inferences
- calc, typical RV run 20 ft: at 1090 MHz RG-58 ≈ 4.8–5 dB, LMR-240 ≈ 1.7 dB (interpolated ~8.4 dB/100 ft), LMR-400 ≈ 0.8–0.9 dB. At 162 MHz the same run costs ~1.2 dB (RG-58), ~0.6 dB (LMR-240), ~0.3 dB (LMR-400). For a 40 ft motorhome run double these. LMR-240 (or LMR-200/RG-8X class) is the sweet spot for a vehicle: flexible, routes through roof glands, <2 dB at 1090 over 20 ft. For long ADS-B runs, a mast-head 1090 MHz filtered LNA powered by bias-tee makes cable loss irrelevant.
- calc: quarter-wave at 162.5 MHz ≈ 46 cm (≈18 in) on a ground plane; a 1090 MHz quarter-wave ≈ 6.9 cm. A 162 MHz quarter-wave also resonates near 3λ/4 at ~490 MHz, not at 1090 MHz, so a VHF marine whip is a poor ADS-B antenna (expected high SWR, pattern lobes upward/downward) — this is the core multiband compromise.
- One antenna for everything: a wideband discone or a VHF/UHF "scanner" mobile whip covers 88–1300 MHz poorly but adequately for strong signals; it gives up ~several dB vs dedicated antennas at 162 and 1090 and is useless for AM. Better compromise: (1) AM active whip, (2) FM/162 MHz vertical (an FM quarter-wave whip works acceptably at 162 MHz as ~0.4–0.5λ; a marine VHF whip works acceptably on FM), (3) 1090 collinear.
- Combining: a VHF/UHF diplexer (e.g. 2 m/70 cm duplexers are wrong band; need a low-pass ≤ 200 MHz / high-pass ≥ 900 MHz diplexer) or a simple RF switch at the SDR lets one SDR input see multiple antennas; with a single time-shared RTL-SDR, software can select the antenna port per service. Since bias-tee DC must reach an active antenna through the diplexer, choose one with DC pass on the required port.

### Gaps
- No datasheet-grade vehicle ADS-B antenna (no-ground-plane, magnetic mount) found; marine/ADS-B collinears are designed for masts.
- Specific commercial 160 MHz/1090 MHz diplexers (with DC pass) not identified within budget.
- NWS page's 162.5 MHz ground-plane dimensions are in an image not readable here.

## 5. Combination/multiband roof antennas (RV/overland) and whether they expose AM/FM ports for an SDR

### Takeaway
Several multi-port roof antennas include AM/FM alongside LTE/GNSS/Wi-Fi with separate pigtails: Taoglas MA1045 "Supercombo" (2×GPS, LTE, SiriusXM, AM/FM/WB on its own Fakra, active, powered by a separate 12 V wire) and Taoglas MAR140 (5G MIMO, Wi-Fi, AM/FM+TV on F-type); Panorama offers VHF/UHF/AM/FM options on its non-metallic-panel MIMO antennas; Poynting's MIMO/Puck lines are cellular/Wi-Fi/GNSS only. Any of these with a discrete AM/FM output can feed an SDR via Fakra or F → SMA, with 12 V supplied separately.

### Cited Findings
- Taoglas MA1045 Supercombo 5-in-1: 2× GPS, LTE (4G/3G/2G), SiriusXM, AM/FM/WB; AM/FM output on its own Fakra; AM/FM/WB circuits powered by a 12 V DC wire — [Taoglas MA1045](https://taoglas.com/product/ma1045-supercombo-vehicle-mount-5in1-antenna) (via search summary; product page returned 403 on direct fetch). Note "WB" = NOAA weather band, i.e. the active AM/FM element is specified to cover 162 MHz.
- Taoglas MAR140: low-profile RV antenna with 5G MIMO, Wi-Fi, AM/FM and TV; separate pigtails — SMA(M) LTE, RP-SMA(M) Wi-Fi, F-type for TV/radio; GPS not listed — [Westward Sales MAR140](https://westwardsales.com/taoglas-mar140-antenna); [RVMobileInternet Taoglas Synergy](https://www.rvmobileinternet.com/gear/taoglas-synergy/)
- Panorama multi-band flyer: 2×2 MIMO cellular 617–960/1710–6000 MHz, GNSS with LTE Band 13/14 filtering, "a range of VHF, UHF or AM/FM options"; for non-metallic panels; 90 mm high × 243 mm long — [Panorama 4G/5G sharkfin MIMO flyer](https://www.getwirelessllc.com/wp-content/uploads/2023/07/Panorama-4G-5G-Sharkfin-MIMO-Antenna-Flyer.pdf)
- Panorama 5-in-1 dome (4× LTE + GNSS) with 5 m Fakra cables — [Tecisoft listing](https://www.tecisoft.com/products/panorama-5in1-dome-antenna-4xlte-gnss-fakra-cbls); Panorama low-profile RV antenna — [RVMobileInternet](https://www.rvmobileinternet.com/gear/panorama-low-profile/)
- Poynting MIMO-1: 2× Wi-Fi + 2× LTE + GPS in a low-profile dome; Puck series: MIMO cellular, dual-band Wi-Fi, GPS/GLONASS — no AM/FM — [RVMobileInternet Poynting combination antennas](https://www.rvmobileinternet.com/gear/poynting-combination-antennas/); [Poynting MIMO-1](https://www.rvmobileinternet.com/gear/poynting-mimo-1/); [Poynting 7-in-1 at TechnoRV](https://technorv.com/products/poynting-7-in-1-mimo-omnidirectional-antenna)
- Furrion FAN73B7C rooftop: marketed as AM/FM + LTE + TV antenna (GPS not stated) — via search summary of RV retail listings ([RVMobileInternet search hub](https://www.rvmobileinternet.com/?p=41253))

### Inferences
- For Godwit, a Taoglas MA1045-class antenna is the most "factory-like" single-hole solution: active AM/FM/WB on Fakra (needs 12 V on a separate wire — easy to provide from the vehicle accessory rail, no SDR bias tee needed), GNSS for the head unit's nav, LTE for a modem. ADS-B and AIS would still need a separate 1090 MHz antenna and (for AIS range) ideally a dedicated VHF whip.
- RV TV/radio combos (Taoglas MAR140, Furrion, Winegard) output AM/FM+TV on a 75-ohm F connector; F-female→SMA-male adapter is fine on FM; AM performance through a TV-oriented 75-ohm amplifier is unknown and likely weak.
- Morad (Canadian marine/commercial vehicle maker) and Shakespeare offer marine AM/FM whips and VHF/AIS whips; no Morad combo with separate AM/FM port was confirmed.

### Gaps
- Taoglas MA1045 AM/FM gain, NF, supply current, and price not retrieved (page 403).
- No Morad datasheet retrieved; no Winegard RV FM-specific datasheet retrieved.
- Whether Panorama's AM/FM option is active and what connector it uses not confirmed.

## 6. Concrete good/better/best recommendations

### Takeaway
(a) AM+FM only: Good = existing passive car/marine AM/FM whip + Motorola→SMA adapter on FM, plus a JFET high-Z AM buffer; Better = active roof AM/FM antenna (12 V) or bias-tee-powered mini-whip for AM + passive FM whip, with RTL-SDR V4/V4L; Best = dedicated tuner (TEF6686) or 14-bit SDR (SDRplay RSPduo for 2-antenna FM diversity) with an active AM antenna and two spaced FM antennas. (b) All services: add a 162 MHz vertical (marine VHF/AIS whip) and a 1090 MHz collinear on LMR-240, optionally a Taoglas-class combo for GNSS/LTE/AM-FM-WB, and either RF switching into one SDR or separate SDRs per service.

### Cited Findings (supporting part/price facts)
- RTL-SDR V4 bias tee 4.5 V/180 mA, HF upconverter; V4L successor — [V4 datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf); [V4L guide](https://www.rtl-sdr.com/v4L)
- Nooelec SMArt v5 has no bias tee; SMArTee has always-on 4.5 V/250 mA — [Nooelec FAQ](https://support.nooelec.com/hc/en-us/articles/360005807694-NESDR-SMArt-Series)
- RSPduo two phase-coherent tuners for diversity — [RSPduo datasheet](https://www.sdrplay.com/docs/RSPduoDatasheetV0.5.pdf)
- PA0RDT mini-whip, coax-powered, ~−20 dBm max output — [F8VOL](https://www.qsl.net/f8vol/miniwhip.html)
- Shakespeare 5215-AIS 162 MHz 3 dB whip, SO-239 — [5215-AIS PDF](https://www.k-po.com/files/uploads/product-downloads/16136/5215-ais.pdf)
- FlightAware 1090 MHz 5.5 dBi N-female, ~$45 — [rtl-sdr.com review](https://www.rtl-sdr.com/review-flightaware-ads-b-antenna-and-filter/)
- Fakra→Motorola/DIN adapters with 12 V phantom injection (Metra 40-EU55/40-EU56, Incartec 21-123/21-128) — [Metra 40-EU56](https://www.metraonline.com/product/40-EU56); [Incartec 21-123](https://incartec.co.uk/product/Fakra-to-Male-DIN-aerial-antenna-adapter-cable-With-12v-phantom-power-21-123)
- Coax losses (LMR-240 3.0 dB/100 ft @150 MHz, ~7.6 @900 MHz) — [LMR-240 datasheet](https://cdn.shopify.com/s/files/1/1182/1008/files/LMR-240.pdf)
- Taoglas MA1045 AM/FM/WB + GPS + LTE combo, Fakra, 12 V wire — [Taoglas MA1045](https://taoglas.com/product/ma1045-supercombo-vehicle-mount-5in1-antenna)

### Inferences (recommendation tables — prices are rough street estimates, not sourced unless noted)

(a) AM + FM only
| Tier | Antenna | Front end / receiver | Connectors & cable | Notes |
|---|---|---|---|---|
| Good | Existing vehicle passive mast or a marine/RV passive AM/FM whip | NESDR SMArt v5 or RTL-SDR V4/V4L; small in-enclosure JFET high-Z buffer on a dedicated AM input (DIY, <$20 parts) | Motorola plug → SMA-male adapter for FM; keep AM lead short, low-C (RG-62-type) | FM loss from 75→50 ohm ≈0.2 dB; without the buffer, AM is ~30–40 dB down vs a car radio |
| Better | Active AM/FM roof antenna with 12 V feed (Fakra/Motorola) or bias-tee-powered mini-whip (AM) + passive ~75 cm FM whip | RTL-SDR V4/V4L (bias tee on for mini-whip) | Fakra→SMA with DC injector (12 V via fused accessory wire); LMR-240 or RG-58 for short runs | Mount AM element on roof, away from solar controller/inverter/DC-DC; ferrite chokes on coax at entry and on noisy DC leads |
| Best | Active AM antenna + two FM antennas ≥1.5 m apart (e.g. front roof + rear) | TEF6686 tuner (AM+FM with RDS, designed for car antennas), or SDRplay RSPduo for 2-antenna MRC FM diversity; AM via RSPduo Hi-Z/HF port | As above | Highest dynamic range; diversity aids RDS while moving |

(b) All services
| Tier | AM/FM | 162 MHz NOAA+AIS | 1090 ADS-B | GNSS/LTE/Wi-Fi | Receiver plan |
|---|---|---|---|---|---|
| Good | As (a) Good | Use the FM whip (acceptable at 162 MHz) | Small 1090 MHz mag-mount or quarter-wave (~6.9 cm) on roof, short RG-58 (<10 ft) | Existing | Single RTL-SDR, time-shared, manual/auto RF switch |
| Better | Active AM + passive FM whip | Marine VHF whip (e.g. Shakespeare 5215-AIS, 36 in, SO-239) or 46 cm ground-plane | FlightAware/ADSBx 5.5 dBi collinear on LMR-240 (~1.7 dB/20 ft) | Taoglas/Panorama/Poynting combo for GNSS+LTE | Two RTL-SDRs (one VHF/AM-FM time-shared, one dedicated 1090) |
| Best | Taoglas MA1045-class active AM/FM/WB + second FM antenna for diversity | Dedicated AIS/VHF whip on its own SDR (AIS needs both 161.975 & 162.025 — one RTL-SDR at 2.4 MS/s covers both) | 1090 collinear + filtered masthead LNA (bias-tee) on LMR-240/400 | MA1045 GNSS+LTE | RSPduo or TEF6686 for broadcast, separate RTL-SDRs for AIS and ADS-B running continuously |

Installation notes (inference from cited sources): vertical polarization for NOAA/AIS/ADS-B; place antennas on metal roof center where possible (ground plane), or use ground-plane-independent (end-fed half-wave/collinear) types on fiberglass; route coax away from solar/inverter wiring; common-mode chokes at roof entry; DC-block any port where a bias tee is enabled toward a DC-shorted antenna (V4 warning); use FM bandpass/notch filters on ADS-B/AIS inputs in strong FM areas.

### Gaps
- Current (late-2026) prices and availability for Taoglas MA1045, Panorama AM/FM combos, Shakespeare/Morad whips, and TEF6686 modules not verified.
- No sourced product recommendation for a commercial vehicle-grade active AM antenna with SMA/bias-tee powering; DIY mini-whip/JFET designs are the best-documented route.
