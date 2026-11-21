# Canadian RDS PI codes and call signs

Checked 2026-11-21 from primary sources.

## Takeaway
A Canadian call sign can be computed from most Canadian PI codes. ISED hosts a CBC-built calculator whose formula is deterministic and reversible over 0xC101–0xCEC3 (CF/CH/CI/CJ/CK calls, 3- and 4-letter). The CRTC has no rule on RDS or PI codes. Mexico has no published method.

## Cited findings
- **ISED BPR-3**, *Application Procedures and Rules for FM Broadcasting Undertakings* (Issue 7, May 2024; also Issue 6, Feb 2016): §2.3.1 says starting SCMO "including Radio Broadcast Data System (RBDS) operations" needs a certificate amendment. §3.13.2 requires RBDS stations to use the US NRSC RBDS standard and reserves 54.6–59.4 kHz for it. Neither issue mentions PI codes. — https://ised-isde.canada.ca/site/spectrum-management-telecommunications/en/node/82
- **ISED "Program information codes for radio broadcasting stations"** (updated Oct 26 2007, modified 2011-07-14), from the Technical Advisory Committee on Broadcasting (B-TAC):
  - each station must transmit a unique PI code;
  - Canada's NRSC-4 allocation is C000–CFFF (with exceptions), plus B_02–B_09 for CBC network stations;
  - the page acknowledges that some stations send off-format codes, encoder defaults or no PI.
  — https://ised-isde.canada.ca/site/spectrum-management-telecommunications/en/node/587
- **Calculator:** "RBDS PI Code Algorith v2.xls" (CBC-authored, Oct 2007), in https://ised-isde.canada.ca/site/spectrum-management-telecommunications/sites/default/files/attachments/2022/rbds-pi-code-algorithv2.zip
  - Forward: p = prefix (CF 0, CH 1, CI 2, CJ 3, CK 4; others "Not valid", CB not included); b = 3rd letter (A–Z = 0–25); c = 4th letter (blank = 0, A–Z = 1–26). n = p·702 + b·27 + c + 257; PI = 0xC000 + n + ⌊(n − 257)/255⌋. This counts up from 0xC101, skipping codes ending in 00: CFA = C101 … CKZZ = CEC3. The sheet's own sample rows (CJZZ = CC03, CKA = CC04, CKZZ = CEC3) match.
  - Reverse (valid for high byte 0xC1–0xCE, low byte ≠ 0, PI ≤ 0xCEC3): k = (hi − 0xC1)·255 + (lo − 1); p = k div 702; r = k mod 702; b = r div 27; c = r mod 27 (0 = 3-letter call).
  - Special cases: VOAR = CEC4, VOCM = CEC5. Low-power "VF####" stations: N mod 584 into a 584-entry table (CEC6–CEFF, CF01–CFFF, AC01–ACFF, AFC1–AFCF, AFAC); not reversible, and the A-range collides with US remaps.
  - Sheet notes: repeaters and nested stations reuse the main station's PI; stations with identical programming share one PI; C000, Cx00 and C0xx are normally reassigned into A-codes as in NRSC.
- **Verification:** the reverse formula round-trips all 3,510 call signs. Off-air Calgary captures (walczakp/rds-spy-logs, Canada folder, May 2019 — https://github.com/walczakp/rds-spy-logs/tree/master/Canada) agree: C185 CFEX ("NOW ON X929"), C37C CFXL, C456 CHFM, C95C CJAY, CB42 CJSW, CE23 CKUA, CD59 CKMP. C185 and CB42 re-checked by hand.
- **CBC networks** (ISED node/587 and NRSC-4-B Table D.7): B_02 CBC Radio One, B_03 CBC Radio Two (now CBC Music), B_04 Première Chaîne (now ICI Radio-Canada Première), B_05 Espace Musique (now ICI Musique), B_06–B_09 CBC reserved. "_" is a coverage-area nibble, 1–F. NRSC-4-B §D.7.3: no regional-variant PI search for 0xC000–0xCFFF or 0xF000–0xFFFF. (Plain-text extraction of Table D.7 shifts its columns; B_02–B_05 is correct.)
- **CRTC:** no rule, policy or decision on RDS, PI codes or call-sign display found on crtc.gc.ca.
- **Open-source decoders:** redsea (MIT) has US rules and the CBC B-codes but no Canadian C-range decoding. TEF6686_ESP32 (GPL-3.0) applies US arithmetic with a 'C' prefix, which fails for real C-range codes. FM-DX Webserver (GPL-3.0) looks transmitters up in an online database. None implements the ISED/CBC method.
- **Mexico (0xF___):** NRSC-4-B §D.7 only says Mexico uses 0xF; no IFT allocation method found.

## Inferences
- Implementing the reverse formula is a few lines and needs no table. Reimplement from the description; the spreadsheet itself has no stated open licence.
- Show the computed call only for C101–CEC3 with a non-zero low byte; otherwise fall back to PS (and later RT+ StationName.Short).

## Gaps
- No independent statement of who assigns or polices Canadian PI codes beyond self-derivation with the tool.
- How many Canadian stations actually follow the formula on air today (the Calgary sample is 2019).
