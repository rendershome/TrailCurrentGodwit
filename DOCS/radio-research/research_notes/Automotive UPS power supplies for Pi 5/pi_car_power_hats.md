# Automotive 12 V power / ignition-shutdown / UPS add-on boards for Raspberry Pi 5 and CM5 (as of Nov 2026)

Research scope note: about 22 tool calls. Several vendor pages (The Pi Hut, Geekworm wiki, Mausberry) returned HTTP 403/429, so some figures come from search-engine extracts of those pages, distributor listings or news articles. Each is marked. No manufacturer published switching frequency, ripple or EMI figures for any board found. That is the biggest gap for the AM-reception concern.

## Which Pi car-power boards exist, and what are their specs?

### Takeaway
Only one current product is purpose-built for a car with ignition sense: the CarPiHAT PRO 5 (12 V buck, ignition latch, CAN, DAC, RTC, up to 5 A to a Pi 5). It has no hold-up energy store. Everything else is either a general UPS or power-management board that happens to accept a 12 V-class input (Geekworm/SupTronics X1203/X1205, Geekworm UPS Scap 5V5A supercap, UUGear Witty Pi 5, Sequent Super Watchdog), an industrial DIN-rail CM5 controller (Sfera Strato Pi Max), or a legacy or discontinued Pi 3/4-era product (Mausberry, PiJuice, Sixfab, Juice4halt, LiFePO4wered/Pi+). None publishes ISO 7637-2 / ISO 16750-2 compliance, load-dump ratings, cold-crank ride-through or switching-frequency/EMI data in the sources reached.

### Cited Findings

**CarPiHAT PRO 5 (The Pi Hut brand; designer GitHub "gecko242" / SamT-J), current product**
- "Next generation of the original CarPiHAT PRO". A new power-switching IC and higher-quality inductors let it provide up to 5 A to a Raspberry Pi 5. — [Core Electronics listing (search extract)](https://core-electronics.com.au/carpihat-pro-5-car-interface-dac.html.plain.md); [The Pi Hut listing (search extract)](https://thepihut.com/products/carpihat-pro-5-car-interface-dac-for-raspberry-pi-5)
- 12 V to 5 V buck converter "with fuse and filtering" powers the Pi and touchscreen. A 5 V output is provided for displays. No input-voltage range, transient/load-dump rating, ISO claim, switching IC part number or switching frequency is stated. — [Core Electronics](https://core-electronics.com.au/carpihat-pro-5-car-interface-dac.html)
- Draws less than 1 mA when switched off. — [Core Electronics](https://core-electronics.com.au/carpihat-pro-5-car-interface-dac.html); same figure in the [original CarPiHat wiki](https://github.com/gecko242/CarPiHat/wiki)
- Ignition and shutdown: the Pi latches its own power on a GPIO, a script or service watches the "Switched 12V" (ignition) input, and after shutdown the latch is released when the CPU halts. The example script uses a 10 s delay after ignition-off before shutdown starts. Holding the latch pin low after shutdown removes parasitic draw. — [CarPiHat wiki Quick Start (search extract)](https://github.com/gecko242/CarPiHat/wiki/Quick-Start-Guide); [Floating in the clouds write-up](https://floatingintheclouds.com/openauto-pro-carpihat/)
- Hold-up: no supercap or battery. The board relies on a permanent 12 V feed plus the ignition-sense input (constant plus switched 12 V), so it covers ignition-off shutdown but not loss of the battery feed. — [RPi forum thread, mraught, Dec 2023](https://forums.raspberrypi.com/viewtopic.php?t=361432); [Core Electronics](https://core-electronics.com.au/carpihat-pro-5-car-interface-dac.html)
- Extras: 1 independent CAN port (controller chip not named on the page), 192 kHz/24-bit Burr-Brown DAC with 3.5 mm out (uses the hifiberry-dacplus overlay), RTC, 2 high-side switched 12 V outputs at 1 A each, 5 opto-isolated 12 V inputs (reverse, illumination, aux, 2 general-purpose), I2C and 1-Wire. Ships with a shroud and fan; the standoffs leave no room for the Active Cooler. — [Core Electronics](https://core-electronics.com.au/carpihat-pro-5-car-interface-dac.html)
- Price AUD 259.95 inc GST. Out of stock at Core Electronics when checked (Nov 2026). — [Core Electronics](https://core-electronics.com.au/carpihat-pro-5-car-interface-dac.html)
- Predecessors: the original CarPiHAT PRO is marked "[Discontinued]" at The Pi Hut. — [The Pi Hut](https://thepihut.com/products/carpihat-pro-car-interface-dac-for-raspberry-pi). In Dec 2023 a forum user relayed the designer's statement that the older CarPiHAT could not supply the 5 A a Pi 5 needs, and another user reported its DAC did not work on the Pi 5 (this predates the PRO 5). — [RPi forum t=361432](https://forums.raspberrypi.com/viewtopic.php?t=361432)
- Open-source status: a GitHub repo and wiki ("A place to store files and info about the CarPiHat") exist, but I could not confirm whether PRO 5 schematics or a licence are published. — [GitHub SamT-J/CarPiHat](https://github.com/SamT-J/CarPiHat)

**Geekworm UPS Scap 5V5A (supercapacitor UPS for Pi 5), new (CNX article 5 Oct 2026)**
- Inputs: 9-24 V DC (5.5x2.1 mm barrel) or 12 V USB-C PD; use only one at a time. — [CNX Software](https://www.cnx-software.com/2026/10/05/geekworm-ups-scap-5v5a-a-supercapacitor-ups-hat-for-raspberry-pi-5/)
- Output up to 5 V / 5 A total, shared between a USB-C port with PD handshake (feeds the Pi 5 through a supplied U-shaped C-to-C adapter, avoiding the "low power" warning) and a USB-A port. A pogo-pin power output is unpopulated. — [CNX Software](https://www.cnx-software.com/2026/10/05/geekworm-ups-scap-5v5a-a-supercapacitor-ups-hat-for-raspberry-pi-5/)
- Capacitance 100 F, 60 F or 22 F. Hold-up with 100 F is about 9 s at 5 V/5 A, or about 3.5 min at 0.5 A in "Long Endurance" mode (1.6 V cutoff; "Performance" mode cuts off at 2.4 V). Recharge takes about 15 min. — [CNX Software](https://www.cnx-software.com/2026/10/05/geekworm-ups-scap-5v5a-a-supercapacitor-ups-hat-for-raspberry-pi-5/)
- Power-loss detection on GPIO26 through pogo pins, a wake/sleep pin, INA219 I2C monitoring of cap voltage and current, and reference shutdown and auto-wake scripts. It has no dedicated ignition input: it detects loss of input power. — [CNX Software](https://www.cnx-software.com/2026/10/05/geekworm-ups-scap-5v5a-a-supercapacitor-ups-hat-for-raspberry-pi-5/); [Geekworm product page (search extract)](https://geekworm.com/products/rpi5-ups-scap-pd5v5a)
- Protection: a main output switch and a discharge button are listed. No overvoltage, reverse-polarity or transient claims. — [CNX Software](https://www.cnx-software.com/2026/10/05/geekworm-ups-scap-5v5a-a-supercapacitor-ups-hat-for-raspberry-pi-5/)
- Price USD 41 (AliExpress, 100 F) or 44.90-48.90 (Geekworm store). — [CNX Software](https://www.cnx-software.com/2026/10/05/geekworm-ups-scap-5v5a-a-supercapacitor-ups-hat-for-raspberry-pi-5/)

**Geekworm / SupTronics X120x Li-ion UPS family for Pi 5 (bottom-mount, pogo pins)**
- X1205: 9-18 V input (DC 5521 or XH2.54) at 3 A or more (12 V 5 A adapter recommended), UPS output 5.1 V ±5 %, max 6 A. 2x 21700 cells, up to 3 A charging. Do not also power the Pi through its USB-C. Has power-loss detection and safe shutdown. — [Geekworm wiki X1205 (search extract)](https://wiki.geekworm.com/X1205); [Geekworm product page](https://geekworm.com/en-au/products/x1205). One Newegg listing says "4-cell", which conflicts with the wiki; the 4-cell model is the X1206. — [Newegg](https://www.newegg.com/p/2VH-00PY-00040)
- X1203: 6-18 V DC input (12 V 3 A adapter recommended), UPS output 5.1 V ±5 % max 5 A. — [The Pi Hut X1203 (search extract)](https://thepihut.com/products/suptronics-x1203-ups-add-on-for-raspberry-pi-5)
- X1202 V1.1: 4x 18650 cells, up to 5 A / 25 W to a Pi 5. — [CNX Software](https://www.cnx-software.com/2024/05/24/suptronics-x1202-raspberry-pi-5-ups-hat-four-18650-batteries-25w/?amp=1)
- Geekworm X1502 CM5 IO carrier: 9-17 V wide input, 5.5 A for the CM5. No UPS or ignition features listed. — [Geekworm X1502](https://geekworm.com/products/x1502)

**UUGear Witty Pi 5 HAT+ (RP2350-based power manager), current**
- Input 5 V USB-C or 6-30 V DC (KF350 screw terminal). Mode 1 Power HAT+, delivers up to 5 A to the Pi and its peripherals. Ideal diodes isolate VIN and VUSB. — [UUGear product page](https://www.uugear.com/product/witty-pi-5/)
- Monitors VIN and can shut down or power on at voltage thresholds. Also has schedules and temperature triggers, an RX8025T RTC (±3.8-5 ppm, CR2032 backup) and a TMP112 sensor. No dedicated ignition input is listed, and it has no battery or supercap hold-up. — [UUGear](https://www.uugear.com/product/witty-pi-5/); [CNX Software Jan 2026](https://www.cnx-software.com/2026/01/19/witty-pi-5-hat-a-raspberry-pi-rp2350-based-power-scheduler-with-time-temperature-and-voltage-based-triggers/)
- Standby is about 0.7 mA for the board in hibernation, or 2.5-3 mA with a Pi attached. A reseller lists about 8 mA, which conflicts. Operating range -30 to 80 °C. — [UUGear](https://www.uugear.com/product/witty-pi-5/); [The Pi Hut (search extract)](https://thepihut.com/products/witty-pi-5-hat-real-time-clock-and-power-management-for-raspberry-pi)
- Firmware and software are open source on GitHub (uugear/Witty-Pi-5-Firmware, uugear/Witty-Pi-5-Software); the page names no licence. Price EUR 39 (Witty Pi 5 Mini EUR 29). — [UUGear](https://www.uugear.com/product/witty-pi-5/)

**Waveshare UPS HAT (E)**
- 4x 21700 cells, 5 V 6 A output, bidirectional USB-C (up to 40 W), pogo pins. Input is USB-C (multi-voltage PD), not a 12 V vehicle input. The wiki tells Pi 5 users to set usb_max_current_enable=1 or PSU_MAX_CURRENT=5000. — [Waveshare product page](https://www.waveshare.com/ups-hat-e.htm); [Waveshare docs](https://docs.waveshare.com/UPS-HAT-E); [CNX Software](https://www.cnx-software.com/2024/08/03/waveshare-ups-hat-e-for-raspberry-pi-5-4-3b-takes-four-21700-lithium-batteries-supports-usb-pd-3-0/)
- UPS HAT (D) is for the Pi 4/3 only. — [search extract of Waveshare listings](https://www.waveshare.com/ups-hat-e.htm)

**Sequent Microsystems Super Watchdog (V7) and Multichemistry Watchdog UPS**
- Super Watchdog "can deliver only 2.5A @5V" because of space limits. The listing points Pi 5 users to the Multichemistry Watchdog UPS, which accepts 5 V or 10-24 V and gives 2.5 A continuous / 3 A peak. Neither reaches 5 A. — [The Pi Hut Super Watchdog listing (search extract)](https://thepihut.com/products/super-watchdog-for-raspberry-pi); [RobotShop](https://ca.robotshop.com/products/super-watchdog-with-battery-backup-raspberry-pi)

**Seeed SuperCAP UPS LTC3350 module**
- An LTC3350 supercap backup controller module designed for the reComputer R1000 / R-series (CM4-based industrial PCs). It is not a generic Pi HAT. DigiKey shows a 33-week lead time, and openelab lists it as pre-order. — [Fabtolab](https://www.fabtolab.com/seeed-supercap-ups-ltc3350-module-recomputer-r1000-backup-power-high-efficiency); [DigiKey AU](https://www.digikey.com.au/en/products/detail/seeed-technology-co-ltd/110992004/24814462); [openelab](https://openelab.io/fr/products/seeed-studio-supercap-ups-ltc3350-module)

**Sfera Labs Strato Pi family (industrial, CM5 option)**
- Strato Pi Max now offers a CM5 in a DIN-rail case. — [CNX Software Dec 2024](https://www.cnx-software.com/2024/12/12/sfera-labs-strato-pi-max-din-rail-industrial-controllers-raspberry-pi-compute-module-5/)
- A SuperCaps UPS X2-series expansion board is available for Strato Pi Max. — [Electronics-Lab](https://www.electronics-lab.com/news/sfera-labs-releases-supercapacitors-ups-expansion-board-for-raspberry-pi-server/)
- The older Strato Pi UPS line (Pi 3/4) is specified at 9-28 Vdc with surge and reverse-polarity protection, a 2.2 A resettable fuse and an external 12 V lead-acid battery. — [Strato Pi UPS user guide (Mouser)](https://www.mouser.com/datasheet/2/866/stratopi_ups_user_guide-1217981.pdf). Strato Pi Max input range and 5 V budget were not verified ([user guide link](https://www.mouser.com/datasheet/2/866/stratopimax_user_guide-3460223.pdf)).

**Legacy / discontinued**
- Mausberry 3A Car Supply/Switch: discontinued, and orders ship the "4AMP" version. Rated 3 A peak, senses ignition and shuts down the Pi/Pi 2/Pi 3, with no Pi 4/5 listed. Temporarily out of stock. — [Mausberry (search extract)](https://www.mausberrycircuits.com/collections/car-power-supply-switches/products/3a-car-supply-switch)
- PiJuice HAT: retired at SparkFun and Core Electronics (Core names "Raspberry Pi UPS" as the replacement); unavailable at RS. — [SparkFun](https://www.sparkfun.com/pijuice-hat-raspberry-pi-portable-power-platform.html); [Core Electronics](https://core-electronics.com.au/pijuice-hat.html)
- Sixfab Power Management & UPS HAT: end of life, citing chip shortage. — [Sixfab changelog](https://docs.sixfab.com/changelog/test); [Sixfab community](https://community.sixfab.com/t/ups-hat-discontinued/2281)
- Juice4halt (supercap): 2017 datasheet covers 12 V and 24 V input variants and targets the Pi B+/2/3. It gives "a few seconds" of hold-up, and the caps must fully discharge before reboot. — [Juice4halt datasheet](https://juice4halt.com/wp-content/uploads/2017/11/datasheet20-HV.pdf); [skhg/juice4halt scripts](https://github.com/skhg/juice4halt)
- LiFePO4wered/Pi+: 2 A continuous output rating (per the designer's Hackaday log); at about 1.5 A on a Pi 4 the board reached about 90 °C. — [Hackaday.io](https://hackaday.io/project/20909-lifepo4weredpi)

**Open-source DIY designs**
- RPiCarPowerHAT (Compizfox): LM2596 buck, ignition-switched MOSFET with Pi self-latch, and low-voltage cutoff. — [GitHub](https://github.com/Compizfox/RPiCarPowerHAT)
- Other designs: zbchristian Raspi_PowerSwitch, Benoit3 RaspPowerHat, and Scott Baker's 5-cell supercap UPS for a 12 V input with shunt balancing. — [Raspi_PowerSwitch](https://github.com/zbchristian/Raspi_PowerSwitch); [RaspPowerHat](https://github.com/Benoit3/RaspPowerHat); [sbelectronics/ups](https://github.com/sbelectronics/ups)

**Forum experience (RPi forum "Powering Raspberry Pi 5 in a vehicle", Dec 2023)**
- An xy-3606 buck converter worked on a 12 V 3 A router UPS. Users stressed that a car-rated converter must handle spikes and load dump. rpdom noted the Pi 5 is designed to accept power on the header pins, as with the PoE HAT+. — [RPi forum t=361432](https://forums.raspberrypi.com/viewtopic.php?t=361432)

### Inferences
- No product found combines all of: native 12 V vehicle input, ignition sense, energy hold-up and 5 A out. The nearest combinations are:
  - CarPiHAT PRO 5 (ignition, CAN, DAC and RTC, but no hold-up).
  - Geekworm UPS Scap 5V5A or X1205 (hold-up and 5-6 A, but no ignition input).
  - Witty Pi 5 (6-30 V, 5 A, RTC and voltage triggers, but no ignition input and no hold-up).
- An ignition signal could be added to the Witty Pi 5 or the Geekworm boards with a separate opto input to a GPIO. Godwit already has GPIO and CAN, and ignition state may also be available over CAN.
- Because CarPiHAT PRO 5 has its own CAN port, it would duplicate one channel of the Waveshare 2-CH CAN FD HAT and likely conflict on SPI chip selects and GPIO. Stacking it with the 2-CH CAN FD HAT needs a pin-map check.
- CarPiHAT PRO 5 holds the Pi's own power latch, so it is the only option with true "key off, then delay, then halt, then cut power" sequencing and under 1 mA off-draw. Supercap boards only cover sudden loss of supply.
- A 100 F supercap (about 9 s at 25 W) will not cover a full Linux shutdown of a Pi 5 under full load. In practice the 10.1" panel and USB loads would need to be shed on power-fail.

### Gaps
- Switching frequency, ripple and conducted/radiated EMI figures: none were found for any board (CarPiHAT PRO 5, X120x, Scap, Witty Pi 5). Measuring them would need a bench test or a schematic and BOM review.
- ISO 7637-2 / ISO 16750-2, load-dump and cold-crank ride-through claims: none were found for any Pi HAT. The CarPiHAT says only "fuse and filtering".
- CarPiHAT PRO 5: input range, buck IC part number, CAN controller chip, operating temperature and PRO 5 schematic licence are not confirmed (The Pi Hut page returned 429).
- Mausberry 4A version: specs and Pi 5 suitability were not retrieved (the vendor site returned 429).
- Seeed LTC3350 module: input range, output current and hold-up were not found.
- Sfera Strato Pi Max (CM5): input range and UPS hold-up were not verified.
- I found no current vehicle-power HAT from Pimoroni, Inno-Maker or SunFounder. SunFounder PiPower 5 is a 2S battery HAT+ with 5 V 5 A output ([search mention](https://computingforgeeks.com/best-raspberry-pi-ups-hat/)), but its input range was not verified. 52Pi UPS Gen 6 also exists ([CNX Software](https://www.cnx-software.com/2025/12/10/52pi-ups-gen-6-ups-raspberry-pi-hat-supports-up-to-4x-external-battery-packs-pikapython-scripting/)); its input and output specs were not checked.
- I found no independent in-car test reports for any of the Pi 5-era boards.

## Which can genuinely deliver 5 A at 5 V to a Pi 5 from 12 V?

### Takeaway
Boards claiming 5 A or more to a Pi 5 from a 12 V-class input:
- CarPiHAT PRO 5 ("up to 5A")
- Witty Pi 5 HAT+ (6-30 V in, 5 A, Mode 1 Power HAT+)
- Geekworm X1205 (9-18 V in, 6 A max)
- Geekworm X1203 (6-18 V in, 5 A max)
- Geekworm UPS Scap 5V5A (9-24 V or 12 V PD in, 5 A total shared with USB-A)

Sequent Super Watchdog (2.5 A), Sequent Multichemistry (2.5 A / 3 A peak), LiFePO4wered/Pi+ (2 A) and legacy Mausberry 3A cannot. All 5 A figures are vendor claims; I found no independent load tests.

### Cited Findings
- CarPiHAT PRO 5: "can provide 5A to Raspberry Pi 5". — [Core Electronics](https://core-electronics.com.au/carpihat-pro-5-car-interface-dac.html)
- Witty Pi 5: "As a Mode 1 Power HAT+ board, Witty Pi 5 can deliver up to 5A". — [UUGear](https://www.uugear.com/product/witty-pi-5/)
- X1205: 5.1 V max 6 A from 9-18 V. — [Geekworm wiki](https://wiki.geekworm.com/X1205)
- X1203: 5.1 V max 5 A from 6-18 V. — [The Pi Hut (search extract)](https://thepihut.com/products/suptronics-x1203-ups-add-on-for-raspberry-pi-5)
- UPS Scap 5V5A: 5 V / 5 A total shared between USB-C (PD handshake to the Pi) and USB-A. — [CNX Software](https://www.cnx-software.com/2026/10/05/geekworm-ups-scap-5v5a-a-supercapacitor-ups-hat-for-raspberry-pi-5/)
- Sequent Super Watchdog: 2.5 A max. Multichemistry Watchdog: 2.5 A continuous / 3 A peak. — [The Pi Hut (search extract)](https://thepihut.com/products/super-watchdog-for-raspberry-pi)
- On a Pi 5 fed by a non-PD source, Waveshare directs users to set usb_max_current_enable=1 or PSU_MAX_CURRENT=5000. — [Waveshare docs](https://docs.waveshare.com/UPS-HAT-E)

### Inferences
- For Godwit, header-fed boards (CarPiHAT PRO 5, Witty Pi 5 as a Power HAT+, X120x pogo-pin boards) will not do USB-PD negotiation. The image's existing usb_max_current_enable=1 is therefore required and appropriate.
- The Scap 5V5A instead feeds USB-C with PD. Its 5 A is shared with its USB-A port.
- Rough load estimate (my inference from the brief, not measured): Pi 5 SoC/RAM under load (about 2-2.5 A), panel 0.92 A, 2x RTL-SDR at about 0.6 A, 2x ESP32-S3 dials, USB audio and the CAN FD HAT. That sits close to 5 A, so a board rated exactly 5 A has little headroom. The X1205 (6 A) is the only one with stated margin.

### Gaps
- No thermal derating curves or continuous-vs-peak split was published for CarPiHAT PRO 5, Witty Pi 5 or X1203 at 5 A.
- I found no measurement of 5 V droop at 5 A for any of these boards.
