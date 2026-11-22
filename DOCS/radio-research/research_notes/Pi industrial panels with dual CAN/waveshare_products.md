# Waveshare products for a Raspberry Pi 5 / CM5 vehicle head unit (dual CAN)

Research date: 2026-11-22. All specs are taken from Waveshare product pages (waveshare.com/*.htm), the Waveshare wiki (waveshare.com/wiki) or docs.waveshare.com unless marked "reseller" or "press". "Add to Cart shown" means the Waveshare page showed a buy button and no DISCONTINUED marker when fetched; Waveshare pages do not show stock counts.

## 1. Industrial HMIs / all-in-one panels (Pi 5, CM5, CM4)

### Takeaway
Waveshare has no dual-CAN HMI and no "CM5-HMI-xB" variant. The only HMIs with CAN are the CM5-HMI-7A/8A/10.1A-ACCE. They have one isolated CAN, which is an MCP2515 on SPI1-0. Their own GPIO use (GPIO7, 11, 16, 17) eats into both SPI buses, so the practical route to a second CAN on that panel is USB, not an internal SPI controller. The Pi 5 HMIs (7A, 080D, 101D) have no CAN, no audio and run on 5 V only.

### Cited Findings
**Full HMI line-up (Waveshare "Raspberry Pi HMI Selection" table, 34 rows)** — [Waveshare PI5-HMI-7A page](https://www.waveshare.com/pi5-hmi-7a.htm)
- 7 in, 720x1280, DSI, IPS, 170 deg, optical bonding: PI4-HMI-7A, PI5-HMI-7A, RPI-HMI-7A-ACCE(-B), RPI-HMI-7B-ACCE(-B) (the "7B" adds a 5MP camera), CM5-HMI-7A-ACCE.
- 7 in, 800x480, DSI, frame bonding: CM4-DISP-BASE-7A-BOX (built-in CM4, 5MP camera).
- 8 in, 1280x800 landscape, DSI, optical, 178 deg, 5MP camera: PI4-HMI-080D, PI5-HMI-080D, RPI-HMI-080D-ACCE(-B).
- 8 in, 800x1280 portrait, DSI, optical: RPI-HMI-8A-ACCE(-B) (no camera), CM5-HMI-8A-ACCE (5MP).
- 10.1 in, 1280x800 landscape, DSI, optical, 5MP: PI4-HMI-101D, PI5-HMI-101D, RPI-HMI-101D-ACCE(-B).
- 10.1 in, 800x1280 portrait, DSI, optical: RPI-HMI-10.1A-ACCE(-B), CM5-HMI-10.1A-ACCE.
- Also listed: Zero-DISP-7A (HDMI), the 13.3 in Magic Mirror C4/C4A/C4C and PiPad C4/C4A/C4B (CM4, HDMI, frame bonding), and the 21.5 in FHD Monitor plus (B) Pi 4B, (C) CM4 and (D) Pi 5 variants (HDMI, optical).
- The table has no "C"-suffix Pi 5 HMI (PI5-HMI-080C/101C) and no CM5-HMI "B" variant — [Waveshare PI5-HMI-7A page](https://www.waveshare.com/pi5-hmi-7a.htm)

**PI5-HMI-7A / PI4-HMI-7A / RPI-HMI-7A-ACCE**
- 450 cd/m2 brightness, 720x1280 portrait by default, 5 V supply through a screw terminal (3.81 mm pitch) or Type-C, optional 27 W PSU. Two HDMI, 2x USB 2.0, 2x USB 3.0, GbE, TF card. No audio or CAN mentioned. Price $68.99 to $281.99 depending on bundle — [Waveshare](https://www.waveshare.com/pi5-hmi-7a.htm)

**PI5-HMI-080D / 101D (and PI4-, RPI-...-ACCE, -ACCE-B)**
- 1280x800, 8 in and 10.1 in. Power is 5 V Type-C or a 5 V screw terminal (3.81 mm). Kits include a 27 W supply. "Adjustable brightness via software", with no nit figure given. No CAN, RS485 or audio, only a "Debugging serial port". Price $118.99 to $340.99. No DISCONTINUED marker — [Waveshare](https://www.waveshare.com/pi5-hmi-080d.htm)
- Waveshare publishes the internal HAT clearance as an image, "RPI-HMI-080D-101D-Internal-Space-Height.png". The ~19 mm figure in the brief was not re-verified from that image's text — [Waveshare wiki file](https://www.waveshare.com/wiki/File:RPI-HMI-080D-101D-Internal-Space-Height.png)
- The PI5-HMI-080C wiki gives an operating temperature of 0 / 25 / 60 (min/typ/max, presumably deg C). Treat this as the likely 080D figure, but it is unconfirmed for the D — [Waveshare wiki PI5-HMI-080C](https://waveshare.com/wiki/PI5-HMI-080C)
- The PI5-HMI-080C page and wiki still exist, but the C model is absent from the current selection table, which is consistent with it being discontinued. I did not see an explicit DISCONTINUED marker — [Waveshare selection table](https://www.waveshare.com/pi5-hmi-7a.htm)

**CM5-HMI-7A / 8A / 10.1A-ACCE** (CM5 not included)
- Sizes: 7 in 720x1280 (170 deg), 8 in 800x1280 (178 deg), 10.1 in 800x1280 (178 deg). Optical bonding, 5MP front camera, "Pure passive cooling" aluminium case. Price $125.99 to $129.99 — [Waveshare](https://www.waveshare.com/cm5-hmi-xa-acce.htm)
- The baseboard is the **CM5-IPC-BASE-B**. Package: display, baseboard, metal case and bracket, 2x 22-pin FFC, 2-pin screw terminal, 2x 6-pin double-layer terminal blocks, **12 V 4 A power supply**, thermal tape — [Waveshare](https://www.waveshare.com/cm5-hmi-xa-acce.htm)
- Power: "Supports 7-36 V DC power supply" on a screw terminal, plus Type-C. Supercapacitor power-loss protection works only from the 7-36 V input — [docs.waveshare.com CM5-HMI-7A-ACCE](https://docs.waveshare.com/CM5-HMI-7A-ACCE)
- I/O: "1x CAN supporting 1Mbps communication rate (isolated)", 2x USB 2.0 and 2x USB 3.2 Gen1, GbE, HDMI 4K, M.2 M-key (NVMe or PCIe AI module), M.2 B-key (4G/5G) with Nano SIM, RTC, fan header. Serial options: "2x RS485 + 2x RS232, or 4x RS485 (isolated)". 2 isolated DI (GPIO23, GPIO24, up to 36 V), 2 open-drain DO (GPIO27, GPIO22, 500 mA), 2-channel ADC (SGM58031 at I2C1 0x48), ATSHA204 (0x64), EEPROM (0x50) — [docs.waveshare.com](https://docs.waveshare.com/CM5-HMI-7A-ACCE)
- Audio: only a "Single controllable buzzer" on GPIO7. There is no codec or jack — [docs.waveshare.com](https://docs.waveshare.com/CM5-HMI-7A-ACCE)
- GPIOs the board itself uses: GPIO11 is the supercap low-voltage warning, GPIO16 the external power-cut warning, and GPIO25/26 the user LEDs. Internal headers are described only as "Multi-function interface expanded via GPIO", with no pinout — [docs.waveshare.com](https://docs.waveshare.com/CM5-HMI-7A-ACCE)
- **CAN implementation:** `dtoverlay=spi1-1cs` and `dtoverlay=mcp2515,spi1-0,oscillator=16000000,interrupt=17`, brought up with `ip link set can0 up type can bitrate 500000` — [docs.waveshare.com CAN tutorial](https://docs.waveshare.com/CM5-HMI-7A-ACCE-Tutorials-CAN)
- Operating temperature is not stated. The Resources page links only Raspberry Pi CM5 documents, with **no Waveshare schematic** — [docs.waveshare.com Resources](https://docs.waveshare.com/CM5-HMI-7A-ACCE/Resources-And-Documents)

**CM5-HMI-BOX-A / B** (headless)
- A has 4x RS485. B has 2x RS485 and 2x RS232. Both have 1x isolated CAN at 1 Mbps, 7-36 V DC or 5 V/5 A Type-C input, 2x MIPI (DSI or CSI), M.2 M-key NVMe (2230 to 2280), 2x USB 3.2 and 2x USB 2.0, a supercap (only from the 7-36 V input), and a buzzer on GPIO7 with no audio. The page mentions no 40-pin header, only a "5V output header" and an RTC battery header. SKUs 35745-35750 — [docs.waveshare.com CM5-HMI-BOX-A](https://docs.waveshare.com/CM5-HMI-BOX-A)

### Inferences
- MCP2515 on SPI1-0 occupies the SPI1 pins (GPIO18 CE0, 19 MISO, 20 MOSI, 21 SCLK on the standard Pi mapping). Those are the I2S/PCM pins, so an I2S codec cannot coexist with the onboard CAN on the CM5-HMI.
- SPI0 is also compromised. GPIO11 (SPI0 SCLK) is used as the supercap warning input and GPIO7 (SPI0 CE1) drives the buzzer. A second SPI CAN controller on SPI0 would need those functions removed. The other RP1 SPI buses on alternate pins are an untested option, and no Waveshare header exposes them. The realistic way to get a second CAN on a CM5-HMI is USB.
- The MCP2515 is classic-CAN only, with 2 RX buffers. At 500 kbit/s this is adequate for one bus, but the MCP2518FD is the more robust controller.

### Gaps
- No Waveshare schematic for the CM5-IPC-BASE-B or CM5-HMI, so I cannot say which CM5 pins are routed to the internal "expanded via GPIO" interface.
- No operating temperature is published for any CM5-HMI or for the 080D/101D, only 0-60 deg C on the 080C wiki.
- I could not read the exact HAT-clearance figure for the 080D/101D (it is published only as an image).
- I did not verify per-size prices for the CM5-HMI.

## 2. CM5 carrier / IO boards

### Takeaway
No Waveshare CM5 carrier has two CAN ports. Boards with CAN have one isolated CAN and no 40-pin header. The best HAT host is the **CM5-DUAL-ETH-4G/5G-BASE**, which has 7-36 V input, a 40-pin header, 2x MIPI DSI and an M.2 NVMe slot, but no CAN or audio.

### Cited Findings
- **CM5-DUAL-ETH-4G/5G-BASE**: "7V~36V wide voltage range DC jack, 5V 5A Type-C port", "Raspberry Pi 40PIN GPIO header", 2x 4-lane MIPI 22-pin for "DSI displays or CSI cameras", M.2 M-key PCIe Gen2/3 x1, only **2x USB 3.2 Gen1 Type-A**, RTC battery holder, PWM fan header. Board is 99.6 x 87 mm. Price $55.99 to $69.99; the BOX-A kit adds a case, fan and 12 V 4 A PSU. CAN and audio are not listed — [Waveshare](https://www.waveshare.com/cm5-dual-eth-4g-5g-base.htm)
- **CM5-IO-WIRELESS-BASE**: 7-36 V (DC jack or screw terminal), one isolated CAN (up to 1 Mbps), 2x isolated RS485, 2x MIPI (DSI/CSI), M.2 M-key (2242/2230), 2x USB 2.0 A, 1x USB 2.0 4-pin, 1x USB 3.2 A. It has a GPIO screw terminal rather than a 40-pin header. No audio is mentioned — [docs.waveshare.com](https://docs.waveshare.com/CM5-IO-WIRELESS-BASE)
- Its CAN tutorial uses an MCP2515 SPI overlay at 500 kbit/s — [docs.waveshare.com CAN](https://docs.waveshare.com/CM5-IO-WIRELESS-BASE-Tutorials-CAN)
- **IPCBOX-CM5-A**: 1x isolated CAN at 1 Mbps, dual MIPI, NVMe — [docs.waveshare.com](https://docs.waveshare.com/IPCBOX-CM5-A). Press coverage gives a 7-36 V input — [CNX Software](https://www.cnx-software.com/2025/08/21/raspberry-pi-cm5-carrier-board-supports-dual-ethernet-1gbe-2-5gbe-and-wide-7v-36v-dc-input/)
- A March 2026 Waveshare CM5 industrial computer with RS485/RS232/CAN/DIO and dual Ethernet was reported. I did not open it, so its CAN count is unverified — [CNX Software](https://www.cnx-software.com/2026/03/05/raspberry-pi-cm5-industrial-computer-features-rs485-rs232-can-bus-dio-interfaces-dual-ethernet-optional-4g-5g-cellular-module/)

### Inferences
- With a 40-pin header, the CM5-DUAL-ETH-4G/5G-BASE should accept the 2-CH CAN FD HAT plus an I2S audio HAT, the same way a Pi 5 does. This is not tested, and there may be mechanical clearance problems inside the BOX-A case.

### Gaps
- No schematics were found for the CM5-DUAL-ETH or CM5-IO-WIRELESS bases.
- The March 2026 industrial box may have more than one CAN. Its page needs checking.

## 3. CAN products

### Takeaway
The 2-CH CAN FD HAT in **mode B** (both MCP2518FD on SPI0, INT 25/24) is the cleanest Waveshare dual-CAN option when I2S audio is also needed. The 2-CH CAN HAT+ uses SPI1 by default and would clash with I2S. The USB-CAN-FD (dual, isolated) has no mainline or official SocketCAN driver.

### Cited Findings
**2-CH CAN FD HAT (SKU 17075, $46.99)**
- MCP2518FD controllers, "up to 5KV isolated voltage" (SI8642ED digital isolator plus a B0505LS-1W isolated supply). The page lists "Raspberry Pi series boards, Jetson Nano" and does not name the Pi 5. "Up to 4-CH CAN by cascading" — [Waveshare](https://www.waveshare.com/2-ch-can-fd-hat.htm)
- The DC input is "DC 8~28V" in the spec table but "8~26V DC input" in the "What's On Board" section, so the page contradicts itself. It has a "2A fast resettable fuse" and gives no output-current rating to the Pi — [Waveshare](https://www.waveshare.com/2-ch-can-fd-hat.htm)
- Wiki: Rev2.1 (about April 2022) "can supply power to the Raspberry Pi and work stably from the DC interface" — [Waveshare wiki](https://www.waveshare.com/wiki/2-CH_CAN_FD_HAT)
- Mode A (default): CAN_0 on spi0-0, INT 25; CAN_1 on spi1-0 (SPI1_CE0 = BCM18), INT 24. Overlays: `dtoverlay=spi1-3cs`, `mcp251xfd,spi0-0,interrupt=25`, `mcp251xfd,spi1-0,interrupt=24` — [wiki](https://www.waveshare.com/wiki/2-CH_CAN_FD_HAT)
- Mode B: move four 0R resistors on the front to the "B" position. Then CAN_0 is on spi0-0 (INT 25) and CAN_1 on spi0-1 (INT 24). The wiki still lists `spi1-3cs` in the mode B example, which appears unnecessary — [wiki](https://www.waveshare.com/wiki/2-CH_CAN_FD_HAT)
- Stacking: back-side 0R resistors allow "up to 5-channel CAN interface expansion (three expansion boards)" but require mode A. CAN_1 options are SPI1 CE0/CE1/CE2 with INT 24/23/22 — [wiki](https://www.waveshare.com/wiki/2-CH_CAN_FD_HAT)
- The mainline `mcp251xfd` driver needs kernel 5.4.77 or later. Waveshare's FAQ reports CAN FD bugs in the 2024-10-28 Pi OS image with kernel 6.6.51 — [wiki](https://www.waveshare.com/wiki/2-CH_CAN_FD_HAT)
- Schematic (Rev2.1): [PDF](https://files.waveshare.com/upload/1/10/2-CH-CAN-FD-HAT-Rev2.1-Schematic.pdf)

**2-CH CAN HAT+ (isolated, MCP2515 + SN65HVD230)**
- Defaults are on SPI1: CS GPIO17 (SPI1 CE1) and GPIO16 (SPI1 CE2), INT GPIO22 and GPIO13. Overlays: `spi1-3cs`, `mcp2515,spi1-1,...,interrupt=22`, `mcp2515,spi1-2,...,interrupt=13`. SPI0 is an alternative set by pads — [Waveshare wiki](https://www.waveshare.com/wiki/2-CH_CAN_HAT+)
- Input is "5V, 7~36V". Power and digital isolation are integrated, and there is a jumpered 120 ohm terminator. Pi 5 is listed as supported. It cannot be stacked on another HAT+ without changing chip-select pads, but can stack with the 2-CH CAN HAT or the 2-CH CAN FD HAT — [wiki](https://www.waveshare.com/wiki/2-CH_CAN_HAT+). Schematic: [PDF](https://files.waveshare.com/wiki/2-CH-CAN-HAT%2B/2-CH_CAN_HAT%2B.pdf)
- The wiki does not say whether the 7-36 V input can power the Pi — [wiki](https://www.waveshare.com/wiki/2-CH_CAN_HAT+)
- The older **2-CH CAN HAT** (MCP2515 + SN65HVD230, isolated) uses spi0-0 INT 23 and spi0-1 INT 25 — [wiki HAT+ stacking section](https://www.waveshare.com/wiki/2-CH_CAN_HAT+); [product page](https://www.waveshare.com/2-ch-can-hat.htm)
- A third party (Victron) warns that the MCP2515 has only 2 RX buffers, so frames can be lost under heavy load — [Victron venus wiki](https://github.com/victronenergy/venus/wiki/RaspberryPi-CAN-Interfaces)

**Isolated RS232/RS485/CAN/CAN FD expansion board ("RS232-RS485-CAN-Board", May 2026, about $54.99)**
- It has 1x isolated CAN FD (MCP2518FD + MCP2562FD) on spi0-1, INT 24, and 1x isolated CAN (MCP2515 + SN65HVD230) on spi0-0, INT 23. It also has 2x RS485 (SC16IS752) occupying "the whole channel of SPI1" (INT 25) and 1x RS232 on the UART — [Waveshare wiki](https://www.waveshare.com/wiki/RS232-RS485-CAN-Board)
- Power: "External screw terminal DC 7-36V wide voltage input" or 5 V Type-C. It "can work with Pi5 Connector Adapter (B) to power the Raspberry Pi and the HAT", with no current rating given. It is sold as a DIN-rail or wall-mount kit for the Pi 4/5 — [wiki](https://www.waveshare.com/wiki/RS232-RS485-CAN-Board); [CNX Software](https://www.cnx-software.com/2026/05/19/60-kit-transforms-the-raspberry-pi-4-5-into-a-din-rail-industrial-controller-with-isolated-rs232-rs485-and-can-bus/)
- Schematic: [PDF](https://files.waveshare.com/wiki/RS232-RS485-CAN-Board/RS232_RS485_CAN_Board_Sch.pdf)

**USB-CAN-FD / USB-CAN-FD-B**
- The dual channels are "fully isolated, isolation voltage 3000V DC". The original USB-CAN-FD is Windows-only. The USB-CAN-FD-B adds Linux, with vendor C/Python demos ("hello_cpp") and no stated SocketCAN driver — [Waveshare wiki](https://waveshare.com/wiki/USB-CAN-FD); [product page](https://waveshare.com/usb-can-fd.htm)
- The device is not gs_usb or candleLight. A community out-of-tree SocketCAN kernel driver exists for the non-B model (USB ID 04d8:0053), giving can0/can1 with classic CAN and CAN FD — [GitHub seiga-k](https://github.com/seiga-k/waveshare-usb-can-fd-linux)
- Waveshare also makes a 2-CH CAN MiniPCIe card — [wiki](https://www.waveshare.com/wiki/2-CH_CAN_MiniPCIe). I did not evaluate it; there is no mini-PCIe slot on the target boards.

### Inferences
- Mode B of the 2-CH CAN FD HAT uses only GPIO 7-11 plus INT 24/25, which leaves GPIO18-21 free for I2S. It is the only Waveshare dual-CAN HAT that coexists with an I2S audio HAT without pad rework.
- The RS232-RS485-CAN-Board also puts both CANs on SPI0. If the SC16IS752 overlay is omitted, SPI1 and therefore GPIO18-21 should be free for I2S, but that is unverified, and only one of its CAN channels is FD.
- A 2 A fuse on the CAN FD HAT's DC input means about 2 A at 12 V (roughly 24 W input), and no 5 V output rating is published. It should not be relied on as the Pi 5's 5 A supply.
- The USB-CAN-FD would need an out-of-tree kernel module in the Yocto/AGL build. That is a maintenance burden compared with the mainline `mcp251xfd`/`mcp251x` drivers.

### Gaps
- No isolated, industrial-temperature dual MCP2518FD HAT from Waveshare beyond the CAN FD HAT was found.
- I could not confirm whether the USB-CAN-FD-B Linux package creates SocketCAN interfaces.
- No operating temperature is given for any of the CAN HATs.

## 4. Audio HATs

### Takeaway
The Waveshare I2S codec HATs (WM8960 Audio HAT, DA7212 Audio HAT (B)) offer playback plus **onboard MEMS mics only**, with no external mic input. They use I2S on GPIO18-21, which conflicts with CAN FD HAT mode A and the CM5-HMI's onboard CAN. Driver support is a vendor script for the WM8960 and unclear for the DA7212.

### Cited Findings
- **WM8960 Audio HAT**: I2S on BCM18 (BCLK), 19 (LRCLK), 20 (ADC in), 21 (DAC out), I2C on 2/3, and a button on GPIO17. Speaker 1 W per channel at 8 ohm BTL, headphone 40 mW. Dual MEMS mics. "doesn't feature external MIC input pins even on the 3.5mm jack" — [Waveshare wiki](https://www.waveshare.com/wiki/WM8960_Audio_HAT)
- Driver: vendor `install.sh` and a DKMS module from github.com/waveshare/WM8960-Audio-HAT, with no dtoverlay named. The Pi 5 install has known problems ("vc4-kms-v3d ... not supported on bcm2712"), and Waveshare's fix is a fresh Bookworm image with an attached script — [wiki](https://www.waveshare.com/wiki/WM8960_Audio_HAT)
- **DA7212 Audio HAT (B)** (SKU 35338): DA7212 codec at up to 96 kHz/24-bit, I2C1 address 0x1A, I2S. 3.5 mm headphone, speaker screw terminal and PH2.0 output, onboard dual mics, and an AUX **input** but no mic jack. Pi 5 support and driver are not stated on the overview — [docs.waveshare.com](https://docs.waveshare.com/DA7212-Audio-HAT-B)
- The **M.2 PCM5122 Audio HAT** is playback-only (headphone plus RCA), with no microphone — [Waveshare](https://www.waveshare.com/product/raspberry-pi/hats/ad-da-audio-sensors/m.2-pcm5122-audio-hat.htm)

### Inferences
- From general kernel knowledge, not a Waveshare source: mainline Linux has a `wm8960` codec driver and a `da7213` driver (which covers the DA7212). A Yocto image could use simple-audio-card or an upstream overlay instead of Waveshare's DKMS script, but this needs verification on the Pi 5.
- For a vehicle mic (a cabled boom or visor mic), none of these HATs has a usable external mic input. The current USB audio adapter (Sound Blaster Play! 3) remains the simplest route.

### Gaps
- No Waveshare I2S HAT with an external mic jack was found.
- No mainline dtoverlay name is documented by Waveshare for either codec on the Pi 5.

## 5. DSI displays

### Takeaway
Waveshare's DSI touch panels top out at about 450-500 cd/m2. No sunlight-readable (1000 nit) or wide-temperature Waveshare DSI panel was found. Optically bonded options are the "-A" DSI-TOUCH series and the HMIs.

### Cited Findings
- **10.1-DSI-TOUCH-B** (SKU 32603, $64.99): 500 cd/m2, toughened glass (6H), **not optically bonded**, powered from the GPIO 5 V pin, I2C capacitive 10-point touch. The spec says 720x1280 but the title says 800x1280 — [Waveshare](https://www.waveshare.com/10.1-dsi-touch-b.htm)
- Comparison table on the same page: 10.1-DSI-TOUCH-A (800x1280, toughened glass plus optical bonding), 8-DSI-TOUCH-A (800x1280, optical), 7-DSI-TOUCH-A (720x1280, optical), 7-DSI-TOUCH-C (1024x600, neither) — [Waveshare](https://www.waveshare.com/10.1-dsi-touch-b.htm)
- **10.1inch DSI LCD (C)**: 1280x800, IPS, Pi 5/4B/CM support. Its wiki gives an operating temperature of 0/25/60 — [Waveshare wiki](https://www.waveshare.com/wiki/10.1inch_DSI_LCD_%28C%29)
- PI5-HMI-7A panel: 450 cd/m2 — [Waveshare](https://www.waveshare.com/pi5-hmi-7a.htm)

### Inferences
- For a head unit, the 10.1-DSI-TOUCH-A (optically bonded) is probably preferable to the B already in use, because bonding reduces internal reflections in sunlight. Neither is sunlight-readable.
- The overlay names `vc4-kms-dsi-waveshare-panel-v2` (DSI-TOUCH series) and `vc4-kms-dsi-waveshare-panel` (older DSI LCDs and HMIs) are in the Raspberry Pi kernel tree rather than vanilla mainline. That point comes from the brief and was not re-verified here.

### Gaps
- No brightness figure for the DSI-TOUCH-A panels, no operating temperature for the DSI-TOUCH series, and no list of mainline (non-RPi-tree) panel drivers.

## 6. Power (12 V vehicle to 5 V / 5 A)

### Takeaway
I found no Waveshare HAT that takes 12 V and is rated to supply a Pi 5 at 5 A. Waveshare's wide-input (7-36 V) products are the CM5 carriers and HMIs, the RS232-RS485-CAN-Board and the CAN HAT+. None documents automotive transient (load-dump) protection or an ignition-sense input. The CM5-HMI/BOX supercap gives power-loss warning GPIOs, the closest thing to safe shutdown.

### Cited Findings
- Search turned up PoE HAT (G)/(H) (38-57 V PoE in, 5 V 5 A out) and UPS HAT (E) (5 V 6 A battery UPS, Type-C charging), but no 7-36 V input Pi 5 HAT — [PoE HAT (G)](https://www.waveshare.com/poe-hat-g.htm); [UPS HAT (E) wiki](https://www.waveshare.com/wiki/UPS_HAT_(E))
- CM5-HMI: 7-36 V input, supercap "power-loss hold-up", GPIO16 external power-cut warning, GPIO11 capacitor low-voltage warning. It ships with a 12 V 4 A PSU — [docs.waveshare.com](https://docs.waveshare.com/CM5-HMI-7A-ACCE); [Waveshare](https://www.waveshare.com/cm5-hmi-xa-acce.htm)
- The CM5-DUAL-ETH-4G/5G-BASE takes 7-36 V on a DC jack — [Waveshare](https://www.waveshare.com/cm5-dual-eth-4g-5g-base.htm)
- The RS232-RS485-CAN-Board takes 7-36 V and powers the Pi through the Pi5 Connector Adapter (B), with no current rating — [wiki](https://www.waveshare.com/wiki/RS232-RS485-CAN-Board)
- The 2-CH CAN FD HAT takes 8-28 V (or 8-26 V; the page conflicts) with a 2 A fuse and can power the Pi — [Waveshare](https://www.waveshare.com/2-ch-can-fd-hat.htm)

### Inferences
- The upper limit of 7-36 V covers a 12 V system's normal range, but a 12 V load dump (ISO 7637-2 / ISO 16750-2 pulse 5a, often 80-100 V unsuppressed) is outside it. An external automotive front end (TVS plus surge stopper) or a third-party vehicle PSU is still needed in front of any Waveshare product.
- The supercap power-cut GPIO on the CM5-HMI/BOX can be wired into a systemd shutdown service when power drops. It is not an ignition-sense input, because it fires on supply loss rather than on ACC.

### Gaps
- No Waveshare documentation of transient or reverse-polarity protection levels, supercap hold-up time, or current rating to the Pi for any 7-36 V board.

## 7. Proposed Waveshare-only combinations with two CAN buses

### Takeaway
Three viable Waveshare-only builds give 2x CAN, audio in/out, NVMe and a 12 V input. Option A (CM5 + DUAL-ETH base + CAN FD HAT mode B + I2S audio) is the cleanest on Linux drivers. Option C (CM5-HMI + USB-CAN) is the most integrated but depends on an out-of-tree USB-CAN driver.

### Cited Findings
- Component facts are as cited in sections 1-6 above: [CM5-DUAL-ETH-4G/5G-BASE](https://www.waveshare.com/cm5-dual-eth-4g-5g-base.htm), [2-CH CAN FD HAT wiki](https://www.waveshare.com/wiki/2-CH_CAN_FD_HAT), [WM8960 wiki](https://www.waveshare.com/wiki/WM8960_Audio_HAT), [CM5-HMI docs](https://docs.waveshare.com/CM5-HMI-7A-ACCE), [RS232-RS485-CAN-Board wiki](https://www.waveshare.com/wiki/RS232-RS485-CAN-Board), [USB-CAN-FD wiki](https://waveshare.com/wiki/USB-CAN-FD)

### Inferences
**Option A: CM5 + CM5-DUAL-ETH-4G/5G-BASE (or BOX-A) + 10.1-DSI-TOUCH-A + 2-CH CAN FD HAT (mode B) + WM8960 or DA7212 audio HAT (or keep USB audio)**
- 12 V: the base's own 7-36 V jack. CAN: 2x isolated MCP2518FD on SPI0 (INT 25/24) with mainline `mcp251xfd`. Audio: I2S on GPIO18-21 is free in mode B. NVMe: onboard M-key. Display: one of the two MIPI ports.
- Open questions:
  - It has only 2 USB-A ports. Two RTL-SDRs plus USB knobs plus optional USB audio need a powered USB hub.
  - Whether two stacked HATs fit, mechanically and thermally, in or on the base (the BOX-A case will not take them).
  - The WM8960 has no external mic input, so a cabled mic still needs USB audio.
  - No published operating temperature and no load-dump spec.
  - The 10.1-DSI-TOUCH power needs 5 V from the header.

**Option B: Pi 5 + RPI-HMI-101D-ACCE(-B) enclosure (or the existing 10.1-DSI-TOUCH) + RS232-RS485-CAN-Board + Pi5 Connector Adapter (B) + Waveshare PCIe-to-M.2 NVMe + USB audio**
- 12 V: the board's 7-36 V terminal powers the Pi via the adapter. CAN: 1x CAN FD (MCP2518FD, spi0-1) and 1x classic (MCP2515, spi0-0), both isolated. The RS485 SPI1 overlay can be left out.
- Open questions:
  - Current rating to the Pi 5 (5 A?) is unpublished.
  - The board's form factor and DIN enclosure will not fit inside the HMI's ~19 mm HAT space, and the HMI itself is 5 V-only.
  - The MCP2515 channel has 2 RX buffers.
  - Whether the Pi 5 PCIe is shared between the board's "reserved PCIe connector" and the NVMe adapter.
- Variant B2: the same Pi 5 and 101D HMI with the **2-CH CAN FD HAT mode B** inside the HMI's HAT space, plus a separate 12 V to 5 V/5 A converter, because the HAT's 2 A-fused input is not a documented 5 A Pi 5 supply. Open question: whether the CAN FD HAT plus the NVMe adapter fit within the 19 mm clearance.

**Option C: CM5-HMI-10.1A-ACCE (7-36 V, 1 isolated MCP2515 CAN, NVMe, supercap, RTC) + second CAN via Waveshare USB-CAN-FD (dual isolated) + USB audio (Sound Blaster Play! 3)**
- The most integrated, rugged-cased option, with a 12 V PSU, optical bonding and power-loss warning GPIOs. It gives three CAN channels in total if both USB-CAN channels are used, or the USB-CAN-FD's two channels can be used and the onboard one ignored.
- Open questions:
  - The USB-CAN-FD has no mainline or official SocketCAN driver: Waveshare says it is Windows-only, the B model ships a vendor demo, and only the community `waveshare-usb-can-fd-linux` module exists for the non-B. That is a maintenance risk in AGL/Yocto.
  - There are only 4 USB ports for the USB-CAN, Sound Blaster, 2 RTL-SDRs and knob dials, so a hub is needed.
  - The panel is portrait 800x1280 and needs rotation.
  - No schematic, and no internal SPI/I2S breakout for an internal second controller: SPI1 is used by the onboard CAN, and GPIO7/GPIO11 are used on SPI0.
  - No operating temperature is published.
- A non-Waveshare gs_usb adapter (covered by another researcher) would remove the driver risk.

### Gaps
- None of the combinations has been tested together. HAT stack heights, thermal behaviour at in-cab temperatures and USB power budget (two RTL-SDRs at about 300 mA each) are unverified.
- No Waveshare product in any combination publishes automotive-grade transient protection or an operating range beyond 0-60 deg C.
