# Integrated Raspberry Pi (CM5 / Pi 5) panel PCs with two or more CAN interfaces

Research date: 2026-11-22. Scope: all-in-one touch panels (display + touch + enclosure) on CM5 / Pi 5 (CM4 flagged) with 2+ onboard CAN.

## Which integrated Pi/CM5 panels have 2+ CAN?

### Takeaway
The only integrated CM5 touch panel family I could confirm with two onboard CAN ports is the **EDATEC ED-HMI3630 series** (ED-HMI3632 / 3633 / 3634 sub-models, in 10.1", 11.6", 13.3" and 15.6"). It has 2 x isolated CAN 2.0B (not CAN FD) and a 9-36 V DC input. Every other vendor I checked (Chipsee, Waveshare, Seeed, PiLink, EDATEC's other HMI lines) has one CAN or none on its panels, or has dual CAN only on box PCs with no display.

### Cited Findings

**EDATEC ED-HMI3630 family (CM5) — has 2 x CAN**
- ED-HMI3630-156C is a "15.6-inch Industrial HMI Based on Raspberry Pi CM5". Its headline I/O is 1 x HDMI, 2 x LAN, 2 x USB 2.0, 4 x isolated RS232/485, 2 x isolated CAN, 8 x isolated DI and 8 x isolated DO — [EDATEC ED-HMI3630-156C datasheet, built 2025-07-09, via DigiKey](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- CAN detail (same in the 15.6" and 13.3" datasheets):
  - 2 x CAN ports on 6-pin 3.5 mm Phoenix terminals
  - Isolation protection 3 kV
  - "CAN Protocol: CAN 2.0B"
  - Baud rate 10-1000 kbps
  - Signals CANH, CANL, GND
  - CANB0-CANB1 status LEDs
  - Sources: [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf); [13.3" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-133C-10232-S.pdf)
- The sub-model number sets only the serial port mix: ED-HMI3632 = 2 x RS485 + 2 x RS232; ED-HMI3633 = 3 x RS485 + 1 x RS232; ED-HMI3634 = 4 x RS485. All of them list the 2 x CAN — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- 15.6" display: 1920x1080 TFT, 450 cd/m2, 1500:1 contrast, 85° viewing angles, capacitive touch (up to 5 points, USB-attached) — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- 13.3" model (ED-HMI3630-133C / ED-HMI3632-133C):
  - 1920x1080, up to 450 cd/m2, active area 293.76 x 165.24 mm
  - Same 2 x isolated CAN 2.0B and 9-36 V input as the 15.6"
  - 34 W max, 340.2 x 211.2 x 66.5 mm, about 2.2 kg
  - Source: [13.3" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-133C-10232-S.pdf)
- 10.1" model (ED-HMI3630-101C): 1280x800 capacitive touch on CM5, with 2 x isolated CAN supporting CAN 2.0B. It is listed at -20 to 60 °C operating. Caveats:
  - These figures come from a search-engine summary of the [Mouser-hosted datasheet](https://www.mouser.com/datasheet/2/1563/EDATEC_01_16_2025_ED_HMI3630_101C_datasheet_en-3541061.pdf). I could not open the PDF myself (fetch timed out).
  - The same summary said the CAN uses 6-pin 3.5 mm connectors with 120 Ohm termination. That came from the manuals.plus user manual, which returned 403 to me.
- An 11.6" member exists: a DigiKey-hosted datasheet named ED-HMI3632-116C-10432-S — [DigiKey PDF](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-116C-10432-S.pdf). I did not extract its specs.
- Power: "Wide voltage power input range of DC 9V~36V with reverse polarity protection, overvoltage protection, and overcurrent protection", on a 2-pin 3.5 mm Phoenix terminal. 15.6" draws 36 W max — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- Power-loss and RTC:
  - "Integrated supercapacitor (backup power supply, optional), RTC, Watch Dog, EEPROM, and crypto authentication"
  - The RTC has a 1 F supercap backup, plus an optional CR1220 battery base
  - The watchdog "has been reserved on hardware" and its software is "under development"
  - No ignition-sense input is listed
  - Source: [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- Storage:
  - CM5 eMMC 16/32/64 GB, RAM 2/4/8/16 GB
  - 1 x microSD
  - 1 x M.2 **B-key** 2230/2242 SSD, offered at 128/256/512 GB
  - The datasheet says "M.2 B port"; it does not say NVMe or SATA
  - Source: [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- USB: 2 x USB 2.0 Type-A, plus Micro USB for programming. HDMI 2.0 out (4K60) — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- Audio: 1 x 3.5 mm combo jack, "Audio In/Stereo Out … can be used as MIC IN and LINE OUT", plus a built-in PA and speaker (4 Ohm / 5 W per the extracted text) — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- Other I/O:
  - 1 x GbE + 1 x 100M Ethernet
  - Optional Wi-Fi 5 / BT and Mini PCIe 4G (Quectel EC20/EC25 variants, including EC25-AFX for North America with GNSS)
  - Nano SIM slot
  - Source: [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- Environmental (13.3" and 15.6"): operating -20 to 50 °C, storage -20 to 60 °C, 5-95 % RH non-condensing — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- Mounting:
  - "Embedded front installation", compatible with VESA 75x75
  - Kit of 4 buckles plus M4 screws
  - 15.6" is 388.3 x 238.3 x 66.5 mm, about 2.6 kg
  - No IP rating is stated in the datasheet
  - Source: [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)
- Software: Raspberry Pi OS Desktop/Lite, 32- and 64-bit; optional CODESYS Control for Raspberry Pi — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf). A community user reports the vendor's setup adds an EDATEC apt source and installs a firmware package — [balena forum](https://forums.balena.io/t/integrating-edatac-ed-hmi3630-with-balena/374302) (search-summary only).
- Price and stock (all reseller figures, unverified by me):
  - DigiKey Canada lists ED-HMI3632-101C-10832-S at CAD 994.37 with In-Stock 0 — [DigiKey CA](https://www.digikey.ca/en/products/detail/edatec/ED-HMI3632-101C-10832-S/25863650)
  - A search summary reported Mouser US at USD 528.60 for a 10.1" unit and buyzero.de at EUR 398.95 for the ED-HMI3634-101C — [buyzero](https://buyzero.de/en/products/edatec-ed-hmi3634-101c-industrial-hmi-cm5). I could not load either page (buyzero 429, Mouser timeout).
  - I found no 13.3", 15.6" or 11.6" pricing.

**Related EDATEC lines (no CAN on the panel)**
- ED-HMI3120 (CM5; 7", 10.1", 15.6"): coverage lists GbE + 100M Ethernet, isolated RS485/RS232 and wireless, but no CAN — [CNX Software](https://www.cnx-software.com/2025/05/04/edatec-launches-7-inch-10-1-inch-and-15-6-inch-industrial-hmi-displays-powered-by-raspberry-pi-cm5/)
- ED-HMI3020 (Pi 5, not CM5): M.2 NVMe, RS232, RS485, audio jacks; no CAN listed — [CNX Software](https://www.cnx-software.com/2024/01/18/raspberry-pi-5-industrial-hmi-displays-gain-m-2-nvme-ssd-rs232-rs485-interfaces-audio-input-output-jacks/); [Mouser](https://www.mouser.in/new/edatec/edatec-ed-hmi3020-xc-panel-pcs/)
- ED-IPC3630 is a CM5 **box** PC (no display) with 8 DI, 8 DO and 2 CAN. It appears to share the HMI3630 I/O board — [CNX Software](https://www.cnx-software.com/2025/03/24/edatec-ed-ipc3630-raspberry-pi-cm5-powered-industrial-pc-di-docan-bus/)

**Chipsee (confirm/correct)**
- PPC-CM5-156 (15.6", 1920x1080, 400 cd/m2):
  - CAN: "1 x CAN FD BUS, Arbitration Bit Rate up to 1Mbps, Data Bit Rate up to 8Mbps"
  - Power: 15-30 V
  - Storage and USB: M.2 M-key 2230-2280 PCIe Gen2 x1; 2 x USB 3.0 Type-A
  - Audio and RTC: 3.5 mm audio out plus 2 W speaker; RTC with farad cap
  - Lists "Automotive applications"
  - Source: [Chipsee PPC-CM5-156 datasheet via SVTronics](https://svtronics.com/wp-content/uploads/2025/04/PPC-CM5-156.pdf)
- Ordering: "Chipsee products can be customized during the ordering process". The examples given are OS/CM5/RAM/4G options; a second CAN is not offered as a standard option — [same datasheet](https://svtronics.com/wp-content/uploads/2025/04/PPC-CM5-156.pdf)
- PPC-CM5-070 / EPC-CM5-070 manual maps CAN_H/CAN_L to "CPU SPI0 … CAN0", one channel. This confirms an SPI CAN controller — [Chipsee manual](https://www.chipsee.com/docs/PCs/Pi/A76/Manuals/Hardware/EPC_PPC-CM5-070.pdf)
- PPC-CM5-050 has 1 x CAN FD — [Chipsee product page](https://chipsee.com/product/ppc-cm5-050/)
- PPC-CM5-150's pinout starts with a single CAN_H — [SVTronics PDF](https://svtronics.com/wp-content/uploads/2025/04/PPC-CM5-150.pdf) (search-snippet level)

**Waveshare (confirm)**
- CM5-HMI-xA-ACCE (7/8/10.1") panels: 7-36 V DC, 10-point touch. No CAN appeared in the search results — [Waveshare](https://www.waveshare.com/cm5-hmi-xa-acce.htm)
- CM5-HMI-BOX-A and IPCBOX-CM5 are box PCs with 1 x CAN (BOX-A per search summary) or none — [Waveshare docs CM5-HMI-BOX-A](https://docs.waveshare.com/CM5-HMI-BOX-A)
- No dual-CAN Waveshare panel was found.

**Seeed (confirm)**
- reTerminal DM is CM4-based, 10.1" 1280x800, IP65 front, 1 x CAN; USD 409 at launch — [Core Electronics](https://core-electronics.com.au/reterminal-dm-industrial-hmi-raspberry-pi-cm4-controller.html.plain.md); [CNX Software](https://www.cnx-software.com/2023/06/07/reterminal-dm-a-raspberry-pi-cm4-powered-10-1-inch-hmi-controller/)
- I found no CM5 reTerminal DM successor with dual CAN.

**PiLink PL-R5 / PL-R5M (not a panel)**
- These are CM5 industrial PCs in IP20/IP65 enclosures with **no integrated display**. The PL-R5 "DIO CAN" variant has 2 DI, 2 DO and 2 CAN. PL-R5 input is 9-40 V DC, operating -20 to +60 °C. No pricing is published — [CNX Software](https://www.cnx-software.com/2026/02/11/pilink-pl-r5-r5m-series-ip20-ip65-industrial-pcs-powered-by-raspberry-pi-cm5/)

### Inferences
- The CM5 (BCM2712/RP1) has no native CAN controller, so the ED-HMI3630's two CAN ports must come from an SPI CAN controller or a USB-CAN bridge. The datasheet says "CAN 2.0B, 10-1000 kbps" and nothing about FD, which suggests an MCP2515-class part, but this is unconfirmed.
- For Godwit, ED-HMI3630 fits both classic CAN at 500 kbit/s buses. Its 9-36 V input with reverse-polarity and overvoltage protection accepts a 12 V vehicle system directly, though no automotive load-dump (ISO 7637 / ISO 16750) rating is claimed.
- Its weak points against the head-unit requirements:
  - Only 2 x USB 2.0
  - -20 to 50/60 °C operating range
  - M.2 B-key rather than M-key NVMe
  - No stated IP rating or ignition sense
  - A heavy industrial I/O set (8 DI/8 DO, RS485) that is unneeded
  - Panel-cutout mounting
- The PiLink PL-R5 DIO CAN and EDATEC ED-IPC3630 show dual CAN is common in CM5 *box* PCs. A box PC plus a separate display (such as the Raspberry Pi Touch Display 2) is the main alternative architecture.

### Gaps
- ED-HMI3630 CAN controller chip, kernel driver, and device-tree overlay names; the EDATEC docs site was not reached.
- How the ED-HMI3630 display attaches (DSI or HDMI-internal) and its overlay name.
- Whether the ED-HMI3630 schematics are published. None were found; EDATEC publishes datasheets and manuals only, as far as seen.
- Front-panel IP rating for ED-HMI3630.
- Full 10.1" and 11.6" specs; the 10.1" PDF fetch timed out and the 11.6" PDF was not extracted.
- Verified current prices and stock, as of November 2026, for 13.3", 15.6" and 11.6" units.
- Comfile (ComfilePi), Industrial Shields Touchberry Pi, Sfera Labs, Kontron/Kunbus (Revolution Pi), Hilscher, Advantech, OnLogic, Distec/SECO: my searches returned nothing about CM5/Pi 5 panels with 2+ CAN from these vendors. That is not proof none exist.

## Do any vendors offer build-to-order / customization to add a second CAN?

### Takeaway
No vendor I found publicly offers a second CAN as an orderable option on a panel. Chipsee says products "can be customized during the ordering process" but lists only OS, CM5 and 4G-type options. EDATEC's HMI3630 already ships with 2 CAN as standard on every sub-model.

### Cited Findings
- Chipsee: "Chipsee products can be customized during the ordering process" and "If your project requires a different OS, please Contact us" — [PPC-CM5-156 datasheet](https://svtronics.com/wp-content/uploads/2025/04/PPC-CM5-156.pdf)
- EDATEC: every HMI3630 variant (3632/3633/3634) includes 2 x CAN; only the RS232/RS485 mix varies — [15.6" datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8564/ED-HMI3632-156C-10432-S.pdf)

### Inferences
- Chipsee's 156/070 boards route CAN through SPI0, so a custom second SPI CAN could be feasible, but that would need a direct engineering request to Chipsee.

### Gaps
- No public statement from Waveshare, Seeed or Chipsee on a dual-CAN custom build. Direct vendor inquiry is needed.
