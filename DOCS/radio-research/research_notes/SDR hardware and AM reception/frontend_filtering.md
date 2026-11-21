# RF Front-End Filtering and Antenna Arrangement for an RTL-SDR Head Unit (AM / FM / NOAA / AIS / ADS-B)

Scope: one Nooelec NESDR SMArt v5 (RTL2832U + R820T2, 28.8 MHz clock) shared one-at-a-time across AM (direct sampling, Q-branch), FM, NOAA WX, AIS, and ADS-B; evaluation of the user's proposed ~200 MHz hardware low-pass, plus filter/antenna architectures. Research date: 2026-11-21. Tool budget limited this to about 17 searches/fetches, so several items rely on datasheet extracts and my own arithmetic (marked as inference).

---

## Q1. What would a ~200 MHz low-pass fix (and not fix) on an R820T/R820T2?

### Takeaway
A 200 MHz LPF removes UHF TV (470–700 MHz), cellular/LTE (600–2700 MHz), and 929–932 MHz paging, but it does **not** remove the blockers that matter most for FM/NOAA/AIS: in-band FM broadcast, 152–159 MHz paging/land-mobile, and VHF-high TV (174–216 MHz). It also cuts off ADS-B at 1090 MHz. HB9AJG's measurements show the R820T's own tracking filter already makes it fairly tolerant of far-off signals, roughly -20 dBm at ±50 MHz, so a UHF cut buys little for VHF reception. A 200 MHz LPF is also **not adequate as an anti-alias filter for direct-sampling AM** (see Q2).

### Cited Findings
- **R820T measured tracking filter (HB9AJG, Aug 2013).** The R820's digitally tuned RF filter after the preamp is "about +/-3MHz wide, but steeper than the filter of the E4000". Up to about ±1 MHz, a signal of about -62 to -68 dBm causes overload. At about ±10 MHz, 1 dB compression becomes the limiting effect. — [HB9AJG PDF (mirror)](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf); summary at [rtl-sdr.com](https://www.rtl-sdr.com/measurements-on-rtl-sdr-e4000-and-r820t-dvb-t-dongles-image-rejection-internal-signals-sensitivity-overload-1db-compression-intermodulation/)
- **R820T two-tone IMD (HB9AJG).** With two -65 dBm tones at 145.1/145.2 MHz (in the passband), IM products appear and the receiver overloads. Two -37 dBm tones at 150.0/150.1 MHz (about 5 MHz off, outside the filter) produce "a few IM products". Two -20 dBm tones at 195.0/195.1 MHz (+50 MHz off) sit at the overload/1 dB compression limit: the noise floor rises about 3 dB and a few new signals appear. HB9AJG also notes "for frequencies far off the center frequency the R820 is less prone to IM than the E4000", and that IM shows up well before the overload limit when the strong signal sits on the filter roll-off. — [HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)
- **Gain behaviour (HB9AJG).** For the R820, the overload limit tracks gain almost linearly: cutting gain by 11/20/30 dB improved the overload limit by 12/20/30 dB (at 145 MHz). Tuner AGC "is to be avoided: it degrades the overload limit by about 10dBs". RTL AGC changed things by only 1–2 dB. — [HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)
- **HB9AJG recommendations.** Keep signals inside the ±3 MHz filter window to no more than about 60 dB above the noise floor. In broadcast bands with many strong signals the voltages add ("add 6dB per equally strong signal"), so gain must be cut further. "If very strong signals are present at the antenna input (>-40dBm), they should be attenuated by bandstop or notch filters." — [HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)
- **R820T sensitivity (HB9AJG MDS, 500 Hz BW).** -127 dBm at 24 MHz; -134 dBm at 52/110/145 MHz; -135 dBm at 435 MHz; -136 at 700 MHz; -137 dBm at 1000 MHz. Measured range 24–1766 MHz. — [HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)
- **Image rejection.** HB9AJG found the R820 "does not have the same image rejection problem" as the zero-IF E4000. The R820T preliminary datasheet lists image rejection of 65 dBc and NF of 3.5 dB. These are R820T (not T2) preliminary figures. — [HB9AJG PDF](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf); [R820T datasheet (ab9il mirror)](https://www.ab9il.net/software-defined-radio/r820T_datasheet.pdf)
- **R820T2 architecture.** The R820T2 integrates an LNA, mixer, fractional PLL, VGA, voltage regulator, and tracking filter. — [datasheetcafe R820T2 summary](https://datasheetcafe.com/r820t2-datasheet-digital-tuner/)
- **Vendor evidence on out-of-band desense (RTL-SDR Blog V4 datasheet).** Test: an "interference" tone at 95 MHz, ramped up until a second tone dropped 3 dB. The V4's triplexer plus notch improved isolation "within the same triplexer band ... by about 8 dB thanks to the notch, and then out of band isolation is improved by 28 - 43 dB". In other words, pre-selection measurably reduces desense from strong FM on the other bands. — [RTL-SDR Blog V4 Datasheet V1.0](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- **Commercial ~200 MHz LPF (Mini-Circuits BLP-200+).** Passband DC–190 MHz, nominal fco 210 MHz. Stopband >20 dB from 290–390 MHz and >40 dB from 390–800 MHz. Note the >20 dB attenuation does not begin until about 290 MHz, so VHF-high TV (174–216 MHz) passes unattenuated. — [BLP-200+ datasheet (Mouser)](https://www.mouser.com/datasheet/3/3705/1/BLP-200+.pdf); variants [SBLP-200+](https://www.minicircuits.com/pdfs/SBLP-200+.pdf), [BBLP-200](https://www.minicircuits.com/pdfs/BBLP-200.pdf)

### Inferences
- **Blocker-by-blocker assessment for a 200 MHz LPF:**
  - **FM broadcast 88–108 MHz:** not touched. This is in-band for FM and only 54–74 MHz from NOAA/AIS, and it is the dominant real-world blocker.
  - **Paging/land-mobile 152–159 MHz:** not touched. These are only 3–10 MHz from NOAA/AIS, on the R820 tracking filter's skirt, which is where HB9AJG saw IMD earliest.
  - **VHF-high TV/DTV 174–216 MHz:** mostly not touched. BLP-200+ has under 20 dB attenuation below 290 MHz.
  - **UHF TV 470–700 MHz, LTE 600–2700 MHz, 929–932 MHz paging:** attenuated more than 40 dB from 390 MHz upward. Per HB9AJG, though, the R820 already tolerates about -20 dBm two-tone at ±50 MHz. A UHF signal reaching that level at a car antenna is rare unless the vehicle is parked next to a cell or TV site. So the measurable benefit for VHF reception is probably small, and mostly comes from lowering total power at the LNA and ADC.
- **ADS-B loss is a hard blocker for a fixed in-line 200 MHz LPF.** ADS-B at 1090 MHz would sit far down in the stopband. The LPF would therefore have to be switched or diplexed (see Q4), not hard-wired.
- **Better VHF alternative to a 200 MHz LPF:**
  - For NOAA/AIS, use a 156–163 MHz band-pass (marine-VHF BPF). It rejects FM, TV, and UHF all together.
  - For FM, rely on gain management and an optional tunable notch (see Q3).
  - A plain low-pass doesn't solve FM-into-NOAA desense. An FM band-stop (or BPF) does that job: the V4 data shows the notch/triplexer improves isolation by 28–43 dB out-of-band.
- **Gain control is the first line of defence.** On the R820, reducing gain improves the overload limit dB-for-dB, so the head-unit software should use a fixed manual gain per service rather than tuner AGC.

### Gaps
- No measured blocking or IIP3 figures specifically for the R820T2 (vs R820T) or the NESDR SMArt v5 were found. HB9AJG measured one R820T dongle with ±1–2 dB uncertainty.
- No SM5BSZ R820T numbers were retrieved within budget.
- R820T2 final-datasheet image rejection is unconfirmed (65 dBc is the R820T preliminary figure).
- **Not verified by fetch:** the R820 low-IF of about 3.57 MHz (defined as R82XX_IF_FREQ in the osmocom librtlsdr source, https://github.com/osmocom/rtl-sdr). If correct, the image sits about 7.14 MHz from the wanted signal, inside the ±10 MHz region where the tracking filter is only partially effective.

---

## Q2. Direct-sampling AM: aliasing and required anti-alias filtering

### Takeaway
At 28.8 MS/s with no front-end filtering, the whole spectrum folds into 0–14.4 MHz. The FM band 88–108 MHz lands on 1.6–14.4 MHz, so it sits in shortwave rather than directly on MW. However, 87.9–88.1 MHz folds onto 1.5–1.7 MHz (top of the AM band). CB/10 m (27.1–28.3 MHz), VHF-low TV channel 6 (84.7–86.9 MHz), and VHF-high TV channel 11 (199.9–203.3 MHz) also fold directly onto 530–1700 kHz. On top of that, the total wideband power loads the 8-bit ADC. A dedicated MW filter is required: a 520–1710 kHz band-pass, or a 2–3 MHz LPF plus a high-pass around 400–500 kHz. A 200 MHz LPF is not an anti-alias filter here.

### Cited Findings
- Direct sampling bypasses the tuner's filtering; "images from higher frequencies such as broadcast FM can be problematic", so an external low-pass is recommended. — [rtl-sdr.com, LPF for direct sampling](https://www.rtl-sdr.com/low-pass-filter-for-rtl-sdr-direct-sampling-mode/); [pa3fwm, How RTL-SDR dongles work](https://www.pa3fwm.nl/technotes/tn20.html)
- Suggested approach: a 0–14.4 MHz LPF, a 14.4–28.8 MHz HPF, or "simply filter your band of interest". One commenter notes the 8-bit ADC gives about 50 dB dynamic range and suggests a 1.6–14.4 MHz band-pass plus an FM trap. One user reported a bought LPF had "no effect" (anecdotal; the problem was likely in-band MW overload, not aliasing). — [rtl-sdr.com LPF article & comments](https://www.rtl-sdr.com/low-pass-filter-for-rtl-sdr-direct-sampling-mode/)
- The RTL-SDR Blog V3 guide warns that direct-sampling HF "makes use of direct sampling and so aliasing will occur". Many users "experience severe overload from strong broadcast AM stations which can cause problems with reception above 2 MHz". — [RTL-SDR Blog V3 User Guide](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/comment-page-1/)
- **V4 avoids direct sampling.** It uses an upconverter with a 28.8 MHz LO: "no more Nyquist folding of signals around 14.4 MHz, improved sensitivity, and adjustable gain on HF". "Very strong reception may still require front end attenuation/filtering". Direct-sampling mode on a V4 "will yield no results". — [V4 Datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf); [rtl-sdr.com/v4](https://www.rtl-sdr.com/v4/)
- **Commercial filters (specs):**
  - **Mini-Circuits SLP-15+ (LPF).** DC–15 MHz passband, fco 17 MHz. Stopband >20 dB at 23–32 MHz and >40 dB at 32–200 MHz. — [SLP-15+ (Mini-Circuits)](https://www.minicircuits.com/pages/s-params/SLP-15+_VIEW.pdf); [DigiKey listing](https://www.digikey.ca/en/products/detail/mini-circuits/SLP-15/21727883)
  - **Mini-Circuits BLP-10.7+ (LPF).** DC–11 MHz passband, fco 14 MHz. Stopband >20 dB at 19–24 MHz and >40 dB at 24–200 MHz. — [BLP-10.7 datasheet](https://collab.phys.unsw.edu.au:4431/bin/../pub/QED/RFElectronicComponents/BLP-10.7.pdf)
  - **Mini-Circuits SBP-10.7+.** A band-pass centred at 10.7 MHz (IL 1.5 dB max), not useful for MW. — [SBP-10.7+](https://www.minicircuits.com/pdfs/SBP-10.7+.pdf)
  - **Nooelec Flamingo+ AM.** A broadcast-AM **band-stop** blocking 300–1900 kHz; it is the opposite of what an AM receiver needs. The original Flamingo AM spec is -3 dB at 350/1900 kHz with >40 dB typical. — [Nooelec Flamingo+ AM](https://www.nooelec.com/store/flamingo-plus-am.html); [Flamingo AM](https://www.nooelec.com/store/flamingo-am.html)
  - **RTL-SDR Blog "Broadcast AM Block" HPF.** Also blocks MW (for HF listening). — [rtl-sdr.com](https://www.rtl-sdr.com/rtl-sdr-com-broadcast-block-high-pass-filter-now-sale/)
  - **Measured Flamingo AM results conflict.** A user measured only 23 dB at 1160 kHz and 8 dB at 1480 kHz, versus N1FD's roughly 70 dB on a Flamingo+ AM. — [SDR-Radio forum](https://forum.sdr-radio.com/viewtopic.php?p=11788); [N1FD BCI notch](https://www.n1fd.org/2024/11/26/notch-filter/)
- **The broadcast FM band-stop filters also serve as an FM trap ahead of direct sampling:**
  - **RTL-SDR Blog FM band-stop.** 7th-order Chebyshev, >50 dB at 88–108 MHz, IL <0.5 dB from 0–1 GHz, USD $16.95. — [RTL-SDR Blog product page](https://www.rtl-sdr.com/product/rtl-sdr-blog-broadcast-fm-band-stop-filter/); [launch post](https://www.rtl-sdr.com/rtl-sdr-com-broadcast-fm-band-stop-filter-88-108-mhz-reject-now-for-sale/)
  - **Nooelec Flamingo+ FM.** More than 60 dB typical average and more than 70 dB typical maximum stopband attenuation. -3 dB points at 75/120 MHz, -10 dB points at 80/116 MHz. IL <0.25 dB out of band, max +18 dBm, DC pass-through at least 250 mA. — [Nooelec Flamingo+ FM](https://www.nooelec.com/store/flamingo-plus-fm.html)
  - **Nooelec Flamingo FM v1 (datasheet).** -30 dB typical (range -10 to -51 dB), centre 98 MHz, 3 dB BW 71 MHz. The rtl-sdr.com review claims about 50 dB, which conflicts with this datasheet figure. — [Nooelec datasheet 100780](https://www.nooelec.com/datasheets/100780)

### Alias computation (my arithmetic; fs = 28.8 MS/s, Nyquist 14.4 MHz)
An input f appears at |f − k·28.8| folded into 0–14.4 MHz.

| Input band | Aliases to |
|---|---|
| FM 88.0–100.8 MHz (k=3, 86.4) | 1.6–14.4 MHz (ascending; 88.1→1.7, 98.1→11.7) |
| FM 100.8–108 MHz (115.2 − f) | 14.4 → 7.2 MHz (descending; 107.9→7.3) |
| **87.9–88.1 MHz** | **1.5–1.7 MHz: top of AM band (1500–1700 kHz)** |
| 84.7–85.9 MHz (TV ch 6, 82–88 MHz) | 1.7–0.53 MHz (inverted): directly onto MW |
| 26.96–27.41 MHz CB | 1.84–1.39 MHz (inverted) onto upper MW |
| 29.3–30.5 MHz (10 m / VHF-low) | 0.53–1.7 MHz |
| 55.9–59.3 MHz (TV ch 2) | onto MW |
| 113.5–116.9 MHz (aero VOR/ILS) | onto MW |
| 142.3–145.7 MHz (2 m ham/gov) | onto MW |
| 171.1–174.5 MHz (VHF LMR, TV ch 7 edge) | onto MW |
| **199.9–203.3 MHz (DTV ch 11, 198–204 MHz)** | onto MW; sits right at a 200 MHz LPF's corner, where it is barely attenuated |
| NOAA 162.4–162.55 MHz | 10.4–10.25 MHz |
| AIS 161.975/162.025 MHz | 10.83/10.78 MHz |
| Any SW 14.4–28.8 MHz | 28.8 − f (mirror) |

In general, every multiple of 28.8 MHz (28.8, 57.6, 86.4, 115.2, 144, 172.8, 201.6, 230.4 MHz … into UHF) ±0.53–1.7 MHz lands on the AM band.

### Inferences
- **Required AM filter: a MW band-pass, 520–1710 kHz (or 500–1800 kHz).** It must give at least 60 dB attenuation by about 26 MHz (CB), at least 70 dB at 84–120 MHz, and at least 60 dB from 140 MHz up to at least 1 GHz. The R820T2/RTL ADC input has no other protection in direct-sampling mode.
  - **Simpler alternative:** a 5th–7th-order elliptic or Chebyshev LPF with fc about 2–3 MHz and at least 60 dB by 10 MHz, cascaded with an HPF at about 400–500 kHz. The HPF removes LW/NDB and the large ignition/alternator noise below 500 kHz.
  - **Insertion loss:** at most 1–2 dB is acceptable, because MW is externally noise-limited.
  - **Construction:** in-band ripple matters little for AM. Use high-Q inductors, or a crystal-free LC design.
  - **Deep stopband at VHF:** this requires attention to layout and shielding, because LC filters "leak" at VHF/UHF through parasitic coupling.
- **Mini-Circuits SLP-15+/BLP-10.7+ are not sufficient on their own.**
  - They pass 1.6–11 MHz, which still contains the FM-band aliases (1.6–14.4 MHz) and SW broadcasters. Those consume 8-bit ADC headroom even though they don't land on MW.
  - Their stopband is only specified to 200 MHz. They do help against VHF aliasing.
  - A ~2 MHz LPF is strongly preferable.
- **Why aliasing matters less than overload for AM.** In a car the dominant problem is likely to be ADC dynamic range, from strong local AM plus the total wideband energy, rather than discrete aliases. HB9AJG's "about 60 dB above noise" rule and the 8-bit ADC's roughly 50 dB dynamic range both point to keeping as much non-MW energy as possible out of the ADC.
- **The V4 upconverter path is architecturally better for AM.** It has an HF triplexer leg (0–28 MHz), no Nyquist folding, and adjustable gain. A MW BPF in front is still advisable near strong AM sites.

### Gaps
- No measured dB figures of FM aliasing into MW on an unfiltered NESDR in direct-sampling mode were found.
- The RTL2832U ADC analog input bandwidth, which determines how far up in frequency aliasing persists, was not found.
- No Nooelec MW band-pass product was found. Nooelec appears to sell only AM band-stop (Flamingo AM) filters, not a MW BPF.
- No specs were retrieved for a commercial off-the-shelf 530–1700 kHz BPF.

---

## Q3. FM broadcast: band-pass vs notch vs gain control

### Takeaway
For FM reception, the overload sources are in-band strong locals, and those pass through an 88–108 MHz BPF unchanged. A BPF mainly helps against TV, paging, and cellular. Since the R820 tolerates far-off signals well, a BPF gives limited improvement in most locations. The effective tools are, in order:
- correct manual gain (dB-for-dB on the R820),
- an optional tunable notch on a specific overpowering local, and
- not using tuner AGC.

### Cited Findings
- **Overload width.** Overload is driven by signals inside the ±3 MHz tracking-filter window (about -62 to -68 dBm per signal). Multiple strong broadcast signals add, so "in frequency bands with many strong signals, e.g. broadcast bands, the gain must be reduced even further." — [HB9AJG](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)
- **Gain reduction is linear on the R820.** Gain cuts of 11/20/30 dB improve the overload limit by 12/20/30 dB. Tuner AGC costs about 10 dB of overload headroom. — [HB9AJG](https://wiki.kewl.org/_media/downloads:hb9ajg.pdf)
- **V4 notch and the FM band.** The V4 notch for broadcast AM, broadcast FM, and DAB "only attenuate[s] by a few dB, but may still help". It works through an open-drain pin on the R828D. — [V4 datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf)
- **Filter placement relative to an LNA.** If FM is strong enough to overload the LNA, put the filter in front of it. Otherwise put it after, to save insertion loss. — [RTL-SDR Blog V3 guide comments](https://www.rtl-sdr.com/rtl-sdr-blog-v-3-dongles-user-guide/comment-page-2/)

### Inferences
- **What an FM BPF (about 87–109 MHz, 5th order) would buy:**
  - It knocks down TV ch 6 (82–88 MHz) and aero 108–118 MHz next to the band edges.
  - It removes VHF-high and UHF energy.
  - For a vehicle it is cheap insurance but not transformative.
  - **Do not cascade it into the NOAA/AIS path.**
- **Tunable notch for a specific strong local.** A notch only helps when the strong local is more than about ±3 MHz from the wanted station (outside the R820 tracking-filter window), or when it drives the antenna amplifier or LNA into compression.
  - A narrow notch (1–2 MHz wide, varactor-tuned) on an adjacent strong local would degrade nearby wanted stations. Since a car moves between markets, a fixed or tunable notch is a poor fit for a head unit.
  - **Recommendation:** implement software-driven gain stepping. Detect overload with the RTL2832U's ADC peak level and spurious-signal heuristics, then back off gain.
- **Broadcast-engineer angle.** Vehicle FM antennas are often amplified (see Q5). The amplifier, not the R820, may be the first stage to overload near transmitter sites. A filter only helps ahead of that amplifier, and it is usually built into the mast.

### Gaps
- No published measurements were found comparing FM reception with vs without an 88–108 BPF on an R820T2.
- No commercial tunable FM notch (single-station trap) product specs were retrieved. Broadcast-engineering tunable FM traps exist (e.g., single-station cavity/stub traps), but none were sourced here.

---

## Q4. Architectures for a shared receiver (diplexer/triplexer, switched filter banks, V4 built-ins, multiple receivers, bias-tee pass-through)

### Takeaway
Best value is either:
- **(a) RTL-SDR Blog V4.** It has an internal triplexer: HF 0–28 MHz / VHF 28–250 MHz / UHF 250–1766 MHz, with a switchable notch, a 28.8 MHz-LO upconverter, and a 4.5 V/180 mA bias tee.
- **(b) The current NESDR plus an external GPIO-controlled switched filter bank** (SP4T in, SP4T out) with four paths:
  1. MW BPF (direct sampling)
  2. FM path (BPF or straight-through)
  3. 156–163 MHz BPF (NOAA/AIS)
  4. 1090 MHz SAW/LNA (ADS-B)

For a vehicle that wants simultaneous ADS-B plus audio, one receiver per service is the cleanest option.

### Cited Findings
- **V4 front end.** R828D tuner with three inputs. The SMA input is triplexed into HF/VHF/UHF, with band edges HF 0–28 MHz, VHF 28–250 MHz, and UHF 250 MHz–1.766 GHz. A notch is implemented via the R828D open-drain pin. Other V4 specs:
  - Bias tee 4.5 V, 180 mA, software switchable
  - 50 Ω input
  - 1 PPM TCXO
  - Frequency range 500 kHz–1.766 GHz
  - Bandwidth 2.56 MHz stable
  - Current draw 250–270 mA
  
  Vendor-reported results: isolation improved about 8 dB in-band from the notch and 28–43 dB out-of-band, with "some minor degradation in the VHF and UHF band" MDS compared with V3. — [V4 Datasheet](https://www.rtl-sdr.com/wp-content/uploads/2024/12/RTLSDR_V4_Datasheet_V_1_0.pdf); band edges per [CNX Software](https://www.cnx-software.com/2023/08/17/rtl-sdr-blog-v4-dongle-launched-with-rafeal-r828d-tuner-chip/?amp=1&noamp=mobile)
- **V4 availability.** The RTL-SDR Blog store page shows the V4 as EOL, and a "V4L" datasheet (Aug 2026) exists. — [RTL-SDR store](https://rtl-sdr.com/store); [V4L datasheet](https://www.rtl-sdr.com/wp-content/uploads/2026/08/RTLSDR_V4L_Datasheet4.pdf) (V4L not fetched; verify successor specs)
- **V4 bias-tee caution.** Don't leave the bias tee on into a DC-shorted antenna; extended shorts can damage the LDO or fuse. Control with `rtl_biast -b 1/0`, or force always-on with `rtl_eeprom -b 1`. — [rtl-sdr.com/v4](https://www.rtl-sdr.com/v4/)
- **RF switch options (GPIO-controlled).** Note that MW needs a switch rated down to kHz.
  - **pSemi PE42441.** Absorptive SP4T, **10 MHz**–8 GHz, IL 0.8–1.2 dB, isolation 31–45 dB, 1.8 V logic compatible. About $9.04 (1–100) or $5.83 (101–999) at RFMW. Its 10 MHz lower limit is **too high for the MW path**. — [pSemi PE42441](https://www.psemi.com/products/rf-switches/pe42441); [RFMW](https://rfmw.com/products/detail/pe42441-psemi/496195/)
  - **Analog Devices ADG904.** SP4T CMOS, DC to about 2.5 GHz, IL 1.1 dB (DC–1 GHz), off-isolation 37 dB at 1 GHz. It covers MW through ADS-B. — [ADI ADG904](https://www.analog.com/en/products/adg904.html)
  - **ADI ADRF5040.** SP4T, 9 kHz–12 GHz, 0.8 dB IL / 34 dB isolation to 8 GHz. — [ADI ADRF5040](https://www.analog.com/en/products/adrf5040.html)
  - **ADI ADRF5051.** 9 kHz–20 GHz, 0.7 dB IL / 53 dB isolation to 6 GHz. — [ADRF5051 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adrf5051.pdf)
  - **pSemi PE42540.** Absorptive SP4T (HaRP); specs not retrieved. — [pSemi PE42540](https://psemi.com/products/rf-switches/broadband-rf-switches/pe42540)
  - **ADI HMC349.** SPDT, 100 MHz–4 GHz, 62 dB isolation, 1.0 dB IL. Not usable for MW. — [ADI HMC349](https://www.analog.com/en/products/hmc349alp4ce.html)
- **FM band-stop DC pass-through.** Flamingo+ FM passes DC (at least 250 mA). The Flamingo AM is a "proper bandstop" and passes bias-tee DC. — [Nooelec Flamingo+ FM](https://www.nooelec.com/store/flamingo-plus-fm.html); [Flamingo AM listing](https://www.desertcart.ma/products/69945889-flamingo-am-broadcast-am-bandstop-filter-am-notch-filter-software.md)

### Inferences
- **Recommended switched-bank topology (for the NESDR SMArt v5 or any R820T2 dongle):**
  - **Switches:** ADG904 or ADRF5040 at the input and output, driven by 2 GPIO lines each from the head unit's SoC.
  - **Cascaded switch performance:** about 1–2 dB total IL and 60–70 dB+ path-to-path isolation at VHF. Isolation of a single CMOS switch degrades with frequency (37 dB at 1 GHz for the ADG904). Cascading two switches, plus terminating the unused paths, is needed so the stopband of each filter isn't bypassed.
  - **Path 1, AM (direct-sampling Q input).** MW BPF 520–1710 kHz. In the NESDR, direct sampling uses a separate internal Q-branch path, so the MW path must reach the same SMA. Check whether the NESDR SMArt v5 routes direct sampling from the same SMA via an internal diplexer, as the RTL-SDR Blog V3 does.
  - **Path 2, FM.** 87–109 MHz BPF (optional) or through.
  - **Path 3, NOAA/AIS.** 156–164 MHz BPF, 5th-order, IL ≤1.5 dB, ≥40 dB at 108 MHz and at 174 MHz+.
  - **Path 4, ADS-B.** 1090 MHz SAW BPF (IL about 1.5–2.5 dB), ideally plus an LNA at the antenna.
- **Antenna inputs into the bank.** A passive diplexer at the antenna side (LPF/HPF crossover around 300–400 MHz) can combine the car AM/FM/marine whip path with a separate 1090 MHz antenna into the one SMA. That avoids an extra antenna-side switch.
- **Bias tee through the bank.** The switches are DC-blocked, so they don't pass bias-tee DC. Active antennas must instead be fed by:
  - separate per-port bias tees (e.g., an inductor plus a GPIO-controlled load switch), or
  - by placing DC-pass filters (Flamingo-type band-stops) outside the switch.
  
  Car AM/FM amplifiers need 12 V phantom power (Q5). The RTL dongle's 4.5 V bias tee is unsuitable for those.
- **One receiver per service.** Concurrent ADS-B logging and FM audio need two dongles. Cost (about $30–40 per dongle) is low compared with the engineering effort of a filter bank. With that option:
  - a dedicated ADS-B dongle feeds from a 1090 MHz antenna through a SAW/LNA;
  - a second dongle (ideally V4-class) serves AM/FM/NOAA/AIS through the vehicle antenna;
  - an Airspy/SDRplay with real pre-selection would be the premium option for the second receiver.
- **The V4 replaces most of the filter bank.**
  - Its built-in triplexer gives about 28–43 dB isolation between bands.
  - Its upconverter removes direct-sampling aliasing for AM.
  - External filters are then needed only for (a) a MW BPF/LPF if strong out-of-band HF/VHF still causes trouble, and (b) a SAW filter for ADS-B near cellular sites.
  - Its notch is weak ("a few dB") and, per the driver, is switched off when tuned within the notched band (see Gaps), so it never helps the band you are listening to.

### Gaps
- **V4 notch auto-disable (unverified).** The exact driver behaviour, i.e. the frequency thresholds where the rtl-sdr-blog driver turns the notch off inside AM/FM/DAB, was not verified by fetch. Check the rtl-sdr-blog fork `tuner_r82xx.c` (github.com/rtlsdrblog/rtl-sdr-blog).
- **NESDR SMArt v5 direct-sampling wiring.** Whether the direct-sampling input is diplexed internally off the same SMA was not confirmed.
- **Switch prices.** No ADG904 or ADRF5040 unit prices were retrieved.
- **V4L successor.** The V4L datasheet (Aug 2026) was found but not read; it may change the current-production options.
- **SDRplay/Airspy pre-selection.** Specs not retrieved within budget.

---

## Q5. Vehicle antenna arrangement

### Takeaway
Use the factory/aftermarket AM/FM mast, which is usually active and phantom-powered at 12 V from the head unit, for AM, FM, and (marginally) NOAA. Add a dedicated 1090 MHz antenna (roof-mount, ¼-wave about 6.9 cm, or a collinear) with a filtered LNA. For AIS/NOAA, a dedicated marine-VHF 156–163 MHz antenna is preferred: the car FM mast and its amplifier are optimized for, and filter to, 88–108 MHz.

### Cited Findings
- **Phantom power.** "Some radios apply a voltage of 12 V to the antenna cable, the so-called phantom power, which supplies the antenna amplifier." Splitter or amplifier manuals advise checking for phantom power and using a switched 12 V supply. — [Nakatanenga DAB car splitter](https://nakatanenga.de/en/dab-car-splitter-adapter-antenna-amplifier-and-crossover/DABAD)
- **Car active-antenna LNA design.** Reference design: ADI/Maxim MAX2180 AM/FM active-antenna LNA. — [ADI design note](https://www.analog.com/en/resources/design-notes/designing-an-amfm-active-antenna-lna-solution-with-the-max2180-lna.html)
- **ADS-B filtered LNAs:**
  - **RTL-SDR Blog 1090 MHz LNA.** SAW between the two LNA stages plus a SAW on the output; 27 dB gain; out-of-band "reduced by at least 60–80 dB". — [rtl-sdr.com](https://www.rtl-sdr.com/new-product-rtl-sdr-blog-1090-mhz-ads-b-lna/)
  - **Nooelec SAWbird+ ADS-B.** About 34–35 dB gain, NF <0.9 dB, 3.3–5.5 V supply. Reported out-of-band ">85 dB" is a reseller claim only. — [Nooelec](https://www.nooelec.com/store/sawbird-plus-adsb.html)
  - **FlightAware 1090 filter.** Conflicting specs: SAW 1075–1105 MHz with 2.3 dB IL, vs LC 980–1150 MHz with 1.65 dB IL and 40–50 dB attenuation. One FlightAware forum thread says the FlightAware filter does not suppress E-GSM 900 (925–960 MHz) adequately. — [rtl-sdr.com review](https://www.rtl-sdr.com/new-product-from-flightaware-a-1090-mhz-bandpass-filter-for-the-rtl-sdr/); [FlightAware forum](https://discussions.flightaware.com/t/flightaware-ads-b-1090-mhz-band-pass-sma-filter-does-not-work-against-e-gsm-900-925-960-mhz/43203)
  - **GPIO Labs pre-filtered ADS-B LNA.** 1090 ±15 MHz; -10 dB at 700 MHz, -8 dB at 800 MHz, -15 dB at 900 MHz (weak rejection). — [Tindie GPIO Labs](https://www.tindie.com/products/gpio/pre-filtered-1090mhz-ads-b-amplifier-lna-15db-gain)
- **User report of SAWbird misuse.** One user ran a SAWbird with SDR gain dropped to about 3 dB and got worse results; adding gain at the antenna then requires reducing dongle gain. — [Nooelec SAWbird reseller reviews](https://www.desertcart.nz/products/544404882)

### Inferences
- **Splitting the car mast between the OEM/aftermarket tuner and the SDR:**
  - **Splitter loss.** A passive splitter loses about 3.5 dB per leg. That is acceptable for FM/AM with an active mast, but it costs margin for NOAA.
  - **Phantom power.** The 12 V must be injected on the antenna side of the splitter. Use a DC-passing splitter, or a separate 12 V injector driven from the head unit's "antenna remote" output. Neither receiver port should be back-fed 12 V: the RTL dongle input is not DC-blocked on all models.
  - **SDR as the only receiver.** If the SDR is the only receiver (the Godwit case), the head unit itself must supply phantom power. Use a DC injector (inductor plus a DC-block capacitor) ahead of the SDR input and filter bank, switched by GPIO, with a polyfuse.
- **Car active masts and NOAA/AIS.** Amplified FM masts typically band-limit to FM, so their gain at 162 MHz is unknown and may be negative. A separate ¼-wave 162 MHz whip (about 46 cm) or a marine VHF antenna is better for RV use. Weather-radio reception in remote areas is the safety-critical case.
- **ADS-B antenna.** Use a separate roof antenna with the LNA mounted at the antenna. Choose a design with SAW filtering both before and after the LNA if near cellular towers, since E-GSM/LTE 850/900 is the known ADS-B blocker. Do not route ADS-B through the AM/FM mast.

### Gaps
- No source quantified a typical OEM active-mast's gain or filtering at 162 MHz.
- No source gave a standard phantom-power current draw (typically tens of mA, but unsourced).
- **Unverified spec:** the ">85 dB" SAWbird+ ADS-B rejection appears only on reseller listings; the Nooelec datasheet plot was not extracted.

---

## Summary: recommended filter specs per service (synthesised; component-level values are my recommendations derived from the cited data)

| Service | Filter | Cutoff / passband | Order / type | Target IL | Target stopband |
|---|---|---|---|---|---|
| AM (direct sampling, NESDR) | MW band-pass (or LPF + HPF) | 520–1710 kHz; LPF fc 2–3 MHz plus HPF about 450 kHz | 5th–7th-order Chebyshev/elliptic LPF + 3rd-order HPF | ≤2 dB | ≥40 dB at 5 MHz; ≥60 dB at 26–30 MHz (CB alias); ≥70 dB at 54–230 MHz (TV ch2/6/7/11, FM, VHF aliases); good to ≥1 GHz (layout/shielding) |
| AM (V4 upconverter) | Optional MW BPF / FM band-stop | as above | as above | ≤2 dB | ≥40 dB above 3 MHz is ample |
| FM 88–108 | Usually none; optional BPF | 87–109 MHz | 5th-order Chebyshev | ≤1 dB | ≥30 dB at 60 MHz and ≥150 MHz; ≥40 dB from 470 MHz up |
| NOAA 162.4–162.55 / AIS 161.975–162.025 | Marine-VHF band-pass (plus FM band-stop if no BPF) | 155–164 MHz | 5th-order (helical/LC) | ≤1.5–2 dB | ≥40 dB at 88–108 MHz (FM) and ≥30 dB at 174+ MHz (TV ch 7). Paging at 152–159 MHz sits inside or at the edge of this passband, so tighter (161–164 MHz) helical/cavity filters are needed near paging sites. FM band-stop alternative: RTL-SDR Blog (>50 dB, $16.95) or Flamingo+ FM (>60 dB). |
| ADS-B 1090 | SAW BPF + LNA at antenna | 1075–1105 MHz | SAW (+LC) | 1.5–2.5 dB (before LNA hurts NF; put the first SAW after a robust LNA unless overloaded) | ≥40 dB at 925–960 MHz (E-GSM) and 700–900 MHz LTE; ≥60 dB overall |
| User's proposed 200 MHz LPF | e.g., Mini-Circuits BLP-200+ | DC–190 MHz pass, fco 210 MHz | — | <1 dB | >20 dB at 290–390 MHz, >40 dB at 390–800 MHz. Only useful on a switched VHF path. It kills ADS-B, doesn't stop FM/paging/VHF-TV, and isn't an AM anti-alias filter. |
