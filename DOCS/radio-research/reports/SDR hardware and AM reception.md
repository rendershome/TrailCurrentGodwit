# Split Godwit's radio across two receivers

> **Verified 2026-11-21.** Some statements below were corrected after independent checking. See [VERIFICATION.md](VERIFICATION.md); where they disagree, the verification log wins.

**The best upgrade for Godwit is a second receiver, not a filter.** An Airspy HF+ Discovery (about $169–199) should take over broadcast AM, FM and NRSC-5 IBOC. The existing NESDR SMArt v5 then runs ADS-B, AIS and NOAA weather radio full time. Of the SDRs the notes cover, the HF+ Discovery is the only one built for broadcast reception. It has dedicated MW and FM preselectors, about 110 dB (HF) and 95 dB (VHF) blocking dynamic range, and an open libusb driver. Its 660 kHz of alias-free bandwidth holds one FM station plus both IBOC sidebands, or any AM channel. An NXP TEF6686 car-radio tuner module is the strongest analog alternative, at $10–50. It has multipath suppression and a noise blanker in silicon, but it arrives with an unpublished NXP patch blob, GPL-3.0 firmware and no Linux driver. Analog-to-digital blend would also need a second audio clock domain. SDRplay receivers are good hardware, but their closed API's 2021 EULA grants no redistribution right, so they cannot go in a public image. The user's ~200 MHz low-pass would not touch the blockers that matter (FM, 152–159 MHz paging, VHF-high TV). It would remove ADS-B and does nothing as an AM anti-alias filter. Each service needs its own filter instead. AM can start today on the SMArt v5 by Q-branch direct sampling, but broadcast-grade AM needs three things: a 520–1710 kHz band-pass, a high-impedance active antenna, and the NRSC-1-C de-emphasis defined below. NRSC-5 IBOC should be an opt-in feature. The GPL-3.0 nrsc5 decoder would run in a separate helper process, fed from the same I/Q capture as analog FM, with blending done in Godwit's MIT audio code.

## The HF+ Discovery wins among SDRs; a TEF6686 wins only on analog-in-motion

### Performance: preselection matters more than ADC bits

All the candidates share the same weakness that limits the current dongle: a wideband front end feeding a limited-range ADC. The NESDR SMArt v5 pairs an RTL2832U with an R820T2. It covers 25–1750 MHz by quadrature sampling and 0.1–25 MHz by direct sampling, at up to 3.2 MS/s with a 0.5 ppm TCXO ([Nooelec datasheet](https://nooelec.com/datasheets/100700)). Its effective resolution is about 7 bits, which leaves roughly 50 dB of usable in-channel range.

The **Airspy HF+ Discovery** covers 0.5 kHz–31 MHz and 64–260 MHz. Its features, all manufacturer figures ([Airspy](https://airspy.com/airspy-hf-discovery/)), are:
- a four-bank HF preselector;
- a separate FM preselector with US, EU, JP and OIRT variants, plus FM-optimised IF filters;
- **+15 dBm (HF) and +13 dBm (VHF) IP3**;
- −141.5 dBm MDS in a 500 Hz bandwidth in the FM band;
- a 0.5 ppm clock.

Independent tests agree:
- In a 2017 review it had "a very noticeable sensitivity edge on the broadcast FM spectrum" over the Airspy Mini and SDRplay RSP2, and overload on broadcast AM was "non-existent" ([rtl-sdr.com](https://www.rtl-sdr.com/our-review-of-the-airspy-hf-compared-against-colibrinano-airspy-mini-rsp2-rtl-sdr/)).
- SM5BSZ's 7.2 MHz blocking tests placed it about 3 dB behind the Perseus, against 18 dB behind for Airspy plus SpyVerter and 23 dB for the RSP1 ([rtl-sdr.com](https://www.rtl-sdr.com/?p=34843)).
- On MW it matched an $899 Elad FDM-DUOr closely ([rtl-sdr.com collection](https://www.rtl-sdr.com/airspy-hf-discovery-collection-of-tests-and-reviews/)).

Its weaknesses are integration risks, not performance ones:
- **Spurs from noisy USB power.** In 2022 RadioDX.pl found unexpected carriers and spurs on wideband FM on every Discovery its group owned. They traced the cause to USB supply noise reaching the clock generator. A 47 pF capacitor plus an aluminium case reduced the spurs but did not remove them ([RadioDX.pl](https://radiodx.pl/?p=12806)).
- **Limited band coverage.** It does not cover 1090 MHz, and 162 MHz falls in its "reduced performance" extended range.

The other SDRs fall behind for this job:
- **Airspy R2 and Mini.** They give 12-bit sampling (10.4 ENOB on the R2) and up to 9 MHz of bandwidth, but they use the same R820T2 tuner family and have no MW without a SpyVerter ([Airspy R2](https://airspy.com/airspy-r2/)). In the 2017 test, the Mini plus SpyVerter was "barely audible" on a 120 km FM station that the HF+ received "fairly clearly", and it overloaded on broadcast AM ([rtl-sdr.com](https://www.rtl-sdr.com/our-review-of-the-airspy-hf-compared-against-colibrinano-airspy-mini-rsp2-rtl-sdr/)). Its real strength is ADS-B: one forum comparison reported roughly 50% more messages than an R820T2 dongle ([rtl-sdr.com](https://www.rtl-sdr.com/rtl-sdr-vs-airspy-on-ads-b-reception/)).
- **SDRplay RSP family.** All models cover 1 kHz–2 GHz, with an 11-band preselector, switchable MW/FM and DAB notches, and 14-bit output below about 6 MS/s ([SDRplay RSP1B](https://www.sdrplay.com/rsp1b/); [rtl-sdr.com RSP1A review](https://www.rtl-sdr.com/a-review-of-the-sdrplay-rsp1a/)). The RSP1B (£105.80 ex VAT) and the RSPdx-R2 (£188) specifically improve noise below 1 MHz ([rtl-sdr.com](https://www.rtl-sdr.com/sdrplay-rspdx-r2-released/)). No independent MDS or IP3 figures at 1 MHz or 100 MHz were found, and MW DXers still call strong-signal handling the RSP1A's weak point ([RadioReference](https://forums.radioreference.com/threads/rsp1b.468670)).
- **RTL-SDR Blog V4** (about $35). It is the cheap AM fix. It replaces direct sampling with an upconverter and a triplexer, and its MDS at 1 MHz is about −134 dBm against about −109 dBm for V3-style direct sampling ([V4 datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)). The costs are 2–3 dB of VHF/UHF sensitivity and a 500 kHz floor right at the band edge. It is also now listed as end-of-life, replaced by a V4L built on an R828S tuner whose datasheet (August 2026) was not reviewed ([RTL-SDR store](https://rtl-sdr.com/store); [V4L datasheet](https://www.rtl-sdr.com/wp-content/uploads/2026/08/RTLSDR_V4L_Datasheet4.pdf)).

### Dedicated tuners: excellent analog, awkward to integrate

The **TEF6686/6687** is an AEC-Q100 automotive "alignment-free digital receiver". It covers FM 65–108 MHz and AM LW/MW/SW. On-chip it has Improved Multipath Suppression, a channel equaliser, AM and FM noise blankers, soft mute, stereo high blend, RDS/RBDS decoding, I2S output and I2C control ([NXP TEF668X](https://www.nxp.com/products/TEF668X); [NXP brochure](https://cache.nxp.com/docs/en/brochure/75017468.pdf)). In other words, Godwit's Phase 2–3 roadmap is already built into the chip. The worldwide FM-DX Webserver network is "mostly TEF6686-based" ([rtl-sdr.com](https://www.rtl-sdr.com/tag/tef6686/)). IEEE Spectrum calls the open PE5PVB receiver "perhaps the highest-performing radio of its kind, ever" ([IEEE Spectrum](https://spectrum.ieee.org/hacking-a-car-radio-chip)).

The evidence for that reputation is thin, though: **no published side-by-side lab measurement of the TEF6686 against any SDR exists**. WTFDA posters dispute it both ways, pointing to the SDRs' narrow instantaneous dynamic range and to the TEF's "miniscule processing power" ([WTFDA](https://forums.wtfda.org/forum/dx-equipment/1815-tef6687-vs-tef6686-vs-others)).

Integration is where the TEF costs most:
- **No Linux driver.** Mainline `drivers/media/radio` has no TEF668x, Si473x or Si468x driver ([Linux kernel](https://github.com/torvalds/linux/tree/master/drivers/media/radio)).
- **A firmware blob of unclear provenance.** The firmware patch is sent from the host at every boot ([IEEE Spectrum](https://spectrum.ieee.org/hacking-a-car-radio-chip)), and NXP does not publish it.
- **GPL-3.0 reference code.** The reference firmware, PE5PVB/TEF6686_ESP32, cannot be merged into MIT code ([GitHub](https://github.com/PE5PVB/TEF6686_ESP32)).
- **Fragile I2S capture.** I2S on the Pi 5 has reported overlay and slave-mode pitfalls ([raspberrypi/linux #5747](https://github.com/raspberrypi/linux/issues/5747)).

The two Skyworks parts are weaker fits:
- **Si4735.** A consumer portable-radio part: 2.2 µV EMF FM sensitivity, 50 dB adjacent and 70 dB alternate selectivity, and 25 µV EMF AM sensitivity ([Si4734/35 data short](https://www.skyworksinc.com/-/media/skyworks/sL/documents/public/data-shorts/si4734-35-short.pdf)). That is below automotive class.
- **Si4689.** It decodes NRSC-5 on-chip, with blend ([Si4689 data short](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-shorts/Si4689-A10-short.pdf)). That only helps a commercial product that holds an Xperi licence.

### Licensing decides the shortlist for an open image

| Receiver | Broadcast AM/FM quality | Driver and licence for an MIT AGL image | Price (2026) | Integration notes |
|---|---|---|---|---|
| NESDR SMArt v5 (current) | FM adequate; AM only by unfiltered 8-bit direct sampling | librtlsdr, GPL-2.0, already shipped | owned | One device shared by every service |
| RTL-SDR Blog V4 / V4L | FM same class; AM about 25 dB better MDS than direct sampling | Blog fork or osmocom ≥ 2.0 ([rtl-sdr.com](https://www.rtl-sdr.com/v4/)) | ~$35 ([HamDeals](https://hamdeals.com/blog/sdr-dongles-ranked/)) | V4 EOL; V4L specs unverified |
| **Airspy HF+ Discovery** | Best SDR for MW and FM | libairspyhf; repo has BSD-3 and GPL-2.0 files, so verify per file; no OE recipe ([GitHub](https://github.com/airspy/airspyhf)) | ~$169–199 ([HamDeals](https://hamdeals.com/blog/sdr-dongles-ranked/)) | Needs clean 5 V and a metal case; no 1090 MHz |
| Airspy R2 / Mini | FM somewhat better than RTL; no MW | libairspy in meta-sdr ([OE layer index](https://layers.openembedded.org/layerindex/recipe/261041)) | Mini $99–149 ([rtl-sdr.com](https://www.rtl-sdr.com/review-airspy-mini/)) | Best used as an ADS-B receiver |
| SDRplay RSP1B / RSPdx-R2 / RSPduo | Good preselection; RSPduo can run 2-antenna diversity | Closed API; EULA grants only "display, perform", with no redistribution ([licence mirror](https://raw.githubusercontent.com/srcejon/sdrplayapi/master/sdrplay_license.txt)) | £106 / £188 / £214 ex VAT | Each builder fetches and accepts the API; never in the default image |
| TEF6686/6687 module | Best analog in motion (community consensus, unmeasured) | GPL-3.0 firmware on an MCU; NXP patch blob unpublished | $10–50 ([IEEE Spectrum](https://spectrum.ieee.org/hacking-a-car-radio-chip)) | XDR protocol over USB-serial; I2S or analog audio |
| Si4735 | Consumer-grade | No Linux driver; SSB patch has a no-commercial-use request ([PU2CLR](https://github.com/pu2clr/SI4735/wiki)) | low | Not worth the integration |
| Si4689 | Good, with on-chip IBOC | Licensed IBOC firmware | — | Commercial derivatives only |

**SDRplay's licence is the deciding factor.** Its API ships ARM64 binaries ([SDRplay API](https://www.sdrplay.com/api/)), and the MIT SoapySDRPlay3 module wraps it ([GitHub](https://github.com/pothosware/SoapySDRPlay3)). However, the current EULA forbids reverse engineering and transfer and grants no right to reproduce or distribute. The open alternative, libmirisdr-5, loses the preselector and notch control that justifies buying an RSP ([Gqrx group](https://groups.google.com/g/gqrx/c/uf8fYPbcNnQ)).

The SDRplay hardware is still worth noting for one feature: the **RSPduo's two phase-coherent tuners**. They allow maximum-ratio combining of two FM antennas for "up to 3 dB" gain ([rtl-sdr.com](https://www.rtl-sdr.com/two-antenna-diversity-for-the-sdrplay-rspduo-now-available-in-sdruno/)). In dual-tuner mode, each tuner is limited to a 1.536 MHz IF ([SDRplay RSPduo](https://www.sdrplay.com/rspduo/)).

## A 200 MHz low-pass guards the wrong frequencies

### What the proposed filter would and would not do

The R820T's own tracking filter is about ±3 MHz wide. HB9AJG measured its overload limit at **about −62 to −68 dBm for in-window signals**, but about **−20 dBm two-tone at 50 MHz away** ([HB9AJG](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)). Distant UHF signals are therefore rarely the problem; nearby VHF ones are.

A typical ~200 MHz part, the Mini-Circuits BLP-200+, passes DC–190 MHz and does not reach 20 dB attenuation until 290 MHz ([BLP-200+ datasheet](https://www.mouser.com/datasheet/3/3705/1/BLP-200+.pdf)). Against the blockers a vehicle actually meets, it would do the following:

| Blocker | Effect of a ~200 MHz LPF |
|---|---|
| In-band FM broadcast | None |
| 152–159 MHz paging, 3–10 MHz from NOAA/AIS | None; this skirt region is where HB9AJG saw intermodulation first |
| VHF-high TV at 174–216 MHz | Passes almost untouched |
| UHF TV, LTE, 929 MHz paging | Removed; the R820T already tolerates these |
| ADS-B at 1090 MHz | Destroyed |

The filter also **fails as an AM anti-alias filter**. Direct sampling runs the ADC at 28.8 MS/s with no filtering of its own, so everything folds into 0–14.4 MHz. A signal at f appears at |f − n·28.8 MHz|. Computed against the MW band:
- CB at 26.965–27.405 MHz lands on 1395–1835 kHz.
- The bottom of the 10 m band (28.0–28.27 MHz) lands on 530–800 kHz.
- 87.9–88.1 MHz lands on 1.5–1.7 MHz.
- TV channel 6, 2 m and DTV channel 11 at 199.9–203.3 MHz also fold onto MW. DTV channel 11 sits right at the 200 MHz corner, where the filter barely attenuates.

The rtl-sdr.com direct-sampling guidance makes the same point: filter the band you want ([rtl-sdr.com](https://www.rtl-sdr.com/low-pass-filter-for-rtl-sdr-direct-sampling-mode/)).

### What each service needs

**Gain control is the first filter.** On the R820T, each dB of gain reduction buys about a dB of overload headroom, and tuner AGC costs about 10 dB ([HB9AJG](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)). This makes plan item 4.1 a hardware item in practice. Beyond gain control, each service needs a different filter:

| Service | Filter | Passband | Stopband target | Off-the-shelf part, or build |
|---|---|---|---|---|
| AM by direct sampling | MW band-pass, or a 2–3 MHz LPF plus a ~450 kHz HPF | 520–1710 kHz | ≥ 40 dB at 5 MHz; ≥ 60 dB at 26–30 MHz; ≥ 70 dB at 54–230 MHz; shielded to ≥ 1 GHz | No commercial MW BPF found. Nooelec's Flamingo AM is a *band-stop* ([Nooelec](https://www.nooelec.com/store/flamingo-am.html)). SLP-15+ passes the 1.6–14.4 MHz FM aliases ([Mini-Circuits](https://www.minicircuits.com/pages/s-params/SLP-15+_VIEW.pdf)). Build one. |
| FM | None by default; optional 87–109 MHz BPF | 88–108 MHz | ≥ 30 dB at 60 and 150 MHz | Gain management does most of the work. A notch cannot protect FM from FM. |
| NOAA 162.400–162.550 / AIS 161.975, 162.025 MHz | Marine-VHF BPF, or an FM band-stop | 155–164 MHz | ≥ 40 dB at 88–108 MHz; ≥ 30 dB at 174 MHz and up | RTL-SDR Blog FM band-stop (> 50 dB, $16.95) ([rtl-sdr.com](https://www.rtl-sdr.com/product/rtl-sdr-blog-broadcast-fm-band-stop-filter/)) or Flamingo+ FM (> 60 dB, passes DC) ([Nooelec](https://www.nooelec.com/store/flamingo-plus-fm.html)) |
| ADS-B 1090 MHz | SAW BPF with an LNA at the antenna | 1075–1105 MHz | ≥ 40 dB at 925–960 MHz (E-GSM) and LTE 700–900 MHz | RTL-SDR Blog 1090 LNA, SAW on both sides, 60–80 dB out of band ([rtl-sdr.com](https://www.rtl-sdr.com/new-product-rtl-sdr-blog-1090-mhz-ads-b-lna/)) |

The V4's built-in notches attenuate by only "a few dB", and its broadcast notch switches off when tuned to 85–112 MHz ([V4 datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf); [tuner_r82xx.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/tuner_r82xx.c)). Its triplexer, though, improves out-of-band isolation by 28–43 dB.

If one dongle must keep serving every band, a GPIO-switched filter bank works. It needs a switch that reaches down to MW: the ADG904 covers DC to about 2.5 GHz ([ADI](https://www.analog.com/en/products/adg904.html)) and the ADRF5040 covers 9 kHz–12 GHz ([ADI](https://www.analog.com/en/products/adrf5040.html)). The pSemi PE42441 stops at 10 MHz, so it cannot carry the MW path. A two-receiver design makes most of this bank unnecessary, and at about $35 per dongle that is cheaper than building it.

## Car whips lose 30 dB of AM into a 50-ohm SDR

### AM needs a high-impedance buffer or active antenna

On AM a car whip is a small capacitive source. Factory radios are designed around about **80 pF of total antenna-plus-cable capacitance**, and they use low-capacitance cable for that reason ([US5359335](https://patentimages.storage.googleapis.com/14/54/30/f2aef5bcd9a18c/US5359335.pdf)).

Taking a whip of about 15 pF (an estimate), the arithmetic is:
- **50 Ω SDR input:** the whip's 10.6 kΩ reactance at 1 MHz delivers −46 dB of the open-circuit voltage.
- **Car radio:** the capacitive divider loses only about −14.6 dB.

A passive whip on an SMA adapter therefore starts **roughly 27–37 dB behind a car radio** (computed). That gap matters most on the SMArt v5, whose direct-sampling path bypasses the tuner LNA entirely. Gain settings in this mode do nothing.

The fix is either a JFET buffer (input above 100 kΩ, output ≤ 50 Ω) at the receiver end of a short, low-capacitance lead ([WA1ION](https://qsl.net/w/wa1ion/buffers/bufamps.htm)), or an E-field active antenna in the PA0RDT mini-whip style mounted on the roof ([PA3FWM](https://pa3fwm.nl/projects/miniwhip)). A JFET buffer with about 5 pF input loses only about 1.6 dB on a 25 pF whip ([PE2BZ](https://pe2bz.philpem.me.uk/Comm/-%20Antenna/-%20AntennaDesign/ActiveReceivingAntenna/activeantenna1.htm)).

Powering an active antenna takes some care:
- **The SMArt v5 has no bias tee** ([Nooelec FAQ](https://support.nooelec.com/hc/en-us/articles/360005807694-NESDR-SMArt-Series)), so Godwit needs its own DC injector.
- Factory and aftermarket amplified masts expect **12 V phantom power** up the centre conductor ([Incartec](https://incartec.co.uk/product/Fakra-to-Male-DIN-aerial-antenna-adapter-cable-With-12v-phantom-power-21-123)). An RTL-SDR's 4.5 V bias tee will generally not run them.

### Noise sources in RVs and overland rigs

In RVs and overland rigs, the local noise sources decide what can be heard, more than the receiver does:
- **MPPT solar controllers** are the worst offenders. One Victron owner found the noise strong within about 3 ft and gone by about 7 ft ([Victron community](https://communityarchive.victronenergy.com/questions/6205/rf-noise-from-150-60-solar-charger%EF%B9%96childToView=22163.html)).
- **Ferrites help on some controllers.** Fifteen turns on a large core on each PV lead fixed one Victron unit ([Victron community](https://communityarchive.victronenergy.com/questions/1432/rf-interference-and-mppt-charge-controller.html)).
- **Ferrites fail on others.** They did nothing for a controller that radiated from its board, where only a low-RFI replacement helped ([Good Sam forum](https://community.goodsam.com/discussions/rvmusthaves/low-rfi-solar-charge-controller-for-hams/2237872)).
- **FM suffers too.** Controllers have been reported "wiping out" FM reception as well ([Solar-electric forum](https://forum.solar-electric.com/discussion/comment/95710)).

Mount the AM element on the roof, away from the controller, inverter, DC-DC converter and LED drivers. On fibreglass bodies the coax shield becomes the counterpoise, so fit common-mode chokes at the antenna base and at the roof entry.

### FM, 162 MHz and 1090 MHz each want their own vertical

FM is forgiving:
- A 75 Ω car mast into 50 Ω costs only about 0.2 dB (computed), and a passive vertical of about 75 cm on a metal roof is the robust choice.
- Wideband RV TV/FM amplifiers such as the Winegard AR-360B (18 dB gain over VHF, UHF and FM) can overload an 8-bit dongle ([Winegard manual](https://manualmachine.com/winegard/ar360b/19912253-user-manual)).
- Many TV preamps carry switchable FM traps that must be off if radio is fed from the TV system ([3 Star](https://3starinc.com/winegard_ap-8700_antenna_pre-amplifier_with_fm_trap.html?manf=40)).

NOAA and AIS sit within 0.6 MHz of each other, so one vertical serves both:
- NOAA is vertically polarised ([NWS](https://www.weather.gov/cae/antenna.html)).
- The Shakespeare 5215-AIS (36 in, end-fed half-wave, 3 dB, 50 Ω) is a ground-plane-independent option for fibreglass roofs ([Shakespeare PDF](https://www.k-po.com/files/uploads/product-downloads/16136/5215-ais.pdf)).
- A 46 cm quarter-wave works on metal.

ADS-B needs its own antenna:
- A 5.5 dBi collinear, such as the FlightAware ($44.95 at review, now discontinued at some sellers) or the ADSBexchange equivalent ([rtl-sdr.com](https://www.rtl-sdr.com/review-flightaware-ads-b-antenna-and-filter/); [ADSBexchange](https://store.adsbexchange.com/products/5-5dbi-1090-978-antenna)).
- Low-loss coax: over 20 ft at 1090 MHz, RG-58 loses about 5 dB against about 1.7 dB for LMR-240 (computed from [LMR-240](https://cdn.shopify.com/s/files/1/1182/1008/files/LMR-240.pdf) and [Universal Radio](https://www.universal-radio.com/catalog/cable/coaxperf.html) tables).
- For a single-hole install, the Taoglas MA1045 "Supercombo" carries active AM/FM/weather band on its own port (12 V on a separate wire), alongside GNSS and LTE ([Taoglas](https://taoglas.com/product/ma1045-supercombo-vehicle-mount-5in1-antenna)). Its datasheet (SPE-19-8-095-A, read 2026-11-21) adds ([PDF](https://www.taoglas.com/datasheets/MA1045.A.ALBTX.002.pdf)):
  - **AM/FM/weather band:** "in-built amplifier", 12 V separate wire; **no frequency ranges, gain, noise figure or output impedance published**. Connector listed as Fakra Code A in the feature list but BNC(F) in the mechanical table.
  - **Cellular (one port, Fakra D):** 698–803, 824–894, 880–960, 1710–1880, 1850–1990, 1920–2170, 2300–2500 MHz; 36–51 % efficiency. No 600 MHz (band 71), no 2500–2700 MHz (band 41), no 3.3–3.8 GHz, no second port for MIMO.
  - **GNSS (two ports, Fakra C):** GPS L1 1575.42 ± 1.023 MHz and BeiDou B1 1561.098 ± 2.046 MHz; +3 dBi at zenith; LNA and supply not stated. No GLONASS, no L5.
  - **SiriusXM:** 2320–2345 MHz. **Cables:** 300 mm RG-174 pigtails. IP67, −40 to 85 °C, 165 × 97 × 61 mm.
  - **Fit for Godwit:** covers AM/FM/weather band and GNSS, probably AIS (not listed); **not** 1090 MHz. The undocumented amplified AM/FM output must be bench-tested into the SDR (overload risk on FM with an 8-bit receiver; unknown AM output impedance).

## AM works on the current dongle today, within hard limits

### Direct-sampling mechanics on the SMArt v5

The datasheet schematic shows a diplexer at the single SMA input. Its "0.1 to 30 MHz Direct Sampling (Q Branch)" leg goes straight to the RTL2832, and its VHF/UHF leg goes to the tuner ([Nooelec datasheet](http://www.nooelec.com/datasheets/100700)). **No modification is needed, and MW and FM share one connector.**

There is also no MW preselection, no published MW sensitivity, and no anti-alias filtering beyond the unspecified diplexer. Treat its performance as V3-class direct sampling, about −109 dBm MDS at 1 MHz, until a bench measurement exists.

In librtlsdr, `rtlsdr_set_direct_sampling(dev, 2)` switches the chip into Q-branch direct sampling ([osmocom librtlsdr.c](https://raw.githubusercontent.com/osmocom/rtl-sdr/master/src/librtlsdr.c)):
- It puts the R820T into standby, disables Zero-IF and spectrum inversion, and selects the swapped ADC input.
- The "centre frequency" then goes to the RTL2832's 22-bit IF NCO, in about 6.87 Hz steps.
- Legal sample rates are 225.001–300 kHz and 900.001 kHz–3.2 MHz.
- Leaving the mode re-runs the full tuner initialisation.

The rtl-sdr-blog fork switches to Q-branch automatically below 24 MHz for R820T-type tuners, excluding the V4 and V4L ([rtl-sdr-blog librtlsdr.c](https://raw.githubusercontent.com/rtlsdrblog/rtl-sdr-blog/master/src/librtlsdr.c)). Osmocom does not. **Godwit should call the function explicitly** so that it works with either library.

A good capture choice, computed here, is **1,488,375 S/s centred at 1125 kHz**:
- The span is ±744 kHz, about 381–1869 kHz, which covers 530–1700 kHz with guard band.
- The centre sits between 10 kHz channels.
- Decimating by 32 gives exactly the 46,512 S/s that nrsc5's AM path expects.

A 2.4 MS/s capture also covers the band but needs a 51.6:1 resampler for nrsc5. Mode switches should cancel the async read, switch mode, call `rtlsdr_reset_buffer`, and restart. Retune latency has not been measured.

### NRSC-1-C de-emphasis and receiver bandwidth

NRSC-1-C defines the receiver de-emphasis as the exact complement of the "modified 75 µs" pre-emphasis: **one pole at 2122 Hz (τ = 75 µs) and one zero at 8700 Hz (τ ≈ 18.3 µs)**, reaching −10.00 dB at 10 kHz ([NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)). The standard sets two further conditions:
- The response is the receiver's *net* RF+IF+AF response, flat within +1.5/−3 dB (referenced to 400 Hz) out to the receiver's −3 dB point.
- ≤ −20 dB at 10 kHz is recommended.

A matched-z design at a 48 kHz audio rate reproduces the table well. The filter is H(z) = g(1 − q·z⁻¹)/(1 − p·z⁻¹), with:
- p = e^(−2π·2122/48000) = **0.75747**
- q = e^(−2π·8700/48000) = **0.32020**
- g = (1 − p)/(1 − q) = **0.35677**

I computed −2.535, −6.915 and −9.96 dB at 2, 5 and 10 kHz, against the table's −2.54, −6.92 and −10.00. A plain bilinear transform misses by 0.5 dB or more at 10 kHz.

Apply the full curve only at the widest audio setting. At ≤ 5 kHz the channel filter already removes the treble.

Bandwidth steps follow from the emission masks:
- **NRSC-2-C** allows 0 dBc to ±10.2 kHz and only −25 dBc out to ±20 kHz ([NRSC-2-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-2-c.pdf)), so first-adjacent sidebands overlap the wanted audio by design. That supports selectable 10/8/6/4.5/3 kHz audio and an NRSC-recommended high-Q 10 kHz carrier notch.
- **Hybrid IBOC AM stations** limit their own analog audio to 5, 8 or 9.4 kHz ([NRSC-1-C](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-1-c.pdf)), and their ±15 kHz digital sidebands sound like white noise on adjacent channels ([Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)). Sideband-selectable synchronous AM is therefore a core feature, not a luxury.

For the synchronous detector, **SDRangel remains the open reference**: a second-order PLL with ζ = 0.707 and a 10 ms lock window ([SDRangel amdemodsink.cpp](https://raw.githubusercontent.com/f4exb/sdrangel/master/plugins/channelrx/demodam/amdemodsink.cpp); [phaselockcomplex.cpp](https://raw.githubusercontent.com/f4exb/sdrangel/master/sdrbase/dsp/phaselockcomplex.cpp)). SDR++ still had no synchronous AM as of November 2026 ([SDR++](https://github.com/AlexandreRouma/SDRPlusPlus/tree/master/decoder_modules/radio/src/demodulators)).

The remaining parameters below are engineering estimates, not sourced figures:
- **PLL:** two-speed, about 200–400 Hz loop bandwidth to acquire and 20–50 Hz once locked. Coast through fades, and fall back to envelope detection after 100–300 ms without lock.
- **Carrier AGC:** 2–10 ms attack, 0.5–1 s hang, 0.5–2 s decay.
- **Noise blanker:** on wideband IQ, 6–12 dB above a 10–50 ms running mean, blanking 10–50 µs.
- **CPU:** a few percent of one Cortex-A76 core per channel.

## NRSC-5 IBOC fits in the FM capture, but the decoder stays outside the MIT code

### What reception needs

FM hybrid IBOC places OFDM sidebands at roughly ±130 to ±200 kHz from the carrier. The extended modes reach inward to about ±100 kHz. Digital power runs −20 to −10 dBc, and since FCC 24-105 the two sidebands may be asymmetric ([Wikipedia](https://en.wikipedia.org/wiki/HD_Radio); [Federal Register](https://www.govinfo.gov/content/pkg/FR-2024-10-21/html/2024-24105.htm)).

The open decoder is nrsc5. Its status as of November 2026:
- **Licence:** GPL-3.0-or-later ([LICENSE](https://raw.githubusercontent.com/theori-io/nrsc5/master/LICENSE)).
- **Activity:** v3.2.0 shipped on 2 June 2026, and PRs were active in September 2026 ([releases](https://github.com/theori-io/nrsc5/releases)).
- **Modes:** FM hybrid, AM hybrid MA1 and, experimentally, all-digital AM MA3. MA3 synchronisation bugs remain open ([issues](https://github.com/theori-io/nrsc5/issues?q=MA3)).
- **Interfaces:** a C API and pipe or stdin I/Q input at 744,188 S/s (FM) or 46,512 S/s (AM) in cs16/cf32, or 1,488,375 S/s in cu8 ([README](https://github.com/theori-io/nrsc5)).
- **CPU:** a NEON build was "adequate" on a single Pi 3 core back in 2017 ([issue #17](https://github.com/theori-io/nrsc5/issues/17)).

Godwit's FM capture runs at 1.14 MS/s with the station at +285 kHz. The upper sideband therefore reaches +483 kHz, in the decimation filter's roll-off. The cleaner path, computed here, is to **capture at 1,488,375 S/s, mix to 0 Hz and decimate by 2 to 744,187.5 S/s**. That one stream then feeds both the analog demodulator and nrsc5.

On the HF+ Discovery, 768 ksps is converted with an exact 3969/4096 resampler. Offset the station by about 100 kHz to avoid the zero-IF DC artefacts reviewers noted ([RadioDX.pl](https://radiodx.pl/?p=8726)). The sidebands then fall at −98 to +298 kHz, inside the ±330 kHz clean span.

**A single shared capture also makes blend tractable.** The analog and digital audio then share one sample clock. A TEF6686-for-analog, RTL-for-digital hybrid has already been demonstrated on an FM-DX Webserver ([rtl-sdr.com](https://www.rtl-sdr.com/adding-hd-radio-support-to-fm-dx-webservers-with-an-rtl-sdr/)), but it adds a second audio clock domain to the blend.

Blend itself needs three things:
- **A delay line on the analog audio.** Transmitters delay analog by a diversity delay of more than 2 s, preferably 3–5 s ([US9768948](https://patents.google.com/patent/US9768948)), and the offset varies by station.
- **Per-station alignment.** Measure the offset by envelope cross-correlation to within a few samples ([USRE48966](https://patents.google.com/patent/USRE48966)), then match levels.
- **Hysteresis.** Commercial receivers require a strong signal to acquire digital and tolerate a large drop before falling back ([element14](https://community.element14.com/technologies/wireless/b/blog/posts/the-best-hd-radio-receiver)).

Supplemental program services have no analog fallback and must mute or show "signal lost" on dropout.

### Value and legal posture

Radio World counted **2,114 active IBOC FM stations in August 2025, about 18% of US full-power FM** ([Radio World](https://www.radioworld.com/?p=131847)), concentrated in larger markets. AM IBOC has "stalled" ([Radio World](https://www.radioworld.com/tech-and-gear/am-hd-radio-has-stalled-now-what)), with only a handful of all-digital stations ever. FM support therefore matters near metros, AM support is marginal, and MA3 can wait.

On the legal side, Xperi's 2023 NRSC disclosure lists a receiver patent portfolio and offers licences under NRSC policy ([Xperi 2023](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-5-e/Xperi2023.pdf)). The audio codec is a trade secret that was reverse engineered in 2017 ([Theori](https://theori.io/blog/receiving-nrsc-5)). No takedown or lawsuit against nrsc5 was found in nine years of public availability, but nothing licenses it either. The system's consumer brand name is an Xperi trademark and must not be used without a licence ([Xperi trademark guidelines](https://hdradio.com/wp-content/uploads/2025/08/REF_PR_2731-HD-Radio-Trademark-Usage-Guidelines-OEM-Version-4.0.0.5.pdf)).

Godwit should handle this as follows:
- Ship NRSC-5 as an **opt-in, separately built GPL-3.0 helper**, `godwit-nrsc5d`, that links libnrsc5 and talks to the MIT code over a socket. This is the same process boundary used today for librtlsdr.
- Label channels "Digital" and "SPS 2" or with SIS short names.
- Get legal review before any commercial unit ships.

## Two receivers form the recommended architecture

The three architectures compare as follows:
- **SDR-only on one dongle** is cheapest, but it keeps today's one-service-at-a-time limit and the weakest AM.
- **TEF6686 plus RTL-SDR** gives the best analog in motion. Its costs are a blob, an MCU, a GPL firmware boundary, I2S integration and a second clock domain for blend. The RTL-SDR still has to time-share nrsc5 with ADS-B unless a third receiver is added.
- **HF+ Discovery plus the existing RTL-SDR** keeps everything open source and in one DSP framework. It gives the best SDR front end for AM, FM and NRSC-5 on one clock, and it frees the RTL-SDR for ADS-B, AIS and NOAA full time. Godwit then has to write the car-radio DSP itself, which Phases 2–3 already plan.

That last option is the recommendation, with a TEF backend as an optional experiment.

Power must be checked when a second device is added:
- **Pi 5 USB budget.** The Pi 5 limits USB to 600 mA unless a 5 A supply is negotiated or `usb_max_current_enable=1` is set, which raises the limit to 1.6 A ([Raspberry Pi RP-009856](https://pip-assets.raspberrypi.com/categories/685-app-notes-guides-whitepapers/documents/RP-009856-WP-1-USB%20Power%20delivery%20on%20Raspberry%20Pi%205.pdf)).
- **Device draw.** The SMArt v5 draws 270–330 mA ([Nooelec](http://www.nooelec.com/datasheets/100700)). The original HF+ peaked at 140 mA ([RigPix](https://rigpix.com/miscrx/airspy_hfplus.htm)); no Discovery figure is published.
- **Clean supply.** Both radios plus any bias-tee load need the 1.6 A setting, and the HF+ needs a filtered 5 V supply.

| Tier | Receivers | Filters and antennas | Approx. cost |
|---|---|---|---|
| Good | Existing SMArt v5, time-shared; AM by direct sampling | DIY MW BPF; JFET buffer or mini-whip with a DC injector; existing FM mast; 46 cm 162 MHz whip; small 1090 MHz quarter-wave; FM band-stop on the VHF path ($16.95) | ~$60–120 (estimate; DIY parts unsourced) |
| Better | **HF+ Discovery** ($169–199) for AM/FM/NRSC-5; SMArt v5 dedicated to ADS-B, AIS and NOAA | Roof active AM antenna or buffered whip; FM vertical; marine 162 MHz whip; 5.5 dBi 1090 collinear (~$45) on LMR-240; filtered 5 V and a metal enclosure | ~$300–450 (estimate) |
| Best | HF+ Discovery for broadcast; a second RTL-SDR (V4/V4L, ~$35) on 1090 MHz with a SAW-filtered masthead LNA; SMArt v5 on AIS/NOAA; optional TEF6686 module ($10–50) as an experimental analog backend | Taoglas MA1045-class active AM/FM/weather band plus a second FM antenna; dedicated 162 MHz whip; 1090 collinear with LNA; GPIO-switched 12 V phantom injector | ~$500–800 (estimate; Taoglas and LNA prices unretrieved) |

Users who accept SDRplay's EULA can substitute an RSPduo in the Best tier for two-antenna FM diversity. The project should support that through a builder-fetched recipe, never in the default image.

## Work plan: extend Phase 6 and add Phases 7–8

The order follows dependencies:
- **First, as before:** Phase 0 recording (0.1) and the librtlsdr pin (4.4).
- **Then AM bring-up** on the current dongle, in parallel with the hardware docs.
- **Then multi-device support**, which unlocks every second receiver.
- **Then the HF+ backend.**
- **Last, NRSC-5.** Its blend depends on the native audio path (4.3) and the quality indicators (2.1).

Each table follows that order.

### Phase 6: AM (revised; 6.0 correction stands)

| # | What | Why | Concrete parameters | Hardware | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 6.1 | Bench-measure AM on the SMArt v5 | No MW sensitivity or alias data exist for this dongle | Q-branch at 1,488,375 S/s, centre 1125 kHz; MDS at 600/1000/1500 kHz; alias check at 27.1 and 88.0 MHz; strong-local overload, with and without the MW BPF from 7.2 | SMArt v5, signal generator or known locals, filter | Published table in the issue | S–M | Issue |
| 6.2 | Direct-sampling mode switch in godwit-radio | AM needs Q-branch; FM must restore the tuner | Explicit `rtlsdr_set_direct_sampling(dev,2)` / `(dev,0)`; cancel async read, `rtlsdr_reset_buffer`, restart; band-specific sample rate; depends on 4.4 | SMArt v5 | Switch AM↔FM 100× with no stale audio; latency logged | M | Issue |
| 6.3 | AM receiver core: channel filter, envelope detector, carrier AGC | Minimum usable AM | 46.5–48 kHz channel rate; audio widths 10/8/6/4.5/3 kHz; AGC 2–10 ms attack, 0.5–1 s hang, 0.5–2 s decay; 10 kHz raster 530–1700 kHz | none (tests) | New `amreceiver_test`: tone at 30% modulation, AGC step response | M | Issue |
| 6.4 | NRSC-1-C de-emphasis | Standard receiver response | Matched-z at 48 kHz: p = 0.75747, q = 0.32020, g = 0.35677; full curve only at ≥ 9 kHz width | none | Within ±0.1 dB of NRSC-1-C Table 2 at 50 Hz–10 kHz | S | PR (after 6.3) |
| 6.5 | Synchronous AM with sideband select | Fading skywave; IBOC and adjacent-channel hiss | PLL ζ = 0.707, ~300 Hz acquire → 20–50 Hz tracking, 10 ms lock test, coast on fade, envelope fallback after 100–300 ms; DSB/USB/LSB plus auto-select by lower adjacent energy | none, then night recordings | Synthetic selective-fade test; recorded skywave A/B | M | Issue |
| 6.6 | Heterodyne notch and AM noise blanker | First-adjacent 10 kHz whistles; ignition and LED impulses | Tracking IIR notch, Q 30–100, at 10 kHz (9 kHz region option); blanker on wideband IQ at 6–12 dB over a 10–50 ms mean, 10–50 µs blanking | none | Recorded whistle and impulse cases | M | Issue |
| 6.7 | HMI band switch, per-band presets, voice 530–1700 | AM is hard-coded off (was 6.3) | Restore the v1 band toggle; separate preset banks; single AM ceiling of 1700 kHz | none | Simulator and head-unit parity | M | Issue |
| 6.8 | Optional MW channeliser and live station list | Instant tuning from one capture | Polyphase bank over the 1.49 MHz capture, 10 kHz bins, level per channel; ~10–25% of one A76 core (estimate) | none | CPU benchmark on Pi 5 | L | Issue (defer) |

### Phase 7: RF hardware, antennas and a second receiver

| # | What | Why | Concrete parameters | Hardware | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 7.1 | Antenna and front-end guide in the docs | The FM antenna guidance is one phrase; the 200 MHz LPF question will recur | Per-service filter table above; 50 Ω vs high-Z AM loss; 12 V phantom; coax losses; RV noise mitigation; why a 200 MHz LPF does not help | none | Maintainer review | S | PR |
| 7.2 | Open-hardware MW band-pass filter | No commercial MW BPF exists; required for direct sampling | 520–1710 kHz, IL ≤ 2 dB; ≥ 60 dB at 26–30 MHz; ≥ 70 dB at 54–230 MHz; 5th–7th-order LC in a shielded box, SMA | parts, VNA or NanoVNA | Sweep plot; 6.1 re-run with filter | M | Issue |
| 7.3 | AM active antenna or JFET buffer plus DC injector | Recovers ~30 dB lost into 50 Ω | Buffer > 100 kΩ, < 10 pF input, 50 Ω out; injector 5–12 V with polyfuse, GPIO-switched; common-mode chokes at base and entry | DIY parts or a mini-whip | Same-station level vs passive whip, ≥ 20 dB gain | M | Issue |
| 7.4 | Multi-device support in all SDR helpers | Helpers always open device 0; a second receiver is unusable without this | Select by USB serial per service (`rtlsdr_get_index_by_serial`); udev IDs for added devices; config mapping service → device | 2 dongles | Radio and ADS-B run at the same time | M | Issue |
| 7.5 | USB power and supply hygiene | Pi 5 limits USB to 600 mA by default; HF+ spurs from noisy 5 V | Document `usb_max_current_enable=1` / `PSU_MAX_CURRENT=5000`; filtered 5 V; metal enclosure; measure draw per device | USB meter | Measured current table; no FM spurs on HF+ | S | PR (docs) + bench |
| 7.6 | Airspy HF+ Discovery backend | Best open SDR for AM/FM/NRSC-5 | Custom Yocto recipe for libairspyhf (confirm per-file licence first); 768 ksps; FM offset ~100 kHz; 3969/4096 resampler to 744,187.5 S/s; AM decimation to 46.5 kHz; manual gain/attenuator steps | HF+ Discovery | A/B recordings against the SMArt v5 on the same antenna | L | Issue |
| 7.7 | 162 MHz and 1090 MHz paths | Service-specific filtering and antennas | FM band-stop or 155–164 MHz BPF on AIS/NOAA; SAW-filtered 1090 LNA at the antenna; LMR-240 | filters, LNA, antennas | Message rate and NOAA SNR before/after | S–M | Issue |
| 7.8 | Optional TEF6686 tuner backend | Best analog in motion, for users who want it | MCU running existing XDR-protocol firmware as a separate GPL work; Godwit implements a protocol client only; analog or I2S audio | TEF module + ESP32 | Drive-test A/B vs HF+ | L | Issue (experimental) |

### Phase 8: NRSC-5 IBOC digital radio

| # | What | Why | Concrete parameters | Hardware | Verify | Size | Route |
|---|---|---|---|---|---|---|---|
| 8.1 | Agree packaging, naming and legal posture | Patents, trade-secret codec, trademark | Opt-in package built from upstream nrsc5 at a pinned tag; no trademarked names or channel labels; docs note no patent licence is provided | none | Maintainer decision | S | Issue (first) |
| 8.2 | FM capture at 1,488,375 S/s with decimation by 2 | Gives nrsc5 its exact rate with guard band | Keep the +285 kHz offset; mix to 0 Hz; halfband decimate to 744,187.5 S/s; analog chain retuned to the new rate; depends on 0.1, 0.2 | current dongle | All fmreceiver tests pass; recordings unchanged in quality | M | Issue |
| 8.3 | `godwit-nrsc5d` GPL helper | Keeps the MIT code separate | Links libnrsc5 v3.2.0+, `-DUSE_NEON=ON`; cs16/cf32 I/Q over a Unix socket; outputs PCM plus JSON events (sync, MER/BER, CRC, SIS, PSD, LOT paths) | none | Decodes recorded local IBOC I/Q; CPU on Pi 5 below one core | M–L | Issue |
| 8.4 | IBOC presence detector | Run the decoder only when useful | Sideband power at ±129–198 kHz against adjacent noise; per-station cache flag | none | No false starts on analog-only recordings | S–M | Issue |
| 8.5 | Analog/digital blend engine | Seamless audio | ~6 s analog delay buffer; envelope cross-correlation offset (expect 3–5 s), refined and cached per station; RMS level match; switch to digital after N s clean CRC, fall back on CRC rise or sync loss; 0.5–1 s crossfades with hysteresis; depends on 2.1 and 4.3 | none | Drive test vs a commercial receiver; no echo at blend | L | Issue |
| 8.6 | HMI for SPS and metadata | Usable multicast | SPS as "101.1 · 2" with SIS short names; PSD title/artist; LOT art written to a sandbox with whitelisted extensions and size limits; "Digital" indicator from MER | none | Simulator plus real station | M | Issue |
| 8.7 | AM IBOC MA1 (experimental) | Few stations; needs a clean MW path | 46,512 S/s from 6.2 (÷32) or HF+ (7.6); MA3 deferred | HF+ preferred | Lock on a strong local hybrid station | M | Issue (defer) |

## Conclusion

The question changed during this research. It started as "which SDR is best". It ended as "which receivers can an MIT image legally and reliably drive at the same time". That framing rules out SDRplay for the default image and makes the TEF6686 an optional backend. It also shows that the cheapest large improvement is splitting services across two devices. Better ADC bits matter less.

Two findings will surprise people. First, the AM problem sits mostly in front of the dongle, not inside it: the 30 dB lost by loading a whip with 50 Ω and the aliases from CB, 10 m and 88 MHz outweigh the 8-bit ADC. A filter and a buffer worth about $30 make direct-sampling AM worth shipping. Second, NRSC-5 argues for SDR-only broadcast reception. Feeding analog and digital from one I/Q stream on one clock turns blend into a delay-and-correlate problem instead of a two-clock resampling problem.

The open measurements decide the remaining choices:
- SMArt v5 MW sensitivity;
- HF+ Discovery spur behaviour on a vehicle 5 V rail;
- TEF6686 against SDRs, which nobody has measured.

These should be measured first, through items 6.1, 7.5 and 7.8.
