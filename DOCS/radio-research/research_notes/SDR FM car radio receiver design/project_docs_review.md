# Project docs and design review: radio (TrailCurrent Godwit repo)

Source: a read-only review of the repository itself (no web sources). Paths are relative to the repo root; file:line references were taken from the files at commit 438e20b (upstream main, 2026-11-20).

## Summary
The repo describes and builds an FM-only radio. AM survives only as mockup demo data. FM is fixed to the North American band plan whatever Settings > Country is.

## 1. Design mockups vs code
- Mockups: DOCS/Design/Dual Encoder Infotainment System/ (v1 and v2 HTML). hmi/README.md:4 says the HMI is ported from v2.
- v1 has "Station Focus" and "Station List" layouts and a left-knob hold that toggles AM/FM (v1:690, v1:760). v2 shows band pill, genre, 5 signal bars, mute/paused pill, frequency, station name, radio text, 2x3 presets, tick dial (v2:136-164); demo AM stations and AM preset 6 (v2:517, v2:558); AM dial 530-1700 kHz in 10 kHz steps (v2:657). No NRSC-5 IBOC, AF/TA, clock or station list in v2.
- Mismatches:
  1. AM is a placeholder: RadioApp.qml:10-17 switches on band, but Hmi.qml:105 hard-codes band "FM" on the head unit; simulator keeps AM demo data (Hmi.qml:106-108, DemoData.js:58-62). No band switch in UI.
  2. v1's band toggle and Station List dropped without a note; v1 still in DOCS/ unmarked as superseded.
  3. Design says opening Radio makes it the audio source (v2:535); code only takes audio on tuning (Hmi.qml:1443-1444).
  4. Encoder label shows band only (Hmi.qml:2991) vs design "FM MHz".
  5. Additions beyond mockup: seek buttons, drag-to-tune, seek down, status texts, "Stereo"/"Mono" genre fallback.
  6. Knob-hold preset save uses a rotating slot radio.ptr (Hmi.qml:1737, 1749-1750); on the head unit ptr resets to 0 at each start (Hmi.qml:105) and is not saved, so the first knob-hold after each boot overwrites preset 1. Not documented (radio/README.md:34).

## 2. Documentation issues
1. librtlsdr licence contradiction: Legal.js:64 says GPL-2.0-only while Legal.js:65, THIRD_PARTY_LICENSES.md:79, radio/README.md:265 and godwit-radio.bb say GPL-2.0-or-later.
2. Comments say only Aircraft shares the dongle (radio.h:11, 35-37); Ships and Weather Radio do too. os/README.md:171 omits weather radio; 72-godwit-sdr.rules:1 comment omits it.
3. Supported dongles unclear: README says any RTL2832U + R820T/R820T2; udev rule only covers USB IDs 0bda:2832 and 0bda:2838 (72-godwit-sdr.rules:3-4). R828D (RTL-SDR Blog V4) and upconverter dongles not mentioned; AM note assumes all tuners stop at ~24 MHz. Always opens device 0 (main.cpp:299).
4. "Tuner not connected" also covers "no permission" (main.cpp:13, radio.cpp:213-214, Hmi.qml:1385); no troubleshooting for group membership or an already-loaded dvb_usb_rtl28xxu (godwit-sdr.conf only blacklists).
5. FM antenna guidance is one phrase (radio/README.md:22-23); other services have antenna sections.
6. Env vars GODWIT_RADIO=1, GODWIT_RADIO_BIN (and the other *_BIN, GODWIT_MIXER) only in code comments; not in hmi/README.md or simulator.md, so running the real tuner with the PC simulator is undocumented.
7. Shell test command (radio/README.md:252-257) does not say the HMI must not be holding the dongle.
8. Voice "AM is answered with a note" (hmi/README.md:231) only on the head unit (Hmi.qml:978).
9. Ducking undocumented: no WirePlumber ducking/role-priority config in os/meta-godwit (only 60-godwit-volume.conf, 62-godwit-mic.conf); relies on AGL's stock policy (mixer.h:10-14), unverified.
10. Left-knob hold = "Next Station" when radio is the source (Hmi.qml:1667, 2888) not in radio/README.md.
11. DSP design is only in radio/README.md:41-61; no design archive.
12. "About 20 seconds" scan time is not measured anywhere.

## 3. Design issues visible from docs/config
- Audio routing: radio, music and Weather Radio all on the Multimedia role (radio.cpp:222-225, weatherradio.cpp:244, library.cpp:471-475); guidance/assistant on Navigation; wake/dictation on Communication. Radio and Weather Radio share one volume/mute (Hmi.qml:87-89, 337-338).
- Phone calls and mute stop the tuner process rather than ducking (Hmi.qml:114); un-mute reopens the dongle and re-settles (documented hmi/README.md:235).
- One dongle, four users (radio, aircraft, ships, weather) coordinated via tunerHeld (Hmi.qml:114-117). Radio::seek is dropped silently while no helper exists (radio.cpp:122), e.g. while another app is still releasing the dongle (radio.cpp:171-172): a seek right after leaving Aircraft/Ships/Weather can be lost. Helper killed 1.5 s after stop (radio.cpp:256).
- Startup: audio source defaults to "local" (Hmi.qml:36) and last source isn't saved, so radio never resumes at power-on. If the first scan finds nothing (no antenna), presets stay empty and every later start silently rescans (~20 s) (radio.cpp:349-357, 385-388). PipeWire starts after HMI; pw-play failure -> EPIPE -> helper exits 0 -> retry every 2 s.
- Country/band plan: Country gates only Aircraft/Ships/Weather. FM always North American: band 87.9-107.9, 200 kHz (main.cpp:47-50), 75 us (fmreceiver.cpp:17), RBDS PTY table (radio.cpp:33-43), PI->K/W call signs for any PI 0x1000-0x994F (fmreceiver.cpp:290-305), and call sign preferred over PS (radio.cpp:405-409) -> invented call signs replace real names outside NA.
- Range disagreement: voice accepts FM 87.5-108 in 0.1 MHz steps (Intents.js:210-212, 228, 358); godwit-radio ignores tune outside 87.9-107.9 (main.cpp:341); Radio::setFrequency accepts anything (radio.cpp:104-118) -> "set the radio to 87.5/108" shows that frequency while the tuner stays put. Even-tenth frequencies like 88.0 pass through (hzOf rounds to 100 kHz, radio.cpp:21-24) and the knob then moves off the 200 kHz raster (Hmi.qml:1743 clamps but doesn't snap). AM ceiling 1710 in voice vs 1700 in knob/dial.
- LLM prompt shows only FM examples (llm.cpp:27-50).

## 4. Tests and CI
- radio/ has 4 ctest targets (fmreceiver, adsb, ais, wxradio), all passing locally (Qt-free, plain C++). fmreceiver_test covers pilot, blend, >30 dB separation, SNR >25 dB, RDS groups, call sign, PS, PTY, RT, mono (no RDS), noise, retune/discard.
- Not tested: main.cpp seek/scan/protocol (needs librtlsdr + dongle); the HMI radio side (no tests in hmi/CMakeLists.txt); Intents.js radio parsing; mono station with RDS (verified separately by our probe: works).
- No CI: no .github directory or workflows.

## 5. Our own probe results (2026-11-21, synthetic signals through the repo's fm::Receiver)
- Mono station with RDS decodes (PS and call) even with the 57 kHz carrier 3 Hz off; 80 groups at 0 Hz, 56 at 3 Hz offset.
- A loud, heavily clipped synthetic programme read 27.8 dB and got full stereo; the blend concern depends on real-air readings (README says 15-20 dB).
- De-emphasis error vs analogue 75 us: +0.11 dB at 5 kHz, +0.44 dB at 10 kHz, +1.01 dB at 15 kHz.
