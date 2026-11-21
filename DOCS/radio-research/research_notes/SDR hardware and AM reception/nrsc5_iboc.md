# NRSC-5 IBOC Digital Radio Reception for TrailCurrent Godwit

Terminology note: the system is called "NRSC-5" / "NRSC-5 IBOC" / "IBOC digital radio" throughout these notes. The consumer brand name for this system is a registered trademark of Xperi (via its subsidiary iBiquity Digital Corp.); an open-source project must not use that name, its logo, or brand-style channel labels in UI, docs or marketing without a licence from Xperi. Multicast channels are called "supplemental program services" (SPS) here; the main program is the "main program service" (MPS). Some source URLs below contain the trademark because that is their actual title.

Research date: 2026-11-21. Tool-call budget limited; items marked "(unverified, from background knowledge)" were not confirmed by a fetched source in this session and should be checked before being relied on.

## 1. The standard: service modes, sidebands, codec, data services, blend

### Takeaway
NRSC-5-E is the current revision. FM hybrid places OFDM sidebands on both sides of the analog FM signal, at roughly ±130 to ±200 kHz from the carrier; extended hybrid (MP11 etc.) moves the inner edge in to about ±100 kHz. AM hybrid MA1 occupies ±15 kHz, and all-digital AM MA3 uses 10 to 20 kHz. Audio uses the proprietary NRSC-5 audio codec, an HE-AAC variant, which sits outside the open part of the standard. The analog audio is delayed at the transmitter by a "diversity delay" of several seconds, so a receiver has to time-align and level-match the two paths before it blends.

### Cited Findings
- NRSC-5-E is the current revision of the standard. The NRSC page lists the NRSC-5-E document, its reference documents, Xperi supplemental information (program types, AAS port numbers, text encoding types, service data types) and patent disclosures (Xperi 2023; Impulse Radio 2005) — [NRSC NRSC-5-E page](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-5-e/nrsc-5-e.asp); [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)
- Primary service modes in the reference document set are MP1–MP3, MP11, MP5 and MP6. Secondary (all-digital FM) modes are MS1–MS4 — [NRSC reference docs search snippet](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-5-e/NRSC-5-E_reference_documents.pdf) (the PDF was too large to fetch, so this comes from the search snippet only)
- Radio World reported that the NRSC was finalising NRSC-5-E with new Xperi-developed IBOC service modes. These build on the MP11 spectrum footprint by using full 100 kHz sidebands and higher-order modulation. MP11 has long been FCC-approved as an extended hybrid mode. The article date was not visible — [Radio World](https://www.radioworld.com/?p=118928) (search snippet)
- FM hybrid: the analog signal occupies ±130 kHz. The primary digital sidebands extend about 70 kHz beyond that on each side, for a total of about 400 kHz. Extended hybrid limits analog to ±100 kHz and adds extended partitions in the freed ±30 kHz, giving about 50 kbit/s more. Hybrid capacity is about 100, 112, 125 or 150 kbit/s depending on mode. All-digital FM reaches up to 300 kbit/s — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)
- OFDM details from the nrsc5 authors: the working rate is 744,187.5 S/s with a 2048-point FFT, which gives 363.37 Hz subcarrier spacing. Each sideband is about 69 kHz wide. MP1 has 10 partitions per sideband, each with 18 data subcarriers plus reference subcarriers (QPSK data, DBPSK reference). An interleaver block spans 32 OFDM symbols and 16 blocks make 512 symbols. FEC is a K=7 rate-1/3 tail-biting convolutional mother code with puncturing. Layer-2 frames are 18,269 bytes. The decoder's band-pass filter covers 122,457–372,095 Hz in its frame of reference — [Theori blog "Receiving NRSC-5" (2017)](https://theori.io/blog/receiving-nrsc-5)
- Exact standard sideband edges are about ±129.36 kHz to ±198.40 kHz for the primary main partitions, with the MP11 extended partitions reaching inward to about ±101.7 kHz (unverified, from background knowledge; consistent with the ±130/±200 kHz figures above).
- AM hybrid MA1 occupies 30 kHz (±15 kHz) and overlaps the adjacent channels, carrying about 40–60 kbit/s (40 kbit/s default). With the NRSC-5 audio codec with SBR, audio reaches 15 kHz. All-digital AM MA3 carries up to 40.2 kbit/s in 20 kHz out to the 0.5 mV/m contour, and about 20.2 kbit/s in a 10 kHz core beyond it — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)
- the NRSC-5 audio codec is proprietary and MDCT-based, related to HE-AAC but not compatible with it. It is a trade secret, and the specification is only partly open — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio). The nrsc5 authors found the NRSC-5 audio codec is "essentially HE-AAC with some modifications": reordered window fields, no element instance tags or predictor flag, a 1-bit SBR flag, and only SCE/CPE/FIL elements. They adapted FAAD2 accordingly — [Theori blog](https://theori.io/blog/receiving-nrsc-5)
- Multicast: hybrid FM carries the MPS plus SPS2–SPS4 (and more). All-digital FM supports up to seven channels. Per-channel audio is typically 96–128 kbit/s for the MPS, and 48 kbit/s is cited as enough for good fidelity — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio). In October 2020 the FCC found an AM SPS audio stream had not been shown to be technically feasible — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)
- PSD (title/artist) is unencrypted and unsigned — [Theori blog](https://theori.io/blog/receiving-nrsc-5). nrsc5 decodes PSD, SIS, LOT files (album art, station logos), AAS data and emergency alerts (see section 2).
- Diversity delay and blend: the transmitter delays the analog audio by a diversity delay, and the receiver delays the digital audio relative to analog. The receiver plays analog first (fast tune-in), then blends to digital. Time diversity also lets the receiver ride through short outages — [US9768948 time alignment patent](https://patents.google.com/patent/US9768948); [US7546088 blend patent](https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/7546088)
- One patent gives the diversity delay as greater than 2.0 s, preferably 3.0–5.0 s — [search summary of iBiquity patents](https://patents.google.com/patent/US9768948). The commonly quoted figure is about 4.458 s (65,536 × 2 samples at 44.1 kHz ≈ 2.97 s is a different quantity; unverified, from background knowledge). I found no fetched primary source for the exact value.
- Blending only works if the two paths are aligned in time. The actual analog/digital misalignment at a station is often neither known nor fixed, and patents describe normalised cross-correlation of audio envelopes to measure the offset, aligning to within about 3 samples — [USRE48966 time-alignment measurement](https://patents.google.com/patent/USRE48966); [US8180470 fine alignment](https://patents.justia.com/patent/8180470)
- Misalignment is audible: "echo"/"reverb" is a known complaint when analog and digital receivers play together, and receivers can lock to distant digital signals during tropospheric ducting — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)
- Digital power: the original level was −20 dBc (1%). A 2010 FCC order allowed −14 dBc (4%) voluntarily, and up to −10 dBc (10%) on showing no interference — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio). The FCC First Report and Order FCC 24-105 (adopted 24 Sep 2024, MB Docket 22-405, 89 FR 84096) allows asymmetric sidebands by notification — [Federal Register 2024-24105](https://www.govinfo.gov/content/pkg/FR-2024-10-21/html/2024-24105.htm); [FCC 24-105](https://docs.fcc.gov/public/attachments/FCC-24-105A1_Rcd.pdf)
- The FCC authorised voluntary all-digital AM (MA3) on 27 Oct 2020 (MB Docket 19-311) — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio); [Federal Register 2020-25252](https://govinfo.gov/content/pkg/FR-2020-12-03/pdf/2020-25252.pdf)

### Inferences
- For Godwit's FM path, the receiver must cover carrier ±200 kHz (±100–200 kHz for the extended modes), about 400 kHz total. This is well within one RTL-SDR capture.
- Asymmetric sideband operation (now routine under FCC 24-105) means a receiver should not assume equal sideband power. nrsc5 combines both sidebands, but one may be weaker.
- A good blend needs three things: (a) a delay line on analog of about 3–5 s (the exact value is measured per station by cross-correlation, not taken from a constant); (b) level and EQ matching (digital is often processed differently and has wider bandwidth); and (c) a hysteresis-based, crossfaded blend driven by digital quality metrics (MER/BER, CRC errors, decoder audio-frame errors). Blending back to analog must happen before audible digital muting.

### Gaps
- I could not retrieve the NRSC-5-E adoption date, the exact list of new service modes added in E, or the exact sideband frequency tables. The NRSC reference-document PDF is over 10 MB and was not fetched. Check NRSC-5-E and reference doc 1011s (FM air interface) and 1012s (AM air interface) directly.
- I did not confirm the exact nominal diversity delay value from a primary document.
- I did not find whether the FCC finalised the FM digital power methodology change (beyond asymmetric sidebands) by late 2026. Wikipedia mentions a May 2025 FCC confirmation on asymmetric sideband power, but that was not checked against the FCC record.

## 2. The open-source decoder nrsc5

### Takeaway
nrsc5 (theori-io/nrsc5) is GPL-3.0-or-later. It is actively maintained in 2026: v3.2.0 was released 2 June 2026, and PRs were open in Sept 2026. It decodes FM hybrid, AM hybrid (MA1) and, since May 2024, MA3 (with reported sync bugs). It supports multiple programs, PSD, SIS, LOT, alerts and HERE images, and offers a C API (libnrsc5) and Python bindings. It accepts RTL-SDR, rtl_tcp or I/Q input at 1,488,375 S/s cu8, or 744,188 S/s (FM) / 46,512 S/s (AM) in cs16/cf32. CPU cost on ARM is modest: a Pi 3 handled it on one core.

### Cited Findings
- The licence is GNU GPL version 3, "or (at your option) any later version" — [nrsc5 LICENSE](https://raw.githubusercontent.com/theori-io/nrsc5/master/LICENSE)
- Inputs: RTL-SDR by index (`-d`), `rtl_tcp` (`-H host:port`), and I/Q file or stdin (`-r`). It can save raw I/Q (`-w`) and supports gain, ppm, bias-T and direct sampling (`-D`) options. FM is the default and `--am` selects AM — [nrsc5 README](https://github.com/theori-io/nrsc5)
- Exact sample formats: cu8 at 1,488,375 S/s; cs16 at 744,188 S/s (FM) / 46,512 S/s (AM); cf32 at 744,188 S/s (FM) / 46,512 S/s (AM). `--iq-input-format` was added to the CLI in v3.2.0 — [nrsc5 README](https://github.com/theori-io/nrsc5); [nrsc5 releases](https://github.com/theori-io/nrsc5/releases)
- 1,488,375 = 2 × 744,187.5. The decoder works at 744,187.5 S/s with a 2048-point FFT — [Theori blog](https://theori.io/blog/receiving-nrsc-5)
- API: there is a C API in `include/nrsc5.h` (libnrsc5), documented on the project's GitHub Pages, and a Python wrapper in `support/nrsc5.py`. The CLI is built on the C API — [nrsc5 README](https://github.com/theori-io/nrsc5)
- Build: Ubuntu, Debian and Raspbian instructions are provided. `-DUSE_NEON=ON` enables ARM NEON (off by default). FAAD2 with the NRSC-5 audio codec patch is enabled by default (`-DUSE_FAAD2=ON`). Dependencies include FFTW3, librtlsdr, libao, CMake and libtool — [nrsc5 README](https://github.com/theori-io/nrsc5). v3.1.0 can use a system FAAD2 if it is already patched, and the CLI is optional (library-only build) — [releases](https://github.com/theori-io/nrsc5/releases)
- Programs and output: program numbers 0–3 on the command line, with keys 0–7 switching programs at runtime. The service limit rose from 8 to 16 in v3.0. Output is WAV or raw. `--dump-aas-files <dir>` writes LOT/HERE files (the README calls it "insecure"). `--dump-hdc` dumps the NRSC-5 audio codec packets — [README](https://github.com/theori-io/nrsc5); [releases](https://github.com/theori-io/nrsc5/releases)
- Release history (years inferred from linked PR dates):
  - v3.0 (1 Jun 2025): HERE Images decoding; low-SNR audio corruption fix; faster startup on Raspberry Pi (#411, merged 28 May 2025); elastic audio buffering (less jittery audio events); `NRSC5_EVENT_AUDIO_SERVICE`; emergency-alert clear event; SIS event split into finer events.
  - v3.1.0 (22 Dec 2025): service mode and frequency offset reported; LOT header and fragment events; AGC status event; EAS length fixes.
  - v3.2.0 (2 Jun 2026, "Latest"): CRC errors reported via the API; AM system control values exposed; exciter/importer version, leap seconds and local time exposed; audio output queue refactor.
  - Sources: [nrsc5 releases](https://github.com/theori-io/nrsc5/releases); [issue search](https://github.com/theori-io/nrsc5/issues?q=raspberry)
- MA3: issue #240 "Implement AM all-digital mode (MA3)" was closed and PR #344 "Add MA3 (all digital) service mode" was merged on 6 May 2024. Issue #387 "AM synchronization fails for MA3" was opened 28 Mar 2025 and appears open. PR #388 (block sync moved to the acquire stage, merged 29 Mar 2025) may relate. Issue #502 "Refactor AM decoding" was opened 20 Apr 2026. PR #477 (MA1: delay P3 to match P1) is open — [nrsc5 issues, MA3 search](https://github.com/theori-io/nrsc5/issues?q=MA3)
- Maintenance: PR #538 was opened 28 Sep 2026 and updated 30 Sep 2026, showing activity in late 2026 — [nrsc5 issues](https://github.com/theori-io/nrsc5/issues?q=raspberry)
- ARM CPU:
  - Pi 3, single core: 60–70% average CPU in 2017, before optimisation. After PRs #95/#106/#107 (Nov 2017), a Pi 3 with USE_NEON showed a 15-minute load average of about 0.55, and the maintainer concluded "ARM performance seems to be adequate already."
  - Allwinner R8 (C.H.I.P., single-core) had input buffer overflows.
  - Source: [nrsc5 issue #17](https://github.com/theori-io/nrsc5/issues/17)
  - Inference: a Pi 5, CM5 or QCS6490 (Dragon Q6A) has many times a Pi 3's per-core performance, so nrsc5 should use well under one core. This has not been measured.
- SoapySDR: the upstream README does not mention SoapySDR. The fventuri fork adds SDRplay and SoapySDR device support — [fventuri/nrsc5 mirror](https://gh.r-mj.com/fventuri/nrsc5). NRSC5-DUI (a GUI fork) lists Airspy HF+ Discovery / HF+ Dual Port as supported and Airspy Mini/R2 as not supported — [search summary of NRSC5-DUI](https://www.rtl-sdr.com/?p=52007). Other front-ends include NRSC5 Studio (Windows GUI) — [rtl-sdr.com](https://www.rtl-sdr.com/nrsc5-studio-a-new-feature-rich-windows-gui-for-the-nrsc5-hd-radio-decoder/); and GNU Radio/HackRF decoding — [rtl-sdr.com](https://www.rtl-sdr.com/decoding-nrsc5-hd-radio-with-gnu-radio-and-a-hackrf/). No SDR++ plugin was found.
- Sensitivity: I found no measured comparison of nrsc5 against commercial receivers. Commercial receivers differ among themselves by almost 20 dB in first-adjacent rejection — [Radio World receiver design article](https://www.radioworld.com/?p=78681). Commercial receivers use hysteresis on digital acquisition — [element14 review](https://community.element14.com/technologies/wireless/b/blog/posts/the-best-hd-radio-receiver). nrsc5 decodes only the digital layer and has no analog fallback of its own — [rtl-sdr.com](https://www.rtl-sdr.com/decoding-and-listening-to-hd-radio-nrsc-5-with-an-rtl-sdr/)
- Real-world hybrid precedent: Ivan (NO2CW) added NRSC-5 to an FM-DX Webserver (2026). A TEF6686 handles analog band browsing, and an RTL-SDR V3 running the nrsc5 library takes over for digital, exposing MPS/SPS streams. The plugin code is by GitHub user Seehed ([Seehed/NRSC5_HDRadio](https://github.com/Seehed/NRSC5_HDRadio)). An RTL-SDR V4 was tried first, but the needed DLLs worked better with the V3 on Windows — [rtl-sdr.com](https://www.rtl-sdr.com/adding-hd-radio-support-to-fm-dx-webservers-with-an-rtl-sdr/)

### Inferences
- Latency: in this design, nrsc5's own latency matters less than the station's diversity delay. Digital audio arrives about 3–5 s after the analog audio for the same programme. Tune-to-digital-audio time is a few seconds: OFDM sync plus interleaver depth of 512 symbols × about 2.9 ms ≈ 1.5 s, plus audio buffering. The last two figures are estimates, not measured.
- Godwit should treat MA3 as experimental and MA1 as usable but less tested than FM.

### Gaps
- No published Pi 4/Pi 5/ARM64 CPU figures for current nrsc5 versions; benchmark on target hardware.
- I did not fetch the libnrsc5 API docs. From background knowledge (unverified), libnrsc5 provides `nrsc5_open` (RTL device), `nrsc5_open_pipe` plus `nrsc5_pipe_samples_cu8/cs16` (push I/Q from your own code), `nrsc5_open_rtltcp`, `nrsc5_set_frequency`, `nrsc5_set_callback`, and events for audio, ID3/PSD, SIS, LOT, sync/lost sync, MER/BER, and alerts. Confirm against `include/nrsc5.h` v3.2.0, especially cf32 pipe support.
- No measured sensitivity data for nrsc5 with an RTL-SDR versus a Si468x or SAF360x receiver.

## 3. Sharing one RTL-SDR capture between analog FM demod and nrsc5

### Takeaway
Yes. All IBOC FM sidebands lie within carrier ±200 kHz, so one RTL-SDR capture can feed both the analog demodulator and nrsc5. Godwit's current 1.14 MS/s with the station at +285 kHz already contains them: the upper sideband reaches +483 kHz, inside Nyquist at ±570 kHz. But nrsc5 needs exactly 744,187.5 S/s centred on the carrier, so either resample (ratio 3969/6080) or change the capture rate to 1,488,375 S/s and decimate by 2 after mixing.

### Cited Findings
- Hybrid FM digital energy extends to about ±200 kHz (total about 400 kHz) — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)
- nrsc5 accepts cs16/cf32 baseband at 744,188 S/s (FM) and 46,512 S/s (AM), or cu8 at 1,488,375 S/s — [nrsc5 README](https://github.com/theori-io/nrsc5)
- Upstream nrsc5 itself runs the RTL-SDR at 1,488,375 S/s, decimating by 2 with a band-pass filter that also removes the analog FM — [Theori blog](https://theori.io/blog/receiving-nrsc-5)

### Inferences
- The arithmetic is checked here, not sourced:
  - The station sits at +285 kHz in Godwit's 1.14 MS/s capture, so the lower sideband spans +87 to +156 kHz and the upper sideband spans +414 to +483 kHz. Both are inside ±570 kHz, but the upper edge falls in the outer 15% of the band, where RTL2832U decimation-filter roll-off and aliasing begin. Possible effects are a slight amplitude tilt on the upper sideband and aliasing from stations about 1.14 MHz away folding in.
  - Option A (keep 1.14 MS/s): mix by −285 kHz, low-pass to about ±210 kHz, then rational-resample by 744,187.5/1,140,000 = 3969/6080 to cf32. The fraction reduces because 1,488,375 = 3⁵·5³·7² and 2,280,000 = 2⁶·3·5⁴·19. Use a polyphase or arbitrary resampler (e.g. liquid-dsp `msresamp_crcf`) and push the result to libnrsc5 through the pipe API or stdin.
  - Option B (recommended): run the dongle at 1,488,375 S/s, which is a valid RTL rate (nrsc5 uses it). Keep a 285 kHz offset (upper sideband at +483 kHz, well inside ±744 kHz). Mix to 0 Hz, decimate by 2 to 744,187.5 S/s for nrsc5, and feed the same baseband to the analog FM demod after its own decimation. The arithmetic is simpler, and there is more filter guard band.
- Clock accuracy: the 0.5 ppm TCXO on the NESDR SMArt v5 is about 50 Hz at 100 MHz, small compared with the 363 Hz subcarrier spacing. nrsc5 estimates and corrects frequency offset and sample-clock drift anyway.
- With 8-bit RTL I/Q, the digital sidebands sit 10–20 dB below the analog carrier (−20 to −10 dBc). AGC/gain must avoid clipping on strong analog carriers while keeping the sidebands above the quantisation floor. Fixed, well-chosen gain is usually better for nrsc5 than RTL AGC (not measured).
- AM:
  - nrsc5 AM needs 46,512 S/s baseband centred on the carrier, covering ±15 kHz for MA1.
  - RTL-SDR direct sampling (V4 or NESDR Q-branch) or the Airspy HF+ Discovery (native MW, about 660 kHz span) can supply this after decimation. A 744,187.5/16 = 46,511.7 S/s chain from a 1,488,375 S/s capture works, as does resampling from the HF+ native rate.
  - The HF+ Discovery is the better MW front end; 8-bit direct sampling has limited dynamic range on a band full of strong AM carriers.
- Hybrid with TEF6686: the TEF6686 does analog AM/FM including its own RDS. The RTL then only needs to run when the tuned station carries IBOC. The NO2CW FM-DX Webserver uses exactly this pattern — [rtl-sdr.com](https://www.rtl-sdr.com/adding-hd-radio-support-to-fm-dx-webservers-with-an-rtl-sdr/). The catch is that analog/digital blend then needs the TEF6686 audio (I2S/analog) and the nrsc5 audio aligned in one mixer, adding an extra clock domain to resolve.

### Gaps
- No measurement of RTL-SDR V4 versus NESDR SMArt v5 for NRSC-5 at 1.14 vs 1.488 MS/s.
- Not confirmed whether libnrsc5's pipe API accepts cf32 directly in v3.2.0 (the CLI does via `--iq-input-format`).

## 4. Legal / IP

### Takeaway
Xperi/iBiquity hold NRSC-5 patents, including receiver patents, and declare them available for licence under NRSC policy. the NRSC-5 audio codec is a trade secret, and the brand name and logo are trademarks. Commercial receivers pay royalties and buy licensed chips. nrsc5 has been public since 2017 with no takedown or lawsuit found, but no licence covers it. Godwit should keep nrsc5 as an optional, separately built GPL component, avoid trademarks, and not ship it in commercial hardware without legal review.

### Cited Findings
- In 2023 Xperi and iBiquity filed a patent disclosure letter with the NRSC listing patents they consider relevant to NRSC-5. It includes Attachment B, the "Receiver (Non-Transmission) Patent Portfolio", and confirms a licence is available under NRSC policy — [Xperi 2023 NRSC disclosure](https://www.nrscstandards.org/standards-and-guidelines/documents/standards/nrsc-5-e/Xperi2023.pdf)
- Xperi states its IBOC technologies are protected by copyrights, trade secrets and patents, and publishes a patent list — [Xperi patents page](https://xperi.com/hd-radio-patents)
- Receiver makers pay royalties, which is cited as a major reason the technology is not in all receivers. Broadcasters pay a one-time fee for the main channel, 3% of incremental net revenue for SPS, and LPFMs pay US$1,000/yr. the NRSC-5 audio codec is a trade secret and the specification is only partly open — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio)
- The the NRSC-5 audio codec codec was reverse engineered in June 2017 and found to be an HE-AACv1 variant. nrsc5 includes a patch to FAAD2 that is applied during the build — [search summary of Wikipedia the NRSC-5 audio codec article](https://en.wikipedia.org/wiki/High-Definition_Coding); [nrsc5 README](https://github.com/theori-io/nrsc5)
- The Theori authors framed nrsc5 as automotive security research. Their blog post does not discuss legal questions — [Theori blog](https://theori.io/blog/receiving-nrsc-5)
- An unverified forum opinion claims binary distributors could face HE-AAC patent-pool (MPEG LA, now Via LA) claims — [rtl-sdr.com comments](https://www.rtl-sdr.com/decoding-and-listening-to-hd-radio-nrsc-5-with-an-rtl-sdr/). This is not a legal finding.
- No DMCA takedown, lawsuit, or Xperi action against nrsc5 or its forks was found in searches. The repo has been public and maintained since 2017 — [search, no results](https://github.com/theori-io/nrsc5)
- The Si4689 datasheet carries a generic "no license ... is granted" disclaimer and describes the part as compatible with the iBiquity and NRSC-5 standards — [Skyworks Si4689 data short](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-shorts/Si4689-A10-short.pdf) (search snippet)

### Inferences
- GPL interaction with MIT Godwit:
  - Linking libnrsc5 into Godwit's MIT process would make the combined binary GPL-3.0.
  - Running nrsc5 (or a small GPL helper built on libnrsc5) as a separate executable that talks over pipes, sockets or D-Bus is the conventional "mere aggregation" boundary, the same pattern as Godwit's existing GPL librtlsdr helpers.
  - Godwit's MIT code must not include nrsc5 headers or copy its code.
  - Any I/Q-feeding helper that links libnrsc5 must itself be GPL-3.0 and published.
- Distribution posture:
  - Make NRSC-5 support an opt-in build or package. One option is to fetch and build nrsc5 from upstream at install time, rather than shipping prebuilt binaries in the default image.
  - Document that the codec is reverse engineered and that patent licences are not provided.
  - This limits the project's exposure. A company selling a Godwit-based commercial unit would need an Xperi receiver licence, typically obtained via a licensed chip such as the Si468x or SAF360x (see section 5).
- Avoid:
  - the brand name, logo and brand-style channel labels; use "Digital", "SPS 2", "Program 2" or the station-supplied SIS short names instead
  - "certified" claims
  - Xperi's audio-quality or "Artist Experience" brand terms
- This is not legal advice; the user should get counsel before any commercial product.

### Gaps
- Xperi's actual receiver licence terms and fees are not public in fetched sources.
- I found no authoritative legal analysis of non-commercial open-source NRSC-5 decoders.

## 5. Practical value in North America 2026 and on-chip alternatives

### Takeaway
About 2,100 US full-power FM stations (independent count, Aug 2025) to over 2,900 (Xperi claim) broadcast IBOC, roughly 18% of full-power FMs but concentrated in larger markets (97 of the top 100 stations). SPS channels are common on those stations. AM IBOC is marginal, and all-digital AM (MA3) is down to about one station. In a moving vehicle, IBOC coverage is smaller than analog, so smooth blending is essential. For a commercial product, licensed silicon (Si4689-class, NXP SAF360x + TEF668x) decodes NRSC-5 on-chip.

### Cited Findings
- Radio World's own database (15 Aug 2025) counted 2,114 active IBOC stations, about 18% of US full-power FM. The count excludes translators, LPFMs and boosters, and the database holds 21,980 FM stations. Xperi's own figure is higher and likely includes AM stations and licensed-but-inactive stations. 78% of adopting stations in 2025 are outside the top 50 markets — [Radio World analysis](https://www.radioworld.com/?p=131847) (via search summary)
- Xperi's fiscal 2025 10-K reportedly claims more than 2,900 stations, including 97 of the top 100 — [secondary blog](https://voxbooster.com/blog/hd-radio-digital-am-adoption-statistics-2026) (secondary and unverified; treat with caution)
- All-digital AM: one secondary source says only one AM station was still all-digital in June 2025 — [secondary blog](https://voxbooster.com/blog/hd-radio-digital-am-adoption-statistics-2026). Wikipedia lists five that have operated all-digital: WWFD (now WSHE, since June 2024), WMGG, WFAS, WSRO and WYDE — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio). Radio World has described AM IBOC as "stalled" — [Radio World](https://www.radioworld.com/tech-and-gear/am-hd-radio-has-stalled-now-what). Xperi still promotes all-digital AM — [Xperi all-digital AM page](https://hdradio.com/all-digital-am/). About 4,300 AM stations are licensed in the US — [secondary blog](https://voxbooster.com/blog/hd-radio-digital-am-adoption-statistics-2026)
- Coverage: NPR Labs (2008) found that raising digital power gained 17% more in-vehicle population coverage — [Wikipedia](https://en.wikipedia.org/wiki/HD_Radio). Receivers revert to analog when the digital MPS is lost. SPS channels have no analog fallback, so they mute on dropouts — [rtl-sdr.com](https://www.rtl-sdr.com/decoding-and-listening-to-hd-radio-nrsc-5-with-an-rtl-sdr/)
- Commercial receivers use strong acquisition hysteresis (need a strong signal to start decoding, then tolerate a big drop before falling back to analog) — [element14 review](https://community.element14.com/technologies/wireless/b/blog/posts/the-best-hd-radio-receiver)
- Si4689 (Skyworks, formerly Silicon Labs): an AM/FM/DAB receiver compatible with the iBiquity/NRSC-5 standards, covering 520 kHz–1.71 MHz, 76–108 MHz and 168–240 MHz in a 48-QFN 7×7 package — [Skyworks Si4689 product page](https://www.skyworksinc.com/en/Products/Audio-and-Radio/si4689); [DigiKey listing](https://www.digikey.ca/en/products/detail/skyworks-solutions-inc/SI4689-A10-GM/7201966). Skyworks also sells the Si461x automotive digital-radio coprocessors (e.g. Si4613) for IBOC — [Skyworks Si4613](https://www.skyworksinc.com/Products/Audio-and-Radio/Si461x-Automotive-Digital-Radio-Coprocessors/Si4613)
- NRSC-5 firmware and documentation for these chips is typically gated behind an Xperi licence or NDA. This could not be confirmed from fetched sources; the datasheet only has a generic IP disclaimer — [Si4689 data short](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-shorts/Si4689-A10-short.pdf)

### Inferences
- An RV that travels widely will often be in small markets with no IBOC at all. The feature mainly adds value near metros (SPS formats, album art, better audio). For Godwit, analog AM/FM with RDS remains the core feature, and NRSC-5 is an enhancement.
- AM IBOC (MA1) is present on a minority of AM stations, and MA3 is effectively absent. AM IBOC support is low priority. Defer MA3 entirely, and MA1 too unless the Airspy HF+ path is adopted anyway.
- The licensed-chip route (Si4689 or SAF360x/TEF668x) gives proper blend, sensitivity and legal cover, but needs an Xperi licence, NDA firmware and closed blobs. That conflicts with an open MIT/SDR design, so it is only sensible for a commercial derivative.

### Gaps
- No verified 2026 counts of IBOC stations or SPS channels; Canada and Mexico were not researched.
- No in-vehicle dropout statistics for SDR-based reception.
- NXP SAF360x licensing details and current availability were not fetched.

## 6. Recommended integration design for Godwit

### Takeaway
Run nrsc5 (libnrsc5) in a separate GPL helper process that receives baseband I/Q from Godwit's existing RTL capture. Option B: capture at 1,488,375 S/s, decimate by 2, push cf32/cs16 via the pipe API. The helper returns PCM, quality metrics and metadata over a socket. Godwit's MIT audio engine delays the analog FM audio, cross-correlates it with the digital audio to find the per-station offset, level-matches the two, and blends with hysteresis. Expose SPS channels as sub-presets with SIS names, show PSD text and LOT album art, and defer MA1/MA3, alerts UI and data services.

### Cited Findings
- nrsc5 supports stdin/pipe I/Q input, a C API, PSD/SIS/LOT/alert events, CRC and AGC status, and frequency-offset and service-mode reporting — [nrsc5 README](https://github.com/theori-io/nrsc5); [releases](https://github.com/theori-io/nrsc5/releases)
- Blend requires time alignment, which patents perform by envelope cross-correlation to about 3-sample accuracy. The offset is station-specific and can vary — [USRE48966](https://patents.google.com/patent/USRE48966); [US8180470](https://patents.justia.com/patent/8180470)
- The TEF6686 + RTL-SDR hybrid has been demonstrated in the FM-DX Webserver plugin — [rtl-sdr.com](https://www.rtl-sdr.com/adding-hd-radio-support-to-fm-dx-webservers-with-an-rtl-sdr/)

### Inferences (proposed design)
1. **Process boundary.**
   - `godwit-nrsc5d` is a GPL-3.0 helper linking libnrsc5, built from upstream at a pinned tag (v3.2.0+, with `-DUSE_NEON=ON` on ARM64).
   - Input: cf32 or cs16 I/Q at 744,187.5 S/s over a Unix socket or shared-memory ring from Godwit's SDR service.
   - Output: 44.1 kHz PCM per selected program, plus JSON events (sync, MER/BER, CRC errors, service list from SIS, PSD title/artist, LOT file paths, alerts).
   - Godwit stays MIT. The helper is an optional package.
2. **I/Q path.** Switch the FM capture to 1,488,375 S/s with the existing +285 kHz offset, mix to DC, and decimate by 2. The same 744 kS/s stream feeds both the analog FM demod and the helper. If 1.14 MS/s must stay, use a 3969/6080 polyphase resampler.
3. **Gating.** Only spin up decoding when (a) a cheap detector sees OFDM energy at ±129–198 kHz (sideband power vs adjacent noise), or (b) a cached "station has digital" flag is set. This saves CPU and power on the many stations without IBOC.
4. **Blend engine (MIT, in the Godwit audio service).**
   - Delay analog audio by a buffer of about 6 s.
   - On first digital lock, cross-correlate analog and MPS audio envelopes to get the per-station offset (expect about 3–5 s). Refine continuously and cache it per station.
   - Match levels with RMS tracking over several seconds, and optionally match EQ/stereo width with a gentle high-shelf.
   - Blend to digital only after N seconds of clean CRC. Blend to analog on rising CRC errors or lost sync, using about 0.5–1 s crossfades and hysteresis.
   - Because the analog is delayed, the user hears delayed analog until digital is ready. Alternatively, start with undelayed analog and accept one time jump at first blend, as some commercial receivers do. Make this a setting.
   - SPS have no analog fallback: show "signal lost" and mute or cut over rather than blend.
5. **Hybrid TEF6686 variant.** If analog comes from the TEF6686 over I2S, the blend engine must also handle clock drift between the two audio domains (asynchronous resampler) and the same cross-correlation alignment. This is more complex, which is a reason to prefer a single-SDR capture for stations with digital.
6. **UI.**
   - Present SPS as sub-channels of a frequency ("101.1 · 2"), labelled with the station's SIS short name and program type, never with trademarked channel labels.
   - Show PSD title/artist and LOT album art and station logo. Sanitise and size-limit LOT files: nrsc5 warns that its file dump is insecure, so write to a sandboxed dir with whitelisted extensions.
   - Show a "Digital" indicator and a signal-quality bar from MER/BER.
7. **Defer.** Defer the following:
   - AM MA1/MA3 decoding (MW via the Airspy HF+ path only, MA3 experimental upstream)
   - Emergency-alert UI and audio pre-emption (test thoroughly first)
   - HERE images, traffic and weather data services
   - Recording and time-shift
   - All-digital FM (MS modes; no known North American stations)
8. **Validation.**
   - Record I/Q from local IBOC stations with `-w`, and use the files for regression tests of the helper and blend engine.
   - Benchmark CPU on the Pi 5 and Dragon Q6A.
   - Drive-test dropout and blend behaviour against a commercial reference receiver.

### Gaps
- No field data for the blend design; tune its thresholds by drive testing.
- Not confirmed whether nrsc5's audio events expose enough timing information (sample counters) for precise A/V alignment. The cross-correlation approach does not depend on it.
