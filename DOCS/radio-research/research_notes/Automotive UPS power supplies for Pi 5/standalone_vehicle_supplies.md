# Stand-alone vehicle power supplies (non-HAT) for a Pi 5 / CM5 head unit

Scope: stand-alone automotive DC-DC converters, car-PC intelligent supplies, UPS modules, supercapacitor hold-up and reference front-end designs that could feed a 5 V / 5 A+ rail (or a stabilized 12 V rail for 7-36 V / 9-36 V CM5 carriers) from a 12 V (ideally also 24 V) vehicle supply. Research date: November 2026. Tool budget was limited (about 17 calls), so several items are marked as gaps.

## Which products exist, and what are their specifications?

### Takeaway
Only the Mini-Box family (DCDC-USB, DCDC-USB-200, M4-ATX, OpenUPS2) combines ignition sense, programmable off-delay, hard-off timer and low-battery cut-off in one off-the-shelf box. The DCDC-USB-200 is still sold ($89), takes 6-34 V, can be set to a 5 V output, and its datasheet claims spread-spectrum operation. But it has no built-in hold-up: it is ignition logic plus a buck-boost converter, not a battery or supercap UPS. Industrial and railway bricks (Mean Well RSD-30G-5) and hobby regulators (Pololu D36V50F5) give clean, well-documented 5 V at 5.5-6 A with 9-36 V or 5.5-50 V input, but have no ignition or hold-up logic.

### Cited Findings

**Mini-Box DCDC-USB-200 (active; the strongest off-the-shelf "ignition-aware" candidate)**
- Input 6-34 V, fused and TVS-protected. Output defaults to 12 V; jumpers select 5, 6, 9, 12, 13.5, 18 or 24 V, and USB sets 5-24 V in 0.25 V steps. Rated up to 15 A / 160 W. Standby current is under 1 mA in Automotive mode. — [Mini-Box DCDC-USB-200](https://www.mini-box.com/DCDC-USB-200)
- The manual (v1.0c) says "Emissions are reduced using 2 out of sync, spread spectrum buck-boost converters". The manual's intro says the output ranges from 6 to 24 V, but its jumper table includes 5 V, so the 5 V setting appears in one place and not the other. No switching frequency or CISPR 25 figure is published. — [DCDC-USB-200 manual (CarTFT)](https://www.cartft.com/support_db/support_files/DCDC-USB-200-manual.pdf)
- Modes: Dumb, Automotive and Script (via USB). Automotive mode reads the ignition wire and sends ON/OFF pulses to a motherboard's power-switch header (J8). J10 gives an unregulated switched input for peripherals. — [DCDC-USB-200 manual](https://www.cartft.com/support_db/support_files/DCDC-USB-200-manual.pdf)
- Ignition timing presets (jumper-selected OFFDELAY / HARDOFF): 0/0, 5 s/60 s, 5 s/never, 1 min/1 min, 15 min/1 min, 15 min/never, 30 min/1 min, 2 h/never. OFFDELAY is the wait from ignition-off to the shutdown pulse. HARDOFF is how long power stays on after the pulse. During HARDOFF the unit cuts power if the battery falls below 11.2 V. — [DCDC-USB-200 manual](https://www.cartft.com/support_db/support_files/DCDC-USB-200-manual.pdf)
- Electrical specs: minimum input 6 V. Electronic shutdown at 34 V, with clamping at 34-36 V. Deep-discharge threshold 11.2 V. Input current limit 15 A (20 A fuse). Regulation accuracy 2.5%. Efficiency above 94% at 12 V / 2 A output (9-16 V input). All solid-polymer capacitors. MTBF above 100k h. The 12 V output current table runs from 6 A at 6 V input up to 12 A (15 A peak) at 11-18 V input, and 10 A at 20-32 V input. Above 60 W or 65 °C it needs forced air. — [DCDC-USB-200 manual](https://www.cartft.com/support_db/support_files/DCDC-USB-200-manual.pdf)
- The same manual gives two operating-temperature figures: "-40 to +85 °C" in the spec table and "-20 to 65 °C" under operating environment. — [DCDC-USB-200 manual](https://www.cartft.com/support_db/support_files/DCDC-USB-200-manual.pdf)
- Open source: Linux drivers and source are at github.com/mini-box/dcdc-usb (license not stated on the product page), and community Python bindings exist (jackjackandrews/MiniBoxDCDC). A Go library, go-dcdc200, is also published. — [Mini-Box DCDC-USB-200](https://www.mini-box.com/DCDC-USB-200); [pkg.go.dev go-dcdc200](https://beta.pkg.go.dev/github.com/Fishwaldo/go-dcdc200@v0.0.5)
- Price $89.00. The page says "Usually ships in 24h" and "extended availability for the next 5 years" (dated 10/10/2020). — [Mini-Box DC-DC category](https://www.mini-box.com/DC-DC); [Mini-Box DCDC-USB-200](https://www.mini-box.com/DCDC-USB-200)
- A reseller lists it as "up to 150W continuous, 180W peak", which differs from Mini-Box's own 160 W. — [mini-itx.com](https://mini-itx.com/~DCDC-USB-200)

**Mini-Box DCDC-USB (original, 100 W)**
- Input 6-34 V, fused and TVS-protected. Output 5-24 V via jumpers or USB. Up to 10 A / 100 W. Automotive, UPS, Script and Dumb modes. Remote ON/OFF switching up to 6 A (8 A peak). Standby "well under 1mA". Solid-polymer capacitors. Linux source at github.com/mini-box/dcdc-usb. No price, switching frequency or temperature range is listed. "Usually ships in 24h." — [Mini-Box DCDC-USB](https://www.mini-box.com/DCDC-USB)

**Mini-Box OpenUPS2 (active; true battery UPS, but 12-24 V output only)**
- Input 11-24 V. Programmable output 12-24 V at 5 A (default 12 V). Uses 3x 18650 LiFePO4 cells (not included); Li-ion needs other firmware. Charges at 1 A (0.3 A precharge). Per-cell temperature monitoring. Sends an ON/OFF pulse to the motherboard for graceful shutdown on low battery. Standby 1 µA in deep sleep and 530 µA in sleep. Linux console tools are on GitHub, with LiFePO4/Li-ion firmware and a developer API also offered. Neither an ignition input nor a switching frequency is stated on the page. — [Mini-Box OpenUPS2](https://www.mini-box.com/OpenUPS2)
- Price $119.00. — [Mini-Box NUC-UPS / product listing via search](https://www.mini-box.com/NUC-UPS); [Mini-Box OpenUPS2](https://www.mini-box.com/OpenUPS2)
- The NUC-UPS (a related Mini-Box product) is marked End Of Life. — [Mini-Box NUC-UPS](https://www.mini-box.com/NUC-UPS)

**Mini-Box M4-ATX / M4-ATX-HV, DCDC-NUC (car-PC ATX supplies)**
- M4-ATX: 250 W (300 W peak), 6-30 V input, "Intelligent Automotive DC-DC Car PC Power Supply", $89, "Usually ships in 24h". M4-ATX-HV: 220 W, 6-34 V. — [Mini-Box M4-ATX](https://www.mini-box.com/M4-ATX); [Mini-Box M4-ATX-HV](https://www.mini-box.com/M4-ATX-HV); [Mini-Box DC-DC category](https://www.mini-box.com/DC-DC)
- DCDC-NUC: 6-48 V automotive supply with 12 V or 19 V output. Its wiki describes the same ignition-off, motherboard-pulse and HARD_OFF sequence. — [Mini-Box DCDC-NUC](https://www.mini-box.com/DCDC-NUC); [Mini-Box wiki DCDC-NUC](https://wiki.mini-box.com/index.php/DCDC-NUC)

**Mean Well RSD-30G-5 / RSD-30H-5 (railway DC-DC, active)**
- RSD-30G: 9-36 VDC continuous input; 5 V at 6 A rated (0-6 A); ripple and noise 70 mVp-p max; typical input 1.5 A at 24 V. Overvoltage protection on the 5 V model trips at 5.75-7 V. Overload goes to constant-current limiting with auto-recovery. — [Mean Well RSD-30 spec](https://www.meanwell.com/Upload/PDF/RSD-30/RSD-30-SPEC.PDF)
- Switching frequency (block diagram): fosc 110 kHz. The input can withstand about 140% of rated input voltage for 1 s. Inrush is limited by a resistor that a MOSFET then bypasses. — [Mean Well RSD-30 spec](https://www.meanwell.com/Upload/PDF/RSD-30/RSD-30-SPEC.PDF)
- Standards: EN 50155:2021 (S1 interruption level), IEC 60571, IEC 61373 shock and vibration, EN 50121-3-2 EMC, EN 45545-2 fire. EMC emission is EN 55032 conducted Class A and radiated Class B. No ISO 7637-2 or CISPR 25 claim was found. — [Mean Well RSD-30 spec](https://www.meanwell.com/Upload/PDF/RSD-30/RSD-30-SPEC.PDF)
- Working temperature -40 to +55 °C with no derating; at +70 °C, 60% load with free-air convection, or full load with an external base plate. — [Mean Well RSD-30 spec](https://www.meanwell.com/Upload/PDF/RSD-30/RSD-30-SPEC.PDF)
- DDR-30 (DIN rail) input ranges: G 9-36 V and L 18-75 V. A Mean Well flyer lists EN 50155 only for RSD-30~300 and DDR-120~480, which suggests the DDR-30 itself is not EN 50155-rated. I found no switching frequency for the DDR-30. — [Mean Well converter flyer](https://www.meanwell.com/converter/en/data/Flyer-Converter_en.pdf); [Mean Well catalog](https://www.meanwell.com/Upload/PDF/catalog_s.pdf)

**Pololu D36V50F5 (5 V 5.5 A buck module, active)**
- Input 5.5-50 V; typical maximum 5.5 A at 36 V input (3.5-8 A depending on input voltage and thermal limits). Integrated reverse-voltage protection to 40 V. About 500 kHz at heavy load; power-save mode at light load stays above 20 kHz. Quiescent current 2-4 mA; with EN low, about 10-20 µA per volt of VIN. The EN pin's precise threshold (1.2 / 1.35 V) allows a resistor-divider low-voltage cut-off. Has a power-good output, OVP/UVP, short-circuit and over-temperature protection, and soft-start. Accuracy 4%, efficiency 80-95%. Size 25.4 x 25.4 x 9.5 mm. $44.95; stock is "Rationed (Active and Preferred)". — [Pololu D36V50F5](https://www.pololu.com/product/4091)
- Pololu makes no automotive transient (ISO 7637/16750) or EMI claim on the page. — [Pololu D36V50F5](https://www.pololu.com/product/4091)

**Victron Orion-Tr (isolated DC-DC; no 5 V model found)**
- The Orion-Tr 12/12-9 (110 W) takes 8-17 V input; the 24/12-9 takes 16-35 V; 200 V DC galvanic isolation. A remote on/off port can be driven by a low-power switch or the engine run/stop switch. Automatic engine-run detection is a feature of the Orion-Tr Smart charger range, not the plain converter. — [Victron Orion-Tr isolated datasheet (FR)](https://www.victronenergy.com/upload/documents/Datasheet-Orion-Tr-DC-DC-converters-isolated-100-250-400W-FR.pdf); [Victron Orion-Tr Smart specs](https://www.victronenergy.com/media/pg/Orion-Tr_Smart_DC-DC_Charger_-_Isolated/en/specifications.html) (secondary summary via search; full datasheet not read)

**Daygreen / Tobsun generic 12-24 V to 5 V converters**
- The only Daygreen document found is a 12/24 V to 5 V 10 A / 50 W non-isolated converter with 10-30 V input. It gives no switching frequency, transient or EMI data. — [Daygreen datasheet PDF](https://cdn.shopify.com/s/files/1/1598/0223/files/12V_24V_to_5V_10A_50W_DC-DC_CONVERTER_NON-ISOLATED.pdf)
- No Tobsun datasheet was found. Similar potted 5 V / 5 A (25 W) "waterproof" units on marketplaces list 8-32 V input and IP67, but no switching frequency. — (search summary only; marketplace listings, low reliability)

**Dashcam hardwire / power-management kits**
- The BlackVue Power Magic Pro (ignition and battery-voltage cut-off, 6 h minimum timer, switch-selected cut-off such as 12.0 / 12.5 V) is **discontinued**; newer BlackVue X-series cameras build the cut-off in. — [BlackboxMyCar Power Magic Pro](https://www.blackboxmycar.com/pages/everything-you-need-to-know-about-power-magic-pro); [BlackVue PMP manual](https://images.carid.com/blackvue/items/pdf/rvs-pmp-manual.pdf)
- BlackVue advises not setting the cut-off below 12.0 V, and 12.2-12.5 V in cold climates. — [BlackVue forum: low-voltage cutoff](https://forum.blackvue.com/hc/en-us/articles/49160733840921-Battery-Protection-Features-Low-Voltage-Cutoff)
- Generic 12/24 V to 5 V 3 A USB-C hardwire kits offer selectable cut-offs (for example 11.6 / 12.0 / 12.4 V and 23.6 / 24.0 / 24.4 V). They are rated about 3 A, below the 5 A target. — (search summary of marketplace listings; low reliability)

**DIY / hobby Raspberry Pi ignition controllers**
- PicoTouch "Raspberry Pi automatic on/off" (12 V to 5 V with ignition-off delay, then a shutdown script, then cut-off about 45 s later) is **no longer sold**. — [Tindie PicoTouch](https://www.tindie.com/products/picotouch/raspberry-pi-automatic-onoff-v30-new-version/)
- Crankshaft wiki: an ignition-driven relay plus a shutdown script (default 5 s delay). — [Crankshaft wiki](https://github.com/opencardev/crankshaft/wiki/Boot,-reboot-and-shutdown-the-Pi-with-ignition-key)

**Supercapacitor hold-up**
- The Analog Devices LTC3350 is a 25 W-class backup boost controller with a buck supercapacitor charger. It charges, balances and monitors 1-4 series supercaps and targets 12 V ride-through UPS use. Its demo board DC1937B is labeled "12V NOMINAL 25W". The LTC3351 (hot-swap variant) has a DC2464A-A demo board (12 V nominal input, 6 V 25 W backup). — [ADI LTC3350](https://www.analog.com/en/products/ltc3350.html); [ADI DC1937B](https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc1937b.html); [DC1937B schematic](https://www.analog.com/media/en/technical-documentation/eval-board-schematic/DC1937B-3-SCH.PDF); [ADI LTC3351](https://www.analog.com/en/products/power-management/supercapacitor-chargers/ltc3351.html)
- The Vishay EDLCBU-12V reference design (LTC3350IUHF plus 6 parallel strings of 4 series EDLC cells) is a "12V ride-through UPS". Design files and a datasheet are downloadable (dated 29 Jul 2026), but the board itself is not sold, and no hold-up time or power figure is on the landing page. — [Vishay EDLCBU-12V](https://www.vishay.com/en/landingpage/reference-designs/EDLCBU-12V/)

### Inferences
- The DCDC-USB-200 set to 5 V is the only stand-alone box found with ignition sense, programmable off-delay and hard-off, and an 11.2 V low-battery cut-off. Its shutdown pulse is meant for an ATX power-switch header. On a Pi 5 or CM5 it would go to the PWR_BUT / power-button pads (or a GPIO via an opto), which is a wiring task, not a product feature.
- A 5 V output from a 160 W buck-boost is likely fine for 25 W, but Mini-Box publishes no 5 V current table. Its 12 V table caps at 12-15 A, so 5 A at 5 V should be within limits. This is unverified.
- Setting the DCDC-USB-200 or M4-ATX to 12 V and feeding a 9-36 V CM5 carrier is the most documented route if the carrier is chosen. A Pi 5 would then need a second 5 V buck stage.
- Switching noise: the Mean Well RSD-30's 110 kHz fundamental puts harmonics at 550, 660, 770 kHz and so on every 110 kHz across the whole AM broadcast band (520-1710 kHz), at fixed frequencies with no spread spectrum stated. The Pololu D36V50F5's roughly 500 kHz puts its 2nd and 3rd harmonics at about 1000 and 1500 kHz, inside the AM band, and its frequency drifts with light-load power-save mode. Both would need shielding, filtering and placement checks for AM reception. Spread spectrum (claimed on the DCDC-USB-200) spreads the harmonics but raises the wideband noise floor, which can be audible on AM.

### Gaps
- No product found publishes a CISPR 25 class or a conducted or radiated emissions plot, except the TI reference designs listed in the next section. Mini-Box gives no switching frequency.
- DCDC-USB-200 5 V maximum current, cold-crank behavior below 6 V, and ISO 7637-2 / ISO 16750-2 compliance: not published (the TVS-protected input is the only transient statement).
- OpenUPS2 operating temperature, switching frequency and ignition input: not stated on the product page. The original OpenUPS and DCDC-USB prices were not found.
- M4-ATX was not fetched directly; its ignition timing and 24 V support were not verified.
- Victron 5 V output: none found. Victron's converters are 12/24 V outputs only, so they fit only a 12 V-input CM5 carrier route.
- Tobsun specs and any Maxwell supercap module data were not retrieved.
- Mouser and Digi-Key stock and prices were not checked.

## Which off-the-shelf choice best covers ignition sense + hold-up + clean 5 V at 5 A?

### Takeaway
No single off-the-shelf stand-alone product found does all three. The closest practical stack is DCDC-USB-200 (ignition and off-delay logic, 6-34 V in, set to 5 V or 12 V) plus either a supercap ride-through stage (LTC3350-class) or an OpenUPS2 at 12 V for battery hold-up. For the cleanest documented 5 V, follow it with a well-characterized buck and an LC filter.

### Cited Findings
- The DCDC-USB-200 provides ignition sense, preset off-delays from 5 s to 2 h, hard-off, an 11.2 V deep-discharge cut-off, 6-34 V input and spread spectrum, but no energy storage. — [DCDC-USB-200 manual](https://www.cartft.com/support_db/support_files/DCDC-USB-200-manual.pdf)
- The OpenUPS2 provides battery hold-up (3x LiFePO4 18650) with a 12-24 V / 5 A output and shutdown pulses, but only an 11-24 V input and no stated ignition input. — [Mini-Box OpenUPS2](https://www.mini-box.com/OpenUPS2)
- The Mean Well RSD-30G-5 provides a railway-rated 9-36 V input to 5 V / 6 A at 70 mVp-p with a -40 °C rating, but no ignition or hold-up. — [Mean Well RSD-30 spec](https://www.meanwell.com/Upload/PDF/RSD-30/RSD-30-SPEC.PDF)
- The LTC3350 provides 25 W-class supercap backup aimed at 12 V ride-through. — [ADI LTC3350](https://www.analog.com/en/products/ltc3350.html)

### Inferences
- Recommended off-the-shelf path for a 12 V vehicle: DCDC-USB-200 in Automotive mode set to 5 V, with its ON/OFF pulse wired to the Pi's power-button input and OFFDELAY/HARDOFF set to about 5 s/60 s or 1 min/1 min. Because the DCDC-USB-200 draws from permanent battery and keeps running after ignition-off, the "hold-up" for graceful shutdown on key-off comes from the vehicle battery itself. Stored energy is then needed only for crank dips below 6 V and for abrupt battery disconnects. That makes a small supercap stage sufficient, rather than a full battery UPS.
- For 24 V trucks and RVs: the DCDC-USB-200's 34 V electronic shutdown (clamping 34-36 V) leaves little margin for a 24 V system's load dump. A 24 V installation should add an upstream surge stopper or use a 9-36 V or 18-75 V railway brick.
- Supercap sizing (my calculation, not sourced): 30 s at 25 W is 750 J. With a 4-series stack at 10.8 V discharged to 5 V, usable energy is about 45.8 J per farad of stack, so the stack needs about 16 F (about 65 F per cell). A 10 s brown-out ride-through needs about a third of that. The roughly 2-3 s needed for an orderly Linux `poweroff` after an abrupt disconnect needs only a few farads.

### Gaps
- I found no in-vehicle forum test report measuring AM or FM interference from any of these specific units.
- I could not confirm that the DCDC-USB-200's pulse output levels suit the Pi 5 PWR_BUT pad directly.

## What would an open-hardware front end need (component classes and reference designs)?

### Takeaway
Use the classic automotive chain: reverse-battery protection (ideal-diode controller), a surge stopper or TVS for ISO 7637-2 / ISO 16750-2 load dump, a wide-input spread-spectrum synchronous buck (400 kHz or above 2 MHz, so its harmonics stay out of the AM band or are spread) with an input π-filter, optional supercap backup (LTC3350/3351), and a small MCU or PMIC for ignition-sense and timer logic. ADI and TI publish reference designs that cover each block.

### Cited Findings
- ADI app note AN-2083 describes a 12 V / 24 V nominal input to 5 V / 3 A output front end using an LT4356-1 surge stopper and an LT8609A buck. The full app note fetch timed out, so the details are from the search summary. — [ADI AN-2083](https://wcm-cce.cldnet.analog.com/media/en/technical-documentation/app-notes/an-2083.pdf)
- The LT4363 surge stopper has 4-80 V input and an adjustable output clamp. Its demo board DC2062A is a 12 V ISO 7637-2 design that limits up to 50 VDC continuous input and up to 100 V load dumps to about 25 V at the output, with a 3 A load (3.8 A current limit). The LT4363-1 latches off; the LT4363-2 auto-retries. LTspice models are available. — [ADI LT4363](https://www.analog.com/en/products/lt4363.html); [DC2062A schematic](https://www.analog.com/media/en/technical-documentation/eval-board-schematic/DC2062A2-SCH.PDF)
- TI TIDA-01524 uses a 5 A, 400 kHz synchronous buck with spread spectrum and 3.5-36 V operation that supports start-stop and cold-crank, with CISPR 25 Class 5 conducted-emission results. Its main load is a 0.9 V / 10 A core, not 5 V. — [TI TIDA-01524](https://www.ti.com/tool/TIDA-01524)
- TI TIDA-01492 uses a 5 A, 2.2 MHz synchronous buck with smart-diode reverse-battery protection (about 20 mV drop), optimized for CISPR 25 Class 5 conducted emissions. — [TI TIDA-01492](https://ti.com/tool/TIDA-01492)
- TI TIDA-01179 is a 30 W front end for 12 V batteries tested to ISO 7637-2:2004 pulses 1, 2a and 3a/b at level IV (15 V intermediate rail). — [TI TIDA-01179](https://www.ti.com/tool/TIDA-01179)
- LTC3350 / DC1937B (12 V, 25 W) and the Vishay EDLCBU-12V provide a supercap backup reference with downloadable design files. — [ADI DC1937B](https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc1937b.html); [Vishay EDLCBU-12V](https://www.vishay.com/en/landingpage/reference-designs/EDLCBU-12V/)
- Battery-saver cut-off levels used in industry: 12.0 V minimum, 12.2-12.5 V in cold climates (BlackVue); the Mini-Box hard cut-off is 11.2 V. — [BlackVue forum](https://forum.blackvue.com/hc/en-us/articles/49160733840921-Battery-Protection-Features-Low-Voltage-Cutoff); [DCDC-USB-200 manual](https://www.cartft.com/support_db/support_files/DCDC-USB-200-manual.pdf)

### Inferences
- Block list for an open-hardware design:
  1. Input fuse and connector with permanent battery, ignition and ground pins.
  2. Reverse-battery protection with an ideal-diode controller and back-to-back FETs.
  3. Load-dump and ISO 7637-2 protection: an LT4363- or LT4356-class surge stopper, or a large TVS (SMCJ/SM8S class) for 12 V only. A surge stopper is needed for 24 V.
  4. Bulk and supercap storage via an LTC3350 or LTC3351 at the intermediate bus.
  5. A 2 MHz-class spread-spectrum synchronous buck (for example in the LT8609A or TIDA-01492 class) to 5 V / 6 A, with an input π-filter and a shielded inductor.
  6. A small MCU (or reused ESP32-class) for ignition debounce, programmable off-delay and hard-off, battery-voltage cut-off and a Pi PWR_BUT pulse, plus I2C telemetry for a Linux daemon.
- For AM broadcast reception, a converter switching at or above 2 MHz puts its fundamental above 1710 kHz, so only spurs and subharmonic behavior land in the band. That is generally easier than a 400-500 kHz part whose 2nd-4th harmonics fall in band. This is an engineering inference; confirm with near-field probe and receiver testing.

### Gaps
- No ADI or TI reference design found exactly matches 12/24 V input to 5 V / 5 A with ISO 7637-2 and CISPR 25 data together. Designs need to be combined.
- The LTC7860 (named in the brief) was not researched within budget.
- Full AN-2083 details (test pulses, emissions results) were not retrieved because the fetch timed out.
