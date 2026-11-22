# Vehicle-rated (and near-vehicle) Raspberry Pi 5 / CM5 / CM4 units with 2+ CAN buses

Researched 2026-11-22. Specs come from vendor pages where I could reach them. Where a figure comes only from news (CNX Software, LinuxGizmos) or reseller copy (Makerfabs, McHobby), it is flagged. Several vendor sites (edatec.cn, chipsee.com) redirected, timed out or returned 403, and the EDATEC datasheet PDF on Mouser timed out, so those products rely on secondary sources.

## Which Pi 5/CM5 (or CM4) products are marketed for in-vehicle / mobile-machinery use and have 2+ CAN?

### Takeaway
No product found is a CM5/Pi 5 unit that is both marketed for vehicles and has 2+ CAN. The units that really target vehicles (AutoPi TMU CM4, AutoPi CAN-FD Pro, Intrepid neoVI PI, Austral Quantum CM4) are all CM4-based. The CM5 units with 2 CAN (EDATEC ED-IPC3630, Seeed reComputer R21) are DIN-rail industrial boxes with 9-36 V input, not vehicle products. No rugged vehicle display terminal with a CM4/CM5 inside and 2 CAN turned up. Display panels found (Chipsee, CrowPanel, Waveshare, ED-HMI3630) have 0-1 CAN, or their CAN count could not be confirmed.

### Cited Findings

**Vehicle-targeted, CM4-based (flag: CM4, not CM5)**
- AutoPi TMU CM4: built on CM4 (BCM2711, 1/2/4/8 GB RAM, 8/16/32 GB eMMC). It has 2x CAN-FD via SocketCAN at up to 5 Mbps data phase, with native J1939 and OBD-II. A "CAN 2.0 variant" without FD also exists. — [AutoPi TMU CM4](https://www.autopi.io/hardware/autopi-tmu-cm4/)
- AutoPi TMU CM4 power input: the page gives "12-35 V DC" in one place and "9-35 V DC" in another, so the vendor page is self-inconsistent. Its Smart Power Manager does a controlled shutdown on ignition-off and has configurable voltage thresholds to protect the auxiliary battery. — [AutoPi TMU CM4](https://www.autopi.io/hardware/autopi-tmu-cm4/)
- AutoPi TMU CM4 is described as "CE/FCC/E-Mark certified". The same page notes that vehicle type approval requires assessing the full installation. — [AutoPi TMU CM4](https://www.autopi.io/hardware/autopi-tmu-cm4/)
- AutoPi TMU CM4 I/O: mini-HDMI, 2 USB, onboard speaker for alerts, Gigabit Ethernet, LTE Cat 4 + GNSS, Wi-Fi/BT, NXP SE051 secure element, IMU. Price starts at EUR 420 (subscription option, ex-tax). Listed in stock. — [AutoPi TMU CM4](https://www.autopi.io/hardware/autopi-tmu-cm4/)
- An IP67 enclosure is offered as an extension option for the TMU CM4. — [AutoPi TMU CM4 (search snippet)](https://www.autopi.io/hardware/autopi-tmu-cm4/)
- AutoPi CAN-FD Pro: a dual-channel CAN-FD (5 Mbps) / J1939 logger on CM4. The spec page lists supply as "12.5V AC (Car battery power). Up to 35V (Trucks)", and "AC" is apparently a typo. Other specs: mini-HDMI 1080p60, 2 USB, built-in speakers, IP67 only with an optional casing. The spec page gives no operating temperature, transient protection, ignition or E-mark info. — [AutoPi CAN-FD Pro specs](https://docs.autopi.io/hardware/autopi_canfd_pro/device_specifications/); [AutoPi CAN-FD Pro](https://www.autopi.io/hardware/autopi-canfd-pro/)
- AutoPi has no CM5 product (no CM5 hit in search, Nov 2026). — [AutoPi compare page in search results](https://www.autopi.io/hardware/compare/)
- Intrepid Control Systems neoVI PI: built on a Raspberry Pi 4 Compute Module, with two internal ValueCAN4-2 modules giving 4x CAN FD/CAN 2.0. It has an "automotive power supply, 5-60V operation", is "tested and packaged for in-vehicle use", and uses a DB26 vehicle connector. Other I/O: HDMI + 2x DisplayPort, 4x high-current USB host, M.2 NVMe up to 4 TB, RTC, GbE with PoE sink, buzzer. Price is quote-only. No temperature, ignition or EOL info on the page. — [Intrepid neoVI PI](https://intrepidcs.com/products/vehicle-network-adapters/neovipi/)
- Austral Engineering Quantum CM4 OEM: CM4, 2x CAN (NMEA2000, J1939, CANopen), IP67, -20 to +60/65 C, galvanic isolation and watchdog (unspecified), no display (remote HTML5 HMI), from EUR 499 at 100 pcs. **Marked out of stock and "replaced by the Xplorer line"**, with the Xplorer CM5 recommended for new designs. — [Austral Quantum CM4 OEM](http://austral-eng.com/en/quantum-cm4-oem-en/)

**CM5-based with 2 CAN, industrial (not vehicle-marketed)**
- EDATEC ED-IPC3630 family (ED-IPC3632/3633/3634): CM5, 2x CAN 2.0B, 10-1000 kbps, 3 kV isolation, on a 6-pin 3.5 mm terminal. No CAN FD mentioned. 9-36 V DC input on a 2-pin Phoenix terminal, 24 W max. -25 to 60 C. DIN rail mount, 143x100x55 mm. — [CNX Software](https://www.cnx-software.com/2025/03/24/edatec-ed-ipc3630-raspberry-pi-cm5-powered-industrial-pc-di-docan-bus/) (news); [Makerfabs listing](https://www.makerfabs.com/raspberry-pi-cm5-based-industrial-computer.html) (reseller). The manufacturer datasheet is at [Mouser](https://www.mouser.lt/datasheet/3/6063/1/ED_IPC3630_datasheet_en.pdf) but timed out on fetch, so this is not verified first-hand.
- ED-IPC3630 power protection: the reseller lists reverse-polarity, overvoltage and overcurrent protection on the power input. The CNX article lists ESD/surge protection only for the serial ports. — [Makerfabs](https://www.makerfabs.com/raspberry-pi-cm5-based-industrial-computer.html); [CNX Software](https://www.cnx-software.com/2025/03/24/edatec-ed-ipc3630-raspberry-pi-cm5-powered-industrial-pc-di-docan-bus/)
- ED-IPC3630 I/O: 1x HDMI (4Kp60), no DSI. 12 V/1 A and 5 V/1 A outputs for powering an external LCD. M.2 B-key 2230/2242 with NVMe SSD options of 128/256/512 GB. 2x USB 2.0 Type-A plus USB 2.0 on an FPC/WTB header. Buzzer only, no audio. RTC with 1 F supercap + CR1220 (RTC backup only, no UPS). Watchdog hardware present, software support pending. 8 DI / 8 DO, dual Ethernet, optional 4G. — [CNX Software](https://www.cnx-software.com/2025/03/24/edatec-ed-ipc3630-raspberry-pi-cm5-powered-industrial-pc-di-docan-bus/); [Makerfabs](https://www.makerfabs.com/raspberry-pi-cm5-based-industrial-computer.html)
- ED-IPC3630 price: DigiKey from $300 (ED-IPC3632-10432-S, 4 GB/32 GB, Wi-Fi) per CNX. Makerfabs sells IPC3632-10216 for $316, in stock. — [CNX Software](https://www.cnx-software.com/2025/03/24/edatec-ed-ipc3630-raspberry-pi-cm5-powered-industrial-pc-di-docan-bus/); [Makerfabs](https://www.makerfabs.com/raspberry-pi-cm5-based-industrial-computer.html)
- Seeed reComputer Industrial R2000 series (CM5): R21 has 2x isolated CAN FD, R22 has 1x isolated CAN FD (with Hailo-8), and R20's table lists no CAN. All take 9-36 V DC on a 2-pin terminal. Optional SuperCAP UPS module (LTC3350). R20/R21 run -20 to 65 C, R22 -20 to 50 C. 2x HDMI 2.0, M.2 NVMe optional, separate M.2 B-key for 4G/5G. R21 has 3x USB-A 3.0 + 1x USB-A 2.0. Audio jack, 1-255 s hardware watchdog, IP40. CE/FCC/TELEC/RoHS/REACH only, no vehicle certification. Price on quote. — [Seeed reComputer Industrial R](https://www.seeed.cc/product/recomputer-industrial-r)
- Sfera Labs Strato Pi Max: CM4 or CM5, with up to four internal X2 expansion slots that can hold CAN and UPS modules. Channel count, FD support and power range are not stated on the overview page. EUR 502+tax. Strato Pi Plus (Pi 4/5): 10-50 V DC, CAN FD listed (count not stated), CE/FCC/IC, EUR 329+tax. Sfera lists "Marine" and "Fleet Management" as industries but makes no vehicle rating claim. — [Sfera Labs Strato Pi](https://sferalabs.cc/strato-pi/)

**Single-CAN or unconfirmed (ruled out or need checking)**
- Chipsee CS-CM5-BOX: 1x CAN FD, 9-30 V, optional 24 V ignition signal input. This is the only CM5 box found with an ignition input, but it has only one CAN. — [search snippet of chipsee.com](https://chipsee.com/product/cs-cm5-box/) (page returned 403 on fetch)
- Chipsee PPC-CM5-050 5" panel: CAN (count not confirmed), 9-30 V. — [RaspberryPi.dk listing](https://raspberrypi.dk/en/product/chipsee-ppc-cm5-050-industrial-pc-display/)
- EDATEC ED-HMI3630 (7/10.1/15.6" CM5 HMI): 9-36 V, isolated I/O, dual LAN. CAN count not confirmed. — [Astradis news](https://www.astradiselektronik.de/en/news/edatec-ed-hmi3630); [CNX Software](https://www.cnx-software.com/2025/05/04/edatec-launches-7-inch-10-1-inch-and-15-6-inch-industrial-hmi-displays-powered-by-raspberry-pi-cm5/)
- Waveshare IPCBOX-CM5: 1x CAN (1 Mbps), 7-36 V via barrel jack, M.2 M-key NVMe, ~$100 without CM5. — [CNX Software 2026-03](https://www.cnx-software.com/2026/03/05/raspberry-pi-cm5-industrial-computer-features-rs485-rs232-can-bus-dio-interfaces-dual-ethernet-optional-4g-5g-cellular-module/)
- Waveshare CM5-HMI-BOX-A: 1x isolated CAN. — [Waveshare docs](https://docs.waveshare.com/CM5-HMI-BOX-A)
- Elecrow CrowPanel-CM4 7" Pi Terminal: 1 CAN sharing a terminal block with RS485, IP65 front. — [Elecrow manual](https://www.elecrow.com/download/product/DIS01447T/7_inch_CM4_Pi_Terminal_User_Manual.pdf)
- CompuLab IOT-GATE-RPI5 (CM5, reported May 2026): CAN-FD on terminal blocks (count not given), 12-24 V with reverse-voltage protection, 0-60 C or -40 to 80 C industrial grade, 2x DVI-D, optional NVMe. No ignition sense or ISO 7637 claims. — [LinuxGizmos](https://linuxgizmos.com/iot-gate-rpi5-is-a-fanless-raspberry-pi-cm5-gateway-with-rs485-and-can-fd/)
- Techbase ModBerry 500 CM5: CAN listed as "optional", count unknown. — [iiot-shop](https://iiot-shop.com/product/modberry-500-cm5/)

**DIY / add-on route**
- Copperhill jCOM dual-CAN board for Raspberry Pi: targeted list price US$59.95. A J1939 variant implements address claim (J1939/81) and transport protocol (J1939/21). — [Copperhill blog](https://copperhilltech.com/blog/raspberry-pi-with-can-bus-or-sae-j1939-turbo-interface-board/)
- NavBridge CM4: an open-source CM4/CM5 carrier for BMW E-series. It uses an automotive-grade buck converter and a TVS diode on the battery input, with no ISO 7637 testing claimed. — [GitHub NavBridge_CM4](https://github.com/DrZedEau/NavBridge_CM4)

### Inferences
- For a CM5-based Godwit headless box today, the ED-IPC3630 (2x isolated CAN 2.0B, 9-36 V, HDMI, NVMe) and Seeed reComputer R21 (2x isolated CAN FD, 9-36 V, 2x HDMI, 4x USB-A, audio jack, supercap UPS option) are the closest off-the-shelf fits. Both are industrial units and would need external automotive front-end protection and ignition-sense logic for 12 V vehicle use. R21 fits a head unit better (USB 3.0 ports, audio jack, UPS option, 65 C rating). ED-IPC3630 has no audio output.
- Vehicle-marketed Pi units with dual CAN currently stop at CM4 (AutoPi, neoVI PI). Moving Godwit to one of them would mean supporting CM4, which is outside Godwit's current Pi 5/CM5 support.
- Classic CAN at 500 kbit/s is covered by every 2-CAN unit listed, both CAN 2.0B and CAN FD (FD controllers run classic frames).

### Gaps
- EDATEC manufacturer datasheet not read first-hand (timeout). The power-input protections come from the Makerfabs reseller only.
- Austral Xplorer CM5 (successor to Quantum CM4): could not find specs, CAN count or price.
- No rugged vehicle/off-highway display terminal with CM4/CM5 inside and 2 CAN was found. Makers of such displays (e.g. machine-control display OEMs) generally don't disclose the SoC module, and searches turned up none.
- Not checked for lack of budget: Kunbus Revolution Pi (the Connect 5 search returned nothing on CAN), Hilscher netPI, Comfile ComfilePi, PiLink, OnLogic, Kontron. Their CAN counts and vehicle suitability are unverified here.
- Chipsee CS-CM5-BOX details come from a search snippet only (site returned 403).

## Automotive electrical/environmental fitness (transients, ignition, hold-up, temperature, certification)

### Takeaway
None of the sources found states ISO 7637-2 or ISO 16750-2 compliance, load-dump ratings or cold-crank behaviour for any Pi-based unit. AutoPi is the only one claiming E-mark. AutoPi (ignition-off controlled shutdown) and Chipsee CS-CM5-BOX (ignition input, single CAN) are the only units with documented ignition handling. Seeed (LTC3350 supercap UPS) is the only 2-CAN CM5 unit with an optional power-loss hold-up module. Upper operating temperature tops out at 60-65 C, short of a parked cabin's peak temperatures.

### Cited Findings
- AutoPi TMU CM4 has E-mark (plus CE/FCC) and ignition-off controlled shutdown with battery-voltage thresholds. The page states no operating temperature, transient standard or supercap. — [AutoPi TMU CM4](https://www.autopi.io/hardware/autopi-tmu-cm4/)
- AutoPi CAN-FD Pro spec page gives no transient, ignition, temperature or E-mark data. — [AutoPi CAN-FD Pro specs](https://docs.autopi.io/hardware/autopi_canfd_pro/device_specifications/)
- Intrepid neoVI PI has a 5-60 V automotive supply ("tested and packaged for in-vehicle use"). Its low 5 V floor implies tolerance of cold-crank dips, but no standard is cited. — [Intrepid neoVI PI](https://intrepidcs.com/products/vehicle-network-adapters/neovipi/)
- ED-IPC3630: 9-36 V input, -25 to 60 C. Reseller lists reverse polarity/overvoltage/overcurrent protection. RTC supercap only, no UPS. No certifications listed in the CNX article. — [CNX Software](https://www.cnx-software.com/2025/03/24/edatec-ed-ipc3630-raspberry-pi-cm5-powered-industrial-pc-di-docan-bus/); [Makerfabs](https://www.makerfabs.com/raspberry-pi-cm5-based-industrial-computer.html)
- Seeed R21: 9-36 V input, EMC ESD/EFT/surge ratings listed (values not captured), optional LTC3350 supercap UPS, -20 to 65 C, CE/FCC/TELEC certifications only. — [Seeed reComputer Industrial R](https://www.seeed.cc/product/recomputer-industrial-r)
- Chipsee CS-CM5-BOX supports an optional 24 V ignition signal input on a 9-30 V supply. — [Chipsee (search snippet)](https://chipsee.com/product/cs-cm5-box/)
- Quantum CM4: IP67, -20 to 60/65 C, galvanic isolation. Discontinued. — [Austral](http://austral-eng.com/en/quantum-cm4-oem-en/)

### Inferences
- A 9 V input minimum (EDATEC, Seeed) will not ride through a severe cold-crank dip on a 12 V system, which can fall well below 9 V (ISO 16750-2 starting profiles go down to roughly 4.5-6 V; this figure is from general knowledge, not a cited source here). Neither vendor states hold-up time. Seeed's supercap UPS would help but is sized for shutdown, not specified for crank ride-through.
- A 36 V maximum with unspecified surge protection does not cover an unsuppressed 12 V load dump (ISO 7637-2 pulse 5a can reach ~80-100 V; general knowledge, uncited). Godwit would still need a pre-regulator/TVS stage or a centrally clamped alternator.
- No vendor documents operation above 65 C. A unit in a parked cabin in summer will likely need a thermal-shutdown/resume policy.

### Gaps
- No ISO 7637-2 / ISO 16750-2 test reports were found for any product.
- No operating temperature was found for any AutoPi device or the neoVI PI.
- The meaning and scope of AutoPi's E-mark (which regulation, which variant) is not stated.

## What these products imply for a 12 V head unit (features standard on vehicle units vs missing on industrial panels)

### Takeaway
Vehicle-marketed units add ignition-aware power management (ignition-off controlled shutdown, battery-voltage cut-off), E-mark, a vehicle connector, and sometimes very wide input (5-60 V). Industrial CM5 boxes/panels offer wide DC input (9-36 V), isolated CAN and DIN mounting, but generally lack ignition sense, delayed-off timers, crank ride-through specs, load-dump ratings and vehicle certification. Audio and multiple USB-A ports are also often missing (ED-IPC3630 has a buzzer only).

### Cited Findings
- Ignition handling on vehicle units: AutoPi's Smart Power Manager does controlled shutdown on ignition-off and protects the battery via voltage thresholds. — [AutoPi TMU CM4](https://www.autopi.io/hardware/autopi-tmu-cm4/)
- Wide automotive supply and a vehicle harness connector: neoVI PI has a 5-60 V supply and a DB26 vehicle connector. — [Intrepid neoVI PI](https://intrepidcs.com/products/vehicle-network-adapters/neovipi/)
- Industrial units lack ignition and UPS by default. The ED-IPC3630 has an RTC supercap only and no ignition input. Seeed offers a UPS as an optional module, with no ignition input listed. — [CNX Software](https://www.cnx-software.com/2025/03/24/edatec-ed-ipc3630-raspberry-pi-cm5-powered-industrial-pc-di-docan-bus/); [Seeed](https://www.seeed.cc/product/recomputer-industrial-r)
- The exception is Chipsee CS-CM5-BOX, an industrial CM5 box with an optional ignition input (single CAN). — [Chipsee (snippet)](https://chipsee.com/product/cs-cm5-box/)
- AutoPi itself lists what a DIY Pi vehicle build needs: CAN HAT, power regulator, LTE, GPS, enclosure, ignition sensing, safe-shutdown logic. — [AutoPi blog (search snippet)](https://www.autopi.io/blog/raspberry-pi-can-bus-explained/)

### Inferences
- For Godwit, the features to add on top of an industrial dual-CAN CM5 box (or to design into a custom carrier) are:
  - ignition-sense input with a delayed-shutdown timer
  - low-voltage battery cut-off
  - supercap hold-up for crank dips and clean shutdown
  - a load-dump-rated front end (TVS + surge stopper) ahead of the 9-36 V input
  - audio output
  - and, if the product is sold as an installed head unit, E-mark/EMC testing
- The ED-IPC3630's DI inputs (ON at 5-30 V) could in principle serve as an ignition-sense input, with software-driven shutdown. This is an inference: EDATEC does not market it for that use.

### Gaps
- Pricing for Seeed R21, neoVI PI and Strato Pi Max with CAN modules was not found (quote-only or not stated).
- No published schematics were found for any of the vehicle-marketed units. Whether EDATEC/Seeed publish carrier schematics was not verified.
