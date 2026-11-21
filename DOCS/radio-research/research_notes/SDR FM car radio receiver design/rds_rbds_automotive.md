# RDS/RBDS Decoding and Automotive Features: Checking and Extending the Godwit Decoder

Scope: this note checks the existing Godwit decoder (radio/src/fmreceiver.cpp, hmi/src/radio.cpp) against the standards. It also ranks which RDS features to add for a car head unit, focusing on North America (US and Canada).

Primary sources used:
- NRSC-4-B (April 2011) text, extracted from the NRSC archive PDF: https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf (the file name says 2005, but the PDF metadata says April 27, 2011).
- NRSC-G300-C "RDS Usage Guideline", April 2018: https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf
- redsea source code: https://github.com/windytan/redsea

## 1. PI to call sign (NRSC-4-B rules, Canada, Mexico, non-call PIs)

### Takeaway
Godwit's K and W ranges are correct, but its PI decoding is incomplete, and the standards body advises against relying on it for display.
- **Missing PI forms:** Godwit does not handle the `A___` and `AF__` zero-nibble forms, the 3-letter table (0x9950–0x9EFF), or the B/D/E nationally-linked codes.
- **Misdecoded PIs:** Godwit will show a wrong call sign for TMC-substituted 0x1___ PIs and for NAB-assigned translator PIs.
- **No reverse for Canada or Mexico:** Canada uses 0xC___ and Mexico uses 0xF___. There is no published reverse formula for either.
- **Guidance:** NRSC-G300-C explicitly recommends that receivers *not* back-calculate the call sign from PI. Receivers should use RT+ StationName.Short instead.
- **Fix for Godwit:** The HMI preferring the computed call sign over PS should be reversed or demoted to a fallback.

### Cited Findings
- **Who uses call-derived PIs:** "In areas licensed by the U.S. FCC (except Guam), PI codes are calculated by the station's call letters." Stations in Canada and Mexico use PI codes starting with 0xC and 0xF, except certain CBC FM stations. — [NRSC-4-B §D.7](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)
- **Base formula:** Letters map A=0 … Z=25. VAL = L2×676 + L3×26 + L4.
  - K calls: PI = VAL + 4096 (KAAA=0x1000 … KZZZ=0x54A7).
  - W calls: PI = VAL + 21672 (WAAA=0x54A8 … WZZZ=0x994F).
  - 0x0000–0x0FFF is reserved.
  - Worked examples: KGTB → 0x21C7, WKTI → 0x7106.
  — [NRSC-4-B §D.7.1, Table D.7, §D.7.2](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)
- **Exception 1, second nibble zero:** If the call maps to `P1 0 P3 P4`, it is reassigned to `A P1 P3 P4`, for example 1045→A145 and 30F2→A3F2. The reason given is that European receivers treat a second nibble of 0 as a local station and won't switch to an AF. — NRSC-4-B §D.7.1
- **Exception 2, ending 00:** `P1 P2 0 0` becomes `A F P1 P2`, for example 1C00→AF1C and 3200→AF32. The nine cases 1000…9000 are double-mapped: 1000→A100→AFA1, …, 9000→A900→AFA9. — NRSC-4-B §D.7.1
- **Exception 3, simulcasts:** Stations that simulcast must share one PI, either call's mapping. The call letters can still be shown through PS. — NRSC-4-B §D.7.1
- **Exception 4, 3-letter calls:** These use a fixed table in 0x9950–0x9EFF (72 calls, e.g. KEX=9950, KFH=9951, WWJ=9988, KYW=996B, WBZ=996D, WGN=9972, WLS=997D). — NRSC-4-B Table D.7; full PI list as encoded in [redsea src/tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc)
- **Exception 5, nationally-linked networks:** These use B_01–B_FF, D_01–D_FF and E_01–E_FF. The second nibble is the coverage-area code and must be 1–F, since a 0 can stop receivers switching to AFs. NRSC-4-B notes that coverage-area codes are valid in North America only for the B, D and E blocks. — NRSC-4-B §D.7.1, §D.7.3
- **redsea's B-block table:** B_01 NPR-1, B_02 CBC Radio One, B_03 CBC Radio Two, B_04 Radio-Canada Première, B_05 Espace Musique, B_06–B_09 CBC, B_0A–B_0E NPR-2…NPR-6. redsea applies `pi &= 0xF0FF` before the lookup for B, D and E PIs. — [redsea tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc)
  - The CBC part is consistent with ISED: "B_02 to B_09 for CBC network stations". — [ISED PI codes page](https://ised-isde.canada.ca/site/spectrum-management-telecommunications/en/node/587)
- **redsea's `getCallsignFromPI` reverse order:**
  1. If `(pi&0xFFF0)==0xAFA0` and the low nibble is < 0xA → `pi <<= 12`, so AFA1→1000.
  2. Else if `(pi&0xFF00)==0xAF00` → `pi <<= 8`.
  3. Else if `(pi&0xF000)==0xA000` → `pi = ((pi&0x0F00)<<4)|(pi&0xFF)`.
  4. Then: 3-letter table for 0x9950–0x9EFF; B/D/E nationally-linked table; K/W arithmetic for 0x1000–0x994F.
  — [redsea tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc)
  - Licence: redsea is MIT, the same as Godwit, so this logic can be reused with attribution. — [redsea README](https://github.com/windytan/redsea)
  - The fetch summarizer flagged that the 3-letter table appears ordered by call sign while the code uses `std::lower_bound`. I did not verify this; check the table ordering before porting.
- **TMC substitution:** Broadcasters running TMC may substitute 0x1 as the first nibble of the PI. NRSC-4-B warns this "may also cause some receivers which use the PI code to determine the station call sign (by 'back calculation') to behave incorrectly."
  - ~16% of Western-US stations naturally have first nibble 0x1. No Eastern stations do.
  - Suggested heuristic: a first nibble of 0x1 together with the TMC ODA (group 8A) present means a substitution may have been made.
  — NRSC-4-B §D.7.4
- **G300-C on back-calculation:** There are three PI methods in US use: call conversion, TMC substitution, and NAB-assigned translator PIs. Consequently "the receiver cannot rely on PI codes to provide call letters for U.S. broadcast stations… it is recommended that receiver manufacturers decode the RadioText Plus (RT+) StationName.Short field in place of back-calculating the call sign using PI codes." — [NRSC-G300-C §5.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **Translator PIs:** PIs for FM translators that carry HD multicast or AM simulcasts are assigned by an NRSC algorithm and hosted by NAB at picodes.nrscstandards.org. They are unique per frequency/PI pair (for RadioDNS) and are not derived from the translator's call. — [NRSC-G300-C §5.1.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **Bad PIs on air:** G300-C reports multiple US cases where encoders were left at the factory-default PI. Two nearby stations then shared a PI, and some receivers used PI matching to "AF switch" with no AF list, jumping to a station with different programming. — [NRSC-G300-C §5.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **Canada:**
  - ISED lists Canadian ranges as "C000–CFFF (with certain exceptions)" plus B_02–B_09 for CBC.
  - Codes are generated with a CBC spreadsheet ("RBDS PI Code Algorith v1.xls") that converts a 3- or 4-letter call sign to hex. The page was updated 2007 and modified 2011.
  - ISED notes some stations use non-conforming codes, factory defaults, or no PI.
  - The page does not publish the algorithm.
  — [ISED: Program information codes for radio broadcasting stations](https://ised-isde.canada.ca/site/spectrum-management-telecommunications/en/node/587)
- **Current standard:** IEC 62106-9:2021 (the RBDS variant) consolidates and replaces NRSC-4-B. Its PI coding for North America is in Annex A. — search summaries of [iTeh IEC 62106-9:2021](https://standards.iteh.ai/catalog/standards/iec/5a445365-9c6f-436b-9277-bd0b5bf32b8c/iec-62106-9-2021) and the [VDE preview](https://www.vde-verlag.de/iec-normen/preview-pdf/info_iec62106-9{ed1.0}en.pdf). I did not read the full text.
- **PI search after a PI change:** NRSC-4-B §D.7.3 says that if the PI changes on a preset, the receiver may PI-search. If no identical PI is found, it should return to the original frequency and accept the new PI. The regional-variant search (PIs differing only in the second nibble) should be skipped for PI < 0xB000 and for 0xC000–0xCFFF and 0xF000–0xFFFF. — NRSC-4-B §D.7.3

### Inferences
- **Godwit corrections in priority order:**
  1. Demote the call sign to a fallback or secondary label. Prefer RT+ StationName.Short, then a stable PS, then the PI-derived call.
  2. Add A___/AF__ un-mapping, the 3-letter table and the B/D/E network table (redsea's logic is MIT and directly portable).
  3. Treat a raw PI in 0x1000–0x994F whose second nibble is 0 or which ends in 00 as "not a legitimate call mapping". Such stations should have been remapped to A___, so it is probably a non-conforming PI. Optionally suppress the call for 0x1___ when group 8A (TMC) is present.
  4. For 0xC___ (Canada), 0xF___ (Mexico), 0xB/D/E (networks) and anything else, show no call sign and rely on PS or RT+.
- **Station storage:** Save station names keyed by (frequency, PI), not frequency alone. When the car moves to a new market, the same frequency is a different station. A PI change on a stored frequency should invalidate the stored name, which matches the "accept the new PI" behaviour in §D.7.3.
- **Two-in-a-row PI acceptance:** This is a reasonable confirmation rule. PI is in block A of every group, about 11.4 times per second, so two consecutive matches cost under 0.2 s.

### Gaps
- **Canadian algorithm:** The CBC/ISED call-to-PI algorithm (how C, CF, CH, CI, CJ and CK prefixes are packed into C000–CFFF) was not available as text; it is only in the spreadsheet. Whether it is cleanly reversible is unconfirmed. redsea does not attempt Canadian reverse lookup.
- **Mexico:** Only "0xF prefix" is documented in NRSC-4-B. I found no allocation rules for XE/XH calls.
- **IEC 62106-9:2021 Annex A:** I could not confirm whether it changed any NRSC-4-B rules, for example the F0xx/Fx00 exclusions mentioned in an unofficial comparison ([qsl.net rbds_vs_ pdf](https://www.qsl.net/pjdyer/rbds_vs_.pdf), not read).

## 2. RBDS vs RDS PTY tables, and PTY 30/31 behaviour

### Takeaway
Godwit's blanks for PTY 24–28, 30 and 31 are wrong for North America. Under NRSC-4-B, 24 = Spanish Talk, 25 = Spanish Music, 26 = Hip Hop, 27–28 are unassigned (blank is correct), 29 = Weather, 30 = Emergency Test and 31 = Emergency. The European RDS table is entirely different from code 2 upward, so a region switch must select the table.

### Cited Findings
- **NRSC-4-B Table F.2 (North America), PTY name with 8-character display text in parentheses:**
  - 0 None, 1 News, 2 Information ("Inform"), 3 Sports, 4 Talk, 5 Rock
  - 6 Classic Rock ("Cls_Rock"), 7 Adult Hits ("Adlt_Hit"), 8 Soft Rock ("Soft_Rck"), 9 Top 40, 10 Country, 11 Oldies, 12 Soft, 13 Nostalgia ("Nostalga"), 14 Jazz, 15 Classical ("Classicl")
  - 16 Rhythm and Blues ("R_&_B"), 17 Soft R&B ("Soft_R&B"), 18 Foreign Language ("Language"), 19 Religious Music ("Rel_Musc"), 20 Religious Talk ("Rel_Talk"), 21 Personality ("Persnlty"), 22 Public, 23 College
  - 24 Spanish Talk ("Habl_Esp"), 25 Spanish Music ("Musc_Esp"), 26 Hip-Hop ("Hip hop"), 27–28 Unassigned
  - 29 Weather, 30 Emergency Test ("Test"), 31 Emergency ("ALERT !", 16-character "ALERT!_ALERT!")
  — [NRSC-4-B Annex F.2](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)
- **PTY 30:** Broadcast when testing emergency equipment. "Not intended for searching or dynamic switching for consumer receivers. Receivers may, if desired, display 'TEST'." — NRSC-4-B F.2
- **PTY 31:** An emergency announcement for "events causing danger of a general nature. Not to be used for searching – only used in a receiver for dynamic switching." — NRSC-4-B F.2
- **European RDS (EN 50067 Annex F) PTY names:**
  - 0 No PTY, 1 News, 2 Current affairs, 3 Information, 4 Sport, 5 Education, 6 Drama, 7 Culture
  - 8 Science, 9 Varied, 10 Pop, 11 Rock, 12 Easy listening, 13 Light classical, 14 Serious classical, 15 Other music
  - 16 Weather, 17 Finance, 18 Children's, 19 Social affairs, 20 Religion, 21 Phone-in, 22 Travel, 23 Leisure
  - 24 Jazz, 25 Country, 26 National music, 27 Oldies, 28 Folk, 29 Documentary, 30 Alarm test, 31 Alarm
  — [redsea tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc) (redsea keeps both tables, `pty_names` and `pty_names_rbds`, and leaves 27–28 empty in the RBDS table)
- **New codes on older radios:** Hip Hop, Spanish Talk and Spanish Music were added in NRSC-4-B. Older radios may show stations using them as "Undefined". The PTY list was based on Nielsen (Arbitron) formats. — [NRSC-G300-C §5.2](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **PTYN:** The NRSC believes PTYN (group 10A) is not commonly used in US receivers and recommends that stations not encode it. — [NRSC-G300-C §5.3](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **US alerting via RDS:** G300-C §8 covers emergency alerting over RDS: the ALERT FM system, an EAS ODA ("Annex Q"), and earthquake early warning via RDS. — [NRSC-G300-C §8, glossary](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)

### Inferences
- **PTY 31 handling:** On PTY 31 from the tuned station, show a prominent "ALERT" banner and raise or unmute the volume. Hold the alert state while PTY stays 31; a car should not be muted during an alert.
- **PTY 30 handling:** Show at most a "TEST" indicator and never auto-switch.
- **Tuning by PTY:** Do not auto-tune to other stations on PTY 31 unless EON or a dual-tuner scan is implemented. A wideband SDR could monitor PTY on every station within its captured bandwidth.
- **Region table:** Select the PTY table by region setting: RBDS for US, Canada and Mexico, RDS elsewhere. Do not infer it from the PI, since 0xC is also used for UK/Croatia and similar countries in Europe.

### Gaps
- Exact receiver requirements for PTY 31 (volume override, timeout) are not specified in NRSC-4-B beyond "dynamic switching".

## 3. Character set (G0 / Annex E table, RT control codes, Long PS, eRT)

### Takeaway
RDS uses its own 8-bit set (IEC 62106 Table E.1, the "basic character set"), not ASCII or Latin-1.
- **Where Godwit loses information:** Mapping only 0x20–0x7E and blanking the rest loses every accented character. This matters for Canadian French and US Spanish stations: é, è, ç, ñ, á and so on all sit in 0x80–0xFF.
- **Where Godwit differs on 0x20–0x7E:** It also differs from RDS at 0x24 (RDS ¤, while the RDS "$" is 0xAB), 0x5E, 0x60 and 0x7E.
- **Newer services:** Long PS (group 15A) and eRT use UTF-8.

### Cited Findings
- **PS and RT encoding:** Both are transmitted as 8-bit characters per the code tables in Annex E (Table E.1). eRT may use the extended set in Table E.2 (Annex Q, an ODA). — [NRSC-4-B §6.1.5.3, §6.2.2](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)
- **RT control codes:** 0x0D ends the message, 0x0A is a preferred line break, 0x0B is end-of-headline and 0x1F is a soft hyphen. Some RDS ICs don't support 0x0B and 0x1F. "A space shall be substituted by the receiver for any unrecognized symbol or control character." — NRSC-4-B §6.1.5.3
- **RT A/B flag:** A change in the A/B flag means clear the whole display. With no change, write new segments into the existing message. Do not mix 2A and 2B within one message. — NRSC-4-B §6.1.5.3
  - Godwit's A/B and 0x0D handling matches this.
- **Same table for RDS and RBDS:** Both share one character set, which is not ASCII, ISO-8859-1 or Unicode, though it has much in common with ISO-8859-1 in the lower 128. — [NRSC-G300-C §9.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **Dollar sign:** RDS puts "$" at 0xAB, and RDS 0x24 is the generic currency sign ¤. Many broadcasters wrongly send 0x24. G300-C recommends encoders translate 0x24 to 0xAB by default, and receiver makers should follow IEC 62106 Table E.1. — [NRSC-G300-C §9.2](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **Other 7-bit differences:** 0x5E, 0x60 and 0x7E differ from ASCII ^ ` ~ in RDS, per G300-C Table 11. — [NRSC-G300-C §9.3](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
  - From memory of IEC 62106, not verified in a fetched source: they are a horizontal bar, a double vertical bar and an overline.
- **Long PS (group 15A, IEC 62106-2:2018 onward):** Up to 32 bytes, UTF-8. The short PS must still be sent in 0A. Long PS may carry only a service name. — [RDS Forum glossary](https://rds.org.uk/pages/glossary-of-terms.html)
- **eRT:** Up to 128 bytes, UTF-8. — [RDS Forum glossary](https://rds.org.uk/pages/glossary-of-terms.html)
- **Standard editions:** IEC 62106-2:2021 replaced the 2018 edition and added RDS2 file transfer. — [iTeh IEC 62106-2:2021](https://standards.iteh.ai/catalog/standards/iec/bdec93e0-43f3-40db-91a9-062413fd0be7/iec-62106-2-2021)
- **Dynamic PS is common in the US:** "Use of a 'dynamic,' 'framing,' or 'scrolling' PS is a common practice within the U.S." G300-C recommends putting PAD (program-associated data such as song titles) in RT, but stations do scroll PS. — [NRSC-G300-C §6.9, §6.3](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)

### Inferences
- **Character table:** Port the full 256-entry E.1 table to UTF-8 strings. Keep 0x24 displayed as "$" (pragmatic, since most US stations misuse it) and map 0xAB → "$". Map E.1 high-half codes to their Latin letters so French and Spanish text displays.
- **PS assembly:** "Assemble PS only when all 4 segments arrive" is good for static PS. With scrolling PS, segments from different frames can mix.
  - Better: require each segment to be received twice identically, or all four segments within one PS cycle, before committing.
  - Treat PS as "dynamic" if it changes more than N times a minute.
  - Never store a dynamic PS as the station's saved name. Store RT+ StationName.Short, a stable PS, or the call sign instead.
- **Long PS and eRT:** These are Europe-led and low priority for North America (see section 4).

### Gaps
- I did not fetch the actual E.1 glyph table. Candidate sources: the IEC 62106-4 / EN 50067 Annex E table, or the tables in the redsea or gr-rds source (e.g. redsea's text/charset code). Prevalence of Long PS and eRT on North American stations: no data found.

## 4. Automotive features (AF, CT, TA/TP, EON, RT+, Long PS) and North American prevalence

### Takeaway
For a North American car, ranked by value:
1. Robust PS, RT and PI: done, but needs the fixes in sections 1–3.
2. RT+ (artist, title, StationName.Short).
3. PTY 31 alert handling.
4. AF method A with PI verification. US use is rare but real for translator and booster networks.
5. CT (group 4A), only as a display aid with sanity checks.
6. TP display. TA auto-switching is discouraged in the US.

EON, Long PS and eRT are Europe-oriented and low priority. In Europe, AF (methods A and B), TA/TP and EON are core car features.

### Cited Findings
- **AF in the US:** "AF has not been a widely used feature in the U.S." Its intended US use is fill-in translators, boosters and full simulcasts. — [NRSC-G300-C §5.7](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
  - "AF-compatible receivers should verify that the station on the alternate frequency is transmitting the identical PI code… and should not switch if the code is not the same."
  - "In the U.S., only method A is used. Method A can carry up to 25 alternative frequencies."
  - Stations should keep the simulcast delay under 1–2 seconds.
  - If AF is not used, the list should be empty.
- **AF code table:** Codes 1–204 = 87.6–107.9 MHz in 100 kHz steps. 205 = filler, 224 = no AF exists, 225–249 = "N AFs follow" (1–25), 250 = an LF/MF frequency follows. Region 2 MF codes 17–133 = 540–1700 kHz in 10 kHz steps. — [NRSC-4-B §6.2.1.6.1 Tables 10–12](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)
  - The extracted table showed AF code 205 as both "Filler code" and "Not assigned"; read 205 as Filler.
  - NRSC-4-B Table 10 still lists 87.6 MHz as code 1. Note that US FM spans 88.1–107.9 on odd 0.1 MHz steps.
- **Repetition rates:** AF is carried in 0A groups; group 0A/0B carries PI, PS, PTY, TP, AF, TA, DI and M/S at about 40% of group capacity. — NRSC-4-B Table 4
- **Same PI without an AF list:** Some receivers treat the same PI on another frequency as an AF even with no AF list. With factory-default PIs this caused unwanted switching. — [NRSC-G300-C §5.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **How car radios check AFs:**
  - Single-tuner radios do "muted PI checks". They briefly tune to the AF, read PI, and return, often with a "trust timer" so a recently verified AF need not be muted again.
  - Dual-tuner radios update the station list in the background "imperceptible to the end user". Without PI they may use audio correlation.
  - Pre-switch quality checks include field strength, multipath, adjacent-channel energy and frequency offset.
  — [US patent application 20110306313 (Justia)](https://patents.justia.com/patent/20110306313)
  - Consumer head units expose this as a "PI SOUND / PI MUTE" setting. — [Sony RDS help](https://helpguide.sony.net/ev/dsx-b41d/v1/en/contents/TP0002733672.html)
- **TA/TP:** Stations are encouraged to set TP if they air traffic reports, and receivers to display it. However, "automatic receiver switching using the Traffic Announcement (TA) flag is discouraged" because of listener complaints. — [NRSC-G300-C §5.4](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **CT (group 4A):**
  - Sent once a minute, as UTC plus a local offset in half-hours (±15.5 h), with the date as a 17-bit Modified Julian Day. The minute edge is within ±0.1 s of the end of the group. — [NRSC-4-B §6.1.5.6](https://www.nrscstandards.org/standards-and-guidelines/documents/archive/nrsc-4-b-standard-2005.pdf)
  - "Shall not be transmitted" unless accurate. — NRSC-4-B §6.1.5.6
  - G300-C recommends stations not send CT unless they keep it time-synced and DST-adjusted, and warns that wrong offsets "can cause issues with receivers". — [NRSC-G300-C §5.8](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
- **RT+:**
  - The NRSC recommends receivers decode RT+ StationName.Short instead of back-calculating call signs. — [G300-C §5.1](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
  - G300-C has a full section on RT+ for PAD (§6.10), and US alert providers and broadcasters use RT and RT+.
  - The RT+ spec is RDS Forum R08_008_3 (IEC 62106 Annex P). — [G300-C references](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)
  - RT+ content types include 1 item.title, 4 item.artist, 2 item.album, 31 stationname.short and 32 stationname.long. — [redsea tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc)
- **ODAs:** Each ODA is identified by an AID announced in group 3A. US AIDs are registered through NAB/NRSC and coordinated internationally. — NRSC-4-B Annex L
- **PS is not for tuning:** "The Program Service name is not intended to be used for automatic search tuning." — NRSC-4-B §7.12

### Inferences
- **Implementation order for Godwit:**
  1. Group 3A ODA AID parsing, then RT+ via the RT+ AID (0x4BD7 from memory, not verified in a fetched source), feeding the "now playing" artist and title and StationName.Short.
  2. PTY 30/31 handling.
  3. 0A AF list parsing (method A, and also method B for Europe) with PI-verified switching.
  4. 4A CT shown as station time, never used to set the system clock unless it agrees with GPS or RTC within a tolerance. CT is optional and sometimes wrong in the US.
  5. TP indicator only. A TA interrupt can be an opt-in feature mainly for Europe.
  6. 14A EON and 15A Long PS for Europe later.
- **SDR advantage for AF checking:** A wideband SDR capturing several MHz can demodulate RDS on several channels at once. That gives "dual-tuner" background AF and PI qualification with no mute and no tuner retune. Only AFs inside the captured span qualify; others still need a muted check. The receiver can also build a band-wide PI/PS station list continuously, which helps NA presets survive market changes.

### Gaps
- No quantitative recent data on the share of North American stations sending RT+, AF, CT or TP. G300-C gives qualitative statements only.
- I found no published North American statistics on Long PS or eRT usage.

## 5. Error correction, sync rules, and demodulation best practice

### Takeaway
redsea, a widely used open-source reference decoder, uses a conservative FEC: it corrects only single-bit and 2-bit adjacent bursts (not the full 5-bit burst capability), and only on blocks in expected position after sync. That is a sensible model for Godwit.
- **redsea sync and timing:** Sync needs three syndrome-valid blocks in a consistent cyclic A/B/C(C′)/D sequence, each up to 6 blocks apart. Sync is dropped when more than 42 of the last 50 blocks are in error. Timing uses an RRC-matched polyphase symbol synchronizer (liquid-dsp symsync) and a decision-directed PLL rather than picking the max-energy phase.
- **Godwit gaps:** "Must pass syndrome exactly" plus 8-phase max-energy timing is the main sensitivity gap relative to redsea.

### Cited Findings
- **redsea sync acquisition:** While unsynced, the syndrome is checked at every bit. Sync requires three candidate blocks whose spacing is a multiple of 26 bits, at most 6 blocks apart, with consistent block numbering (A=1, B=2, C/C′=3, D=4). — [redsea block_sync.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/block_sync.cc)
- **redsea loss of sync:** In sync, blocks are checked against the expected offset. Sync is lost when the error sum exceeds 42 over the window (constant `kMaxTolerableBLER` = 85%, named as "over 50 blocks"). — [redsea block_sync.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/block_sync.cc)
- **redsea FEC:** Applied only to errored blocks when enabled. The syndrome is looked up in a precomputed table of error patterns 0b1 and 0b11 at all 26 shifts, built per expected offset word. The maximum corrected burst is 2 bits. A version-B group sets the expected offset to C′ before correction. — [redsea block_sync.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/block_sync.cc)
- **Offset syndromes:** A 0b1111011000, B 0b1111010100, C 0b1001011100, C′ 0b1111001100, D 0b1001011000. — [redsea block_sync.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/block_sync.cc)
- **redsea demodulation chain:**
  - Resample to about 171 kHz, then mix down with a 57 kHz NCO.
  - 255-tap FIR low-pass at 2.4 kHz, decimate by 24, then AGC.
  - liquid-dsp `symsync` with an RRC filter (β=0.8, 32 polyphase filters, 3 samples per symbol at 2375 Hz).
  - PLL driven by the modem's phase error, which is decision-directed and Costas-like.
  - Biphase decode as the difference of consecutive PSK symbols. Clock polarity is chosen every 128 symbols by comparing even and odd |re| sums.
  - Then differential decoding. Per-bit EVM is tracked.
  — [redsea src/dsp/subcarrier.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/dsp/subcarrier.cc)
- **Pilot-locked subcarrier:** The 57 kHz subcarrier is phase-locked to the 19 kHz pilot. G300-C shows an oscilloscope view of "a synchronized RDS subcarrier in quadrature with the pilot". The guideline recommends 6% RDS injection (4.5 kHz of 75 kHz deviation). — [NRSC-G300-C Fig. 3 caption, §3](https://www.nrscstandards.org/standards-and-guidelines/documents/guidelines/g300-c.pdf)

### Inferences
- **Carrier recovery:** Godwit's squaring plus 0.2 s averaging is functional. Two better options:
  - Derive the 57 kHz reference from the 19 kHz pilot PLL (×3), which needs no extra loop and works at low RDS SNR. A small phase-offset correction is needed because the quadrature relationship is fixed by the standard.
  - Use a decision-directed Costas loop after matched filtering, as redsea does. Squaring has a 180° ambiguity, but differential coding removes it in either case.
- **Symbol timing:** Replace the 8-phase max-energy pick with a matched filter for the biphase symbol plus a continuous timing loop (Gardner or ML/derivative-MF polyphase, as in liquid `symsync`). This avoids quantised timing error and tracks clock drift.
- **Lowest-risk FEC path:**
  1. Copy redsea's 1–2-bit burst correction, applied only when in sync and only for the expected offset.
  2. Optionally add soft-decision (EVM) gating.
  3. Keep the "PI twice" and per-segment repeat checks, because FEC raises the undetected-error rate.
  - Full 5-bit burst correction is possible with the (26,16) shortened cyclic code but increases miscorrection risk. redsea chose not to use it.
- **Licences:** redsea is MIT, so code reuse is compatible with Godwit's MIT licence with attribution. gr-rds is GPL-3.0+ (mixed GPL-2+ files) and is not reusable in MIT code; consult it only for ideas.

### Gaps
- I found no published measurement of the dB or BLER sensitivity gain from redsea's burst FEC, or from 5-bit correction versus none. The redsea wiki benchmarks were not fetched.
- I did not fetch the EN 50067 / IEC 62106 Annex C text, which has the informative sync and error-correction recommendations. The sync rules above are redsea's, not quoted from the standard.

## 6. Reference decoders: features and licences

### Takeaway
redsea (MIT) is the best reference for Godwit: it has the most complete group and feature coverage among open decoders and a compatible licence. gr-rds is GPL-3.0 and focused on GNU Radio flowgraphs and TMC. I could not verify SDR++ and SDRangel RDS features in this pass.

### Cited Findings
- **redsea:** MIT-licensed, by Oona Räisänen. Outputs JSON per group with PI, group type, PS, PTY, TP/TA, dynamic PTY, music flag, RT, RT+ tags and ODA identification. Inputs are MPX PCM, hex logs or audio files. It "is not safety certified" for emergency use. — [redsea README](https://github.com/windytan/redsea)
  - Its tables include the RDS and RBDS PTY tables, country codes, languages, ODA names, RT+ content types, DI codes and PI→call decoding. — [redsea tables.cc](https://raw.githubusercontent.com/windytan/redsea/master/src/tables.cc)
- **gr-rds:** By Bastian Bloessl. "FM RDS/TMC Transceiver". GPL-3+ main code with some GPL-2+ files. Includes a decoder block and a parser with TMC event tables. The default branch is maint-3.10, last pushed Feb 2025. — [Debian copyright](https://sources.debian.org/src/gr-rds/3.10-2/debian/copyright); [GitHub bastibl/gr-rds](https://github.com/bastibl/gr-rds)

### Inferences
- **SDR++ and SDRangel:** Both are believed to be GPL-3.0 projects with RDS decoding for PS, RT, PTY and PI; SDRangel reportedly also handles AF and CT display. This is unverified, and in any case they are not MIT-compatible for direct code copying.

### Gaps
- SDR++ RDS decoder feature list (which groups, FEC, AF), SDRangel RDS feature list, and their exact licence files were not checked in this session.
- redsea wiki feature list (AF, CT, EON, Long PS and eRT support) was not fetched. The README does not mention them, but its tables suggest broader coverage.
