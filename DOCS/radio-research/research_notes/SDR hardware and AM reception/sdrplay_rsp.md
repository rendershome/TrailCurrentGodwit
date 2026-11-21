# SDRplay RSP family (RSP1B, RSP1A, RSPdx-R2, RSPduo) as the receiver for the Godwit AM/FM head unit

Research date: 2026-11-21. Most primary datasheet/spec-PDF URLs on sdrplay.com returned HTTP 404 during this session (RSP1B datasheet, RSPduo intro PDF, RSPdx ARRL measurements PDF, API spec v3.15 PDF, RSPdx-R2 product page), so several numeric specs below come from product pages, launch articles and older reviews. Gaps are flagged explicitly.

## 1. Hardware specs per model (ADC, bandwidth, range, filters, notches, bias-T, reference, power, price, RSP1A -> RSP1B changes)

### Takeaway
All four models cover 1 kHz–2 GHz continuously with up to 10 MHz of visible spectrum, an 11-band preselector and switchable MW(AM)/FM and DAB notch filters; ADC output is 14-bit below ~6 MHz sample rate and 12-bit above. Prices in 2026 run roughly £106 (RSP1B) to £214 (RSPduo) ex VAT, i.e. about 4–8x an RTL-SDR, but still cheap for a "broadcast-quality" goal.

### Cited Findings
- RSP1B (sdrplay.com product page, Nov 2026): 1 kHz–2 GHz continuous; up to 10 MHz monitored bandwidth; 11 automatically switched preselection band filters; switchable "AM/FM and DAB" notch filters; built-in bias-T to power an external LNA; single tuner; SMA input; max input 0 dBm continuous / +10 dBm burst; −10 °C to +60 °C operating; USB powered; 9.8 × 9.4 × 3.5 cm, 315 g; £105.80 ex VAT (~$138 USD); "In stock". ADC bits, TCXO ppm, current draw and case material not stated on the page. — [SDRplay RSP1B](https://www.sdrplay.com/rsp1b/)
- RSP1B vs RSP1A: SDRplay's datasheet claims improved noise performance below 1 MHz, 3.5–5.5 MHz, 50–60 MHz and 250–320 MHz versus RSP1A; MW/LW improvement is the main headline. — [SDRplay RSP1B datasheet V1.0 (via search snippet; PDF itself 404 at fetch time)](https://www.sdrplay.com/docs/RSP1BdatasheetV1.0.pdf)
- RSP1B was introduced January 2024. — [SDRplay blog, Jan 2024 "Introducing the new SDRplay RSP1B"](https://www.sdrplay.com/2024/01/)
- User reports: an owner of both says the RSP1B's main practical difference is improved MW/LW noise figure (anecdotal); Tech Minds reviewer found sub-1 MHz improvement made NDB reception noticeably better. — [RadioReference RSP1B thread](https://forums.radioreference.com/threads/rsp1b.468670); [rtl-sdr.com on Tech Minds RSP1B review](https://www.rtl-sdr.com/page/65/?pg=409)
- RSP1A ADC: 14-bit output at sample rates below 6.048 MHz, 12-bit above; SDRplay states the 12-bit ADC has ENOB of 10.4 at 8 MHz sampling. RSP1A launched (Nov 2017) at $99. RSP1A added MW, FM and DAB notch filters and much better preselection than the RSP1. — [rtl-sdr.com RSP1A review (2017)](https://www.rtl-sdr.com/a-review-of-the-sdrplay-rsp1a/)
- RSP1A MW notch does not cover the whole MW band: signals at 500–700 kHz are attenuated less. A reader reports the RSP1A DAB notch is too wide and cuts into 166 MHz (relevant to 162 MHz NOAA/AIS if DAB notch is on). — [rtl-sdr.com RSP1A review](https://www.rtl-sdr.com/a-review-of-the-sdrplay-rsp1a/)
- RSPduo: dual tuner; 1 kHz–2 GHz on two SMA ports plus a Hi-Z port (1 kHz–30 MHz) for long wires; 11 band filters; switchable AM/FM and DAB notches; 24 MHz external reference in (1–3 Vpp) and out (~2 Vpp); max input 0 dBm continuous/+10 dBm burst; USB powered; £214.00 ex VAT. Bias-T and TCXO not mentioned on the product page. — [SDRplay RSPduo](https://www.sdrplay.com/rspduo/)
- RSPdx-R2 (launched ~May 2024): enhanced RSPdx with better noise performance below 1 MHz and better dynamic range below 2 MHz in both tuner and HDR modes; redesign partly driven by supply-chain issues; three software-selectable inputs incl. a BNC input up to 200 MHz; 500 kHz LPF for LF/VLF; HDR mode for <2 MHz; notch filters on all inputs; steel case; 1 kHz–2 GHz, 10 MHz visible; launch price £188 / $235 / €225.60 ex tax. — [rtl-sdr.com RSPdx-R2 released](https://www.rtl-sdr.com/sdrplay-rspdx-r2-released/); [SDRplay announcement May 2024](https://www.sdrplay.com/2024/05/10/)
- RSPdx-R2 SDRplay store at search time: £188.00 ex VAT (~$248), marked "Back very soon" (availability gap); distributors e.g. RS Online ~£193.64; eleshop.eu €217.36 ex VAT. — [sdrplay.com/rspdxr2 (search snippet)](https://sdrplay.com/rspdxr2); [RS Online](https://uk.rs-online.com/web/p/communication-wireless-development-tools/0286173); [eleshop.eu](https://eleshop.eu/sdrplay-rspdx-r2-rf-receiver.html)
- RSPdx is described by RSGB (RadCom) as a multi-antenna-port 14-bit SDR with an HDR mode for frequencies below 2 MHz. — [RadCom RSPdx review Jan 2020 (sdrplay.com hosted)](https://www.sdrplay.com/wp-content/uploads/2019/12/RSPdxRadcomJan2020.pdf)

### Inferences
- For a car head unit the RSP1B is the natural candidate: cheapest, single SMA, bias-T (useful for an active amplified car-roof antenna), metal case, and SDRplay's stated improvements are concentrated exactly in MW/LW. RSPdx-R2's extra value (HDR mode <2 MHz, 3 antenna ports, 500 kHz LPF) mainly serves MW/LW DXers; HDR is exposed in the API but the "HDR" benefit is documented chiefly via SDRuno/SDRconnect.
- The MW notch and FM notch are features for *other* bands (they suppress broadcast when listening elsewhere). For AM/FM broadcast reception itself they must be OFF; their value to Godwit is protecting ADS-B/AIS/NOAA/shortwave reception from nearby broadcast transmitters. The DAB notch should be disabled when tuning NOAA/AIS (~162 MHz) given the 166 MHz report.
- 14-bit vs RTL-SDR 8-bit: a ~6 dB/bit rule-of-thumb gives ~36 dB more theoretical ADC dynamic range, but real-world benefit is limited by ENOB (10.4 at 8 MHz) and analog front end; the practical win for FM/AM broadcast is preselection + gain-reduction headroom, not raw bit count.

### Gaps
- Could not retrieve datasheets: exact preselector band edges, notch attenuation (dB) and frequency ranges, LNA state tables per band, noise figure per band, IIP3, TCXO ppm, USB current draw (all PDFs 404). Commonly cited figures (e.g. 0.5 ppm TCXO, ~180–200 mA USB draw, MSi001 tuner + MSi2500-class ADC chain) are NOT verified here.
- RSP1B case material not confirmed on product page (widely described as a metal/steel case, but unverified in this session).
- RSPduo: bias-T presence and reference accuracy not confirmed.
- Current US street prices / stock in late 2026 for RSP1B and RSPduo beyond SDRplay store not checked.

## 2. Measured performance (MDS, dynamic range, FM overload, MW strong-signal) and comparisons vs RTL-SDR V4 / Airspy

### Takeaway
Independent, lab-grade MDS/IP3 numbers for the RSP1A/RSP1B at 1 MHz and 100 MHz were not found in this session; available evidence is qualitative (rtl-sdr.com 2017, forum reports) plus one solid side-by-side (RSP1A = Airspy Mini on 10 m FT8). The RSP1A is much more overload-resistant than the RSP1 on MW/FM thanks to preselection, but its MW strong-signal handling is still regarded as its weak point by MW DXers.

### Cited Findings
- rtl-sdr.com RSP1A review (2017): RSP1 overloaded on the AM band; RSP1A "resisted overload much better"; at 6 MHz the RSP1A's 2–12 MHz preselector filtering of strong AM signals "improved the sensitivity significantly"; at 380/416 MHz FM and DAB images on the RSP1 were reduced/largely gone on RSP1A. — [rtl-sdr.com](https://www.rtl-sdr.com/a-review-of-the-sdrplay-rsp1a/)
- SDRplay told rtl-sdr.com the RSP1A handles FM-band interferers ~10 dB stronger than the RSP1 (vendor figure, not independent). — [rtl-sdr.com](https://www.rtl-sdr.com/a-review-of-the-sdrplay-rsp1a/)
- rtl-sdr.com's comparison verdict (2017): Airspy R2/Mini better for a high-end RF setup with cleaner spectrum; RSP1A better value; Airspy HF+ had the best HF/VHF sensitivity and strong-signal handling of sub-$300 SDRs they tested. — [rtl-sdr.com](https://www.rtl-sdr.com/a-review-of-the-sdrplay-rsp1a/)
- MW DX community summary: one MW DXer found the RSP1A surprisingly good for MW but weak against strong signals; another found its noise only slightly lower than other units. — [search summary of RadioReference RSP1B thread & related](https://forums.radioreference.com/threads/rsp1b.468670)
- VE3NEA (Jan 2024), 24 h side-by-side on 10 m FT8 through a splitter: Airspy Mini 126,415 decodes vs RSP1A 126,286 (<0.1% difference); "same FT8 decoding performance"; both occasionally overloaded (not simultaneously). — [VE3NEA JTSkimmer blog](https://ve3nea.github.io/JTSkimmer/2024/01/30/rsp1a-vs-airspy-10m.html)
- Adam Farson (AB4OJ/VA7OJ) methodology: MDS, RMDR, 2nd/3rd-order IMD; DR3 = Pi − MDS; IP3 = 1.5·DR3 + MDS. No Farson RSP-series report was found. — [AB4OJ FTDX-3000 notes](https://www.qsl.net/a/ab4oj/test/docs/ft3000notes.pdf); [AB4OJ SDR test slides](https://www.qsl.net/ab4oj/sdr/sdrtest2.pdf)
- SDRplay publishes ARRL-lab measurements for the RSPdx and a RadCom review (front end redesigned for better HF dynamic range), but the PDF could not be retrieved for numbers. — [RSPdx ARRL measurements (404 at fetch)](https://www.sdrplay.com/docs/RSPdxARRLmeasurements.pdf); [RadCom RSPdx review](https://www.sdrplay.com/wp-content/uploads/2019/12/RSPdxRadcomJan2020.pdf)

### Inferences
- For FM broadcast near strong transmitters (urban driving, passing hilltop sites), the RSP's tracking preselector (FM band filter) plus fine-grained gain reduction should be a real improvement over the R820T2-based NESDR, whose wideband front end feeds an 8-bit ADC; but FM broadcast audio quality is ultimately bounded by the demodulator/stereo decoder/RDS chain, and a good RTL-SDR already delivers full-quiet stereo on strong stations. The gain from an RSP for FM is mostly in weak-signal/adjacent-channel/overload scenarios, not in "fidelity".
- For AM (MW): the NESDR's direct-sampling mode (Q-branch, no preselection, 8-bit, no AGC) is the weakest part of the current design; any RSP gives a proper tuned MW path with filtering and 14-bit sampling at narrow rates, which is likely the single biggest audible improvement. RSP1B / RSPdx-R2 specifically target <1 MHz noise.
- In a car, the dominant MW noise source is vehicle EMI (alternator, LED drivers, DC-DC converters), which no SDR fixes; antenna/grounding matter more than SDR choice once a properly filtered receiver is used.

### Gaps
- No independent measured MDS at 1 MHz / 100 MHz, IIP3, blocking DR, or FM-band overload numbers for RSP1A/RSP1B/RSPdx-R2/RSPduo were retrieved.
- No head-to-head RSP vs RTL-SDR Blog V4 (which has HF upconverter + filtering) measurement found; this is the most relevant comparison and is missing.
- No SWLing Post measured data retrieved (only an RSPduo 2018 review URL surfaced).

## 3. Software and licensing (closed API, ARM64, service daemon, redistribution in a Yocto/AGL image, open-source alternatives)

### Takeaway
The SDRplay API 3.15 is closed-source, free of charge, and the Linux installer includes x64, ARM32 and ARM64 builds — but the current EULA (effective 12 Jan 2021) grants only rights to "publicly display, publicly perform" the software, contains no redistribution grant, forbids transfer and reverse engineering, and restricts use to SDRplay hardware. Bundling `sdrplay_apiService` + `libsdrplay_api.so` in a distributed AGL image is therefore not clearly permitted without written permission from SDRplay; an older (pre-2021) SDRplay licence did allow unmodified redistribution.

### Cited Findings
- Current API version 3.15; Linux download 499 KB; supports RSP1, RSP2/pro, RSP1A, RSP1B, RSPduo, RSPdx, RSPdx-R2 (nRSP-ST not supported — use SDRconnect WebSocket API instead). Linux "only tested and supported on Ubuntu LTS distributions up to 24.04 LTS". — [SDRplay API page](https://www.sdrplay.com/api/)
- "The Linux installer contains the x64, ARM32 and ARM64 versions of the API"; "We only test on Ubuntu LTS OS distributions". — [SDRplay Hardware API page (search snippet)](https://sdrplay.com/API/)
- RSPdx-R2 support requires API 3.15 or later; Linrad 5.02 only works with API ≤3.11 (i.e., 3.x releases have broken some third-party apps). — [Linrad group: SDRplay API 3.15](https://groups.google.com/g/linrad/c/CZea0QRCqIU)
- Arch AUR libsdrplay 3.15.2 lists aarch64; NixOS packages 3.15.1; sdr-enthusiasts installer has an aarch64 path that installs an ARM64 `sdrplay_apiService` binary (so the API uses a separate service daemon that clients talk to). — [AUR libsdrplay](https://aur.archlinux.org/packages/libsdrplay); [sdr-enthusiasts install_sdrplay.sh](https://github.com/sdr-enthusiasts/install-libsdrplay/blob/main/install_sdrplay.sh); [mynixos sdrplay](https://mynixos.com/nixpkgs/package/sdrplay)
- In 2021 SoapySDR maintainers said SDRplay did not provide a Linux AArch64 API build; that is now superseded by the ARM64 build in the installer. — [SoapySDR issue #333](https://github.com/pothosware/SoapySDR/issues/333)
- Current EULA (effective 12 Jan 2021) as mirrored with the API in srcejon/sdrplayapi (amd64, arm64, armhf binaries): §1.1 grants "a perpetual, worldwide, non-exclusive, no-charge, royalty-free, irrevocable license to publicly display, publicly perform the Software in Object form" (no reproduce/distribute rights); §1.2.1 "you may use the Software only with SDRplay hardware radio products"; §1.2.3 no modify/disassemble/decompile/reverse engineer; §3 no use of SDRplay/Mirics trademarks; §7 no transfer/assignment of rights. No explicit commercial-use restriction. — [srcejon/sdrplayapi licence](https://raw.githubusercontent.com/srcejon/sdrplayapi/master/sdrplay_license.txt); [repo](https://github.com/srcejon/sdrplayapi)
- Older SDRplay licence (Gentoo licenses/SDRplay) granted rights "to reproduce ... and distribute the Product in Object form", with conditions: include a copy of the licence, mark modified files, and include a statement that copyright is owned/licensed by SDRplay. — [Gentoo licenses/SDRplay](http://mirrors.edge.kernel.org/gentoo-portage/licenses/SDRplay)
- Gentoo ebuild sdrplay-3.07.1 installs only x86_64/i686 service binaries (distro packaging typically fetches the vendor installer rather than mirroring it). — [Gentoo ebuild](https://mirrors.aliyun.com/gentoo-portage/net-wireless/sdrplay/sdrplay-3.07.1.ebuild)
- SoapySDRPlay3: MIT-licensed SoapySDR module; "requires SDRplay API V3.15 or later"; includes RSPduo dual-tuner support (Franco Venturi, 2020); README notes it contains AI-generated code. — [SoapySDRPlay3](https://github.com/pothosware/SoapySDRPlay3)
- Open-source alternative: libmirisdr-5 (ericek111) works "mainly OK" with an RSP1A in gqrx via gr-osmosdr (2023 report); earlier libmirisdr-4 forks recognised the RSP1A USB ID but lacked its specific controls (preselector/notch) due to missing documentation; the in-kernel msi2500 driver must be blacklisted; a user reported dead zones (<~48 MHz, ~300–485 MHz) with libmirisdr on MSi2500 hardware. SDRplay hardware is described as closed hardware/closed source. — [Gqrx group RSP1A + libmirisdr-5](https://groups.google.com/g/gqrx/c/uf8fYPbcNnQ); [revspace Msi2500SDR](https://www.revspace.nl/Msi2500SDR); [f4exb/libmirisdr-4](https://github.com/f4exb/libmirisdr-4/releases); [Arch wiki SDRplay](https://wiki.archlinux.org/title/SDRplay)

### Inferences
- Yocto/AGL: technically feasible (ARM64 .so + `sdrplay_apiService` + a systemd unit + udev rule; glibc-linked binaries built for Ubuntu should run on a recent Yocto glibc but this is unsupported). Legally, an MIT open-source project should not ship the binary in a public image under the 2021 EULA. Options: (a) a recipe that fetches the installer at build time with `LICENSE_FLAGS = "commercial"`-style gating so each builder accepts SDRplay's EULA themselves (like the Gentoo/AUR approach), (b) first-boot download, or (c) obtain written redistribution permission from SDRplay. No meta-sdr/OpenEmbedded recipe for the SDRplay API was found in this session.
- A closed, x86/Ubuntu-tested, daemon-based blob is a maintenance and security liability in an automotive image (no source to patch; ABI breakage between 3.x versions already observed with Linrad). libmirisdr-5 avoids the blob but loses the RSP-specific preselector/notch/bias-T control that is the main reason to buy an RSP, so it's not a real substitute for broadcast-quality use.

### Gaps
- Did not read the licence text shipped *inside* the 3.15 installer directly; the 12 Jan 2021 EULA is assumed current based on the mirror. SDRplay's position on OEM/redistribution permission is unknown.
- No Yocto/OpenEmbedded layer (meta-sdr etc.) packaging the SDRplay API was confirmed.
- Exact API 3.15 release date and any 3.16+ release in 2025–2026 not confirmed (API page shows only 3.15, copyright 2014–2026).

## 4. Integration (SoapySDR, multi-app sharing, RSPduo dual-tuner, CPU on ARM, USB 2.0)

### Takeaway
SoapySDRPlay3 gives a standard SoapySDR path (requires API ≥3.15). The RSPduo can run two receivers at once, but in dual-tuner mode each tuner is restricted to low-IF with ≤1.536 MHz IF bandwidth (≈2 MHz slices) and the slave's sample rate is dictated by the master — fine for FM + RDS/AF check or FM + NOAA/AIS, but too narrow for ADS-B (which wants ≥2 MHz, ideally more) to run concurrently at full quality.

### Cited Findings
- RSPduo: single-tuner zero-IF mode up to 10 MHz displayed bandwidth; dual-tuner mode max 2 MHz displayed per tuner; dual-tuner IF bandwidth 200 kHz–1.536 MHz in low-IF mode. — [RSPduo Introduction V3 (search snippet; PDF 404 at fetch)](https://www.sdrplay.com/wp-content/uploads/2018/05/RSPduo-Introduction-V3.pdf); [RSPduo datasheet V0.5 (snippet)](https://www.sdrplay.com/docs/RSPduoDatasheetV0.5.pdf)
- Master/slave: the first tuner opened becomes master; slave sample rate is determined by the master; master must remain active for the slave to work; separate API instances (and e.g. separate SDRuno instances) per tuner; only one IF mode gave balanced output at 6 and 8 MHz sample rates (jsdrplay). — [jsdrplay wiki](https://github.com/DSheirer/jsdrplay/wiki); [SWLing Post RSPduo review 2018](https://swling.com/blog/2018/05/a-review-of-the-sdrplay-rspduo-14-bit-dual-tuner-sdr); [Linrad group RSPduo dual tuner](https://groups.google.com/g/linrad/c/QjtWnv-bP0Q); [SDRplay community recording both channels](https://www.sdrplay.com/community/viewtopic.php?p=16106)
- SoapySDRPlay3 supports RSPduo dual-tuner; requires API 3.15+. — [SoapySDRPlay3](https://github.com/pothosware/SoapySDRPlay3)
- SDRplay's SDRconnect runs natively on Raspberry Pi 4/5 (ARM64), indicating the API is usable on Pi 5-class ARM. — [SDRconnect page (search snippet)](https://sdrplay.com/SDRconnect)

### Inferences
- Architecture: the API is client/service — apps link `libsdrplay_api` and talk to `sdrplay_apiService`, which owns the USB device; this is what lets two processes use the two RSPduo tuners (master/slave). It does not let two apps share one single-tuner RSP simultaneously (same as librtlsdr). Godwit would need its own sample-distribution layer (or SoapyRemote/rtl_tcp-like fan-out) as today.
- USB 2.0: 10 MHz × 2 × 16-bit ≈ 40 MB/s, near the practical USB 2.0 HS limit; 2–2.4 MS/s for FM/ADS-B is trivial. CPU cost on Pi 5/Dragon Q6A is dominated by DSP, not the API; no measured figures found.
- Dual-tuner use cases for Godwit: FM (≈200–400 kHz needed) + RDS AF scanning on tuner 2 works within 1.536 MHz; FM + NOAA/AIS (162 MHz, 25 kHz channels; AIS 161.975/162.025 MHz) works; FM + ADS-B is marginal (ADS-B decoders typically want 2–2.4 MS/s or more; a 1.536 MHz IF filter cuts 1090ES pulse fidelity) — unverified, would need testing. Note both tuners share the master's sample rate.

### Gaps
- No measured CPU usage of sdrplay_apiService/SoapySDRPlay3 on ARM64.
- Whether SoapySDRPlay3 exposes RSPduo master/slave to two independent processes cleanly on aarch64 — not verified.
- Whether dual-tuner mode imposes any restriction on the two tuner frequencies (e.g., shared LO band constraints) not confirmed.

## 5. Fit for other services: ADS-B 1090 MHz and AIS/NOAA 162 MHz

### Takeaway
No rigorous RSP1A/RSP1B ADS-B benchmark was found; anecdotal reports range from "15–25 miles more than RTL-SDR" (old RSP1) to "poor ADS-B performance" threads, and SDRplay's ADS-B software path (dump1090 forks, sdr-enthusiasts docker image) is less mainstream than readsb/dump1090-fa with RTL-SDR. For 162 MHz NOAA/AIS any RSP is adequate; disable the DAB notch.

### Cited Findings
- Old RSP1 vs NESDR SMArt anecdote: SDRplay gave "15 to 25 miles more coverage" than an RTL-SDR side by side on identical antennas (single informal report). — [FlightRadar24 forum](https://forum.flightradar24.com/forum/radar-forums/technical-matters-hardware/198268-sdrplay-rsp1-vs-nooelec-nesdrmart)
- SDRplay forum thread "Poor ADSB performance with RSP1A": SDRplay's downloadable dump1090 was based on an old, unmaintained dump1090-mutability. — [SDRplay community](https://www.sdrplay.com/community/viewtopic.php?p=14645)
- sdr-enthusiasts maintain docker-sdrplay-beast1090 supporting RSP1, RSP1A, RSP1B, RSPdx, RSPduo with BEAST output. — [docker-sdrplay-beast1090](https://github.com/sdr-enthusiasts/docker-sdrplay-beast1090)
- FlightAware users requested SDRplay support in PiAware because it "performs better than an rtl-sdr stick" (opinion). — [FlightAware discussion](https://discussions.flightaware.com/t/sdrplay-on-piaware/19389)
- Airspy measurably beats R820T2 RTL-SDR on ADS-B (~50% more messages in one forum report); no equivalent RSP data. — [rtl-sdr.com RTL-SDR vs Airspy ADS-B](https://www.rtl-sdr.com/rtl-sdr-vs-airspy-on-ads-b-reception/); [FlightAware Airspy vs RTL](https://discussions.flightaware.com/t/comparison-of-airspy-and-rtl-sdr-dongles/60887)
- RSP1A DAB notch reportedly cuts into 166 MHz. — [rtl-sdr.com RSP1A review comments](https://www.rtl-sdr.com/a-review-of-the-sdrplay-rsp1a/)

### Inferences
- In a vehicle, ADS-B performance is dominated by antenna placement and an external 1090 MHz filtered LNA (bias-T powered on RSP1B); a dedicated cheap 1090-optimised RTL-SDR (e.g. FlightAware Pro Stick) is a common, open-source-friendly choice and could stay alongside an RSP used for AM/FM.
- Recommended architecture to weigh: RSP1B (or RSPdx-R2) dedicated to broadcast AM/FM (+ NOAA), keeping a librtlsdr-based dongle for ADS-B/AIS — avoids time-sharing and keeps the open-source path for secondary services. Cost: licensing/distribution issue in §3 remains for the RSP.

### Gaps
- No measured 1090 MHz sensitivity/NF or message-rate comparison for RSP1A/RSP1B vs RTL-SDR Blog V4 / Airspy.
- No AIS-specific RSP test data found.
