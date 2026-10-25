# BlareClock - XIAO ESP32-C3 Desk Alarm Clock with 2.25" ST7789 TFT & DS3231 RTC

A custom digital desk alarm clock powered by a **Seeed Studio XIAO ESP32-C3**, a wide-aspect **2.25" ST7789 SPI TFT display (284x76 IPS)**, a **DS3231 temperature-compensated I2C real-time clock** with CR2032 battery backup, an **85dB passive piezo buzzer**, and **four front-facing tactile push buttons**, built for the Hack Club Stardance **BLARE** mission.

![BlareClock Interactive Simulator and Hardware Inspector](screenshot.png)

Live Interactive Simulator & Hardware Inspector: https://aariv.me/blare-clock/

## Repository Structure

- `pcb/blare_clock.kicad_pro`, `pcb/blare_clock.kicad_sch`, `pcb/blare_clock.kicad_pcb`: Complete KiCad 8 project, schematic, and routed 2-layer 92.0mm x 54.0mm carrier PCB with M2.5 mounting holes and dual-layer ground pours.
- `pcb/gerbers/blare-clock-gerbers.zip`: Fabrication-ready Gerber RS-274X layers (`F_Cu.gtl`, `B_Cu.gbl`, `F_Mask.gts`, `B_Mask.gbs`, `F_SilkS.gto`, `B_SilkS.gbo`, `Edge_Cuts.gm1`) and Excellon drill file (`blare_clock.drl`) ready for JLCPCB.
- `pcb/bom.csv`: Complete Bill of Materials with manufacturer part numbers and footprints.
- `cad/blare_enclosure.scad`, `cad/front_bezel.stl`, `cad/rear_housing.stl`, `cad/button_bar.stl`: Parametric OpenSCAD 15-degree angled desk-stand enclosure and triangulated STL meshes (~72g PLA total).
- `firmware/blare_clock.ino`: ESP32-C3 Arduino firmware featuring automatic Wi-Fi NTP time synchronization that disciplines the DS3231 RTC, ST7789 284x76 wide-screen rendering, configurable daily alarm and 9-minute snooze state machine, and escalating 4-tone piezo melody output.
- `index.html`: Interactive browser simulator and hardware inspector featuring a live playable 2.25" ST7789 TFT clock, working `SNOOZE`, `ALARM SET`, `PLUS +`, `MINUS -`, and `TRIGGER 85dB PIEZO ALARM` buttons with Web Audio piezo tones, 3D exploded CAD viewer, and 2-layer PCB viewer.

## XIAO ESP32-C3 Pinout & Carrier Wiring

| Seeed XIAO ESP32-C3 Pin | Net Name | Connected Hardware |
| --- | --- | --- |
| `D0 / GPIO2` | `BTN_SNOOZE` | `SW1` Tactile Button (Snooze +9m / Toggle Arm) |
| `D1 / GPIO3` | `BTN_ALARM` | `SW2` Tactile Button (Cycle Set Hour / Set Minute) |
| `D2 / GPIO4` | `BTN_PLUS` | `SW3` Tactile Button (Increment Hour / Minute) |
| `D3 / GPIO5` | `BTN_MINUS` | `SW4` Tactile Button (Decrement Hour / Minute) |
| `D4 / GPIO6 (SDA)` | `I2C_SDA` | `U2` DS3231 RTC Module I2C Data (`0x68`) |
| `D5 / GPIO7 (SCL)` | `I2C_SCL` | `U2` DS3231 RTC Module I2C Clock |
| `D6 / GPIO21` | `BUZZER_PWM` | `BZ1` 12mm 85dB Passive Piezo Transducer |
| `D7 / GPIO20` | `TFT_DC` | `J1` 2.25" ST7789 SPI TFT Data/Command |
| `D8 / GPIO8 (SCK)` | `TFT_SCK` | `J1` 2.25" ST7789 SPI TFT Serial Clock |
| `D9 / GPIO9` | `TFT_CS` | `J1` 2.25" ST7789 SPI TFT Chip Select |
| `D10 / GPIO10 (MOSI)` | `TFT_MOSI` | `J1` 2.25" ST7789 SPI TFT Serial Data In |
| `3V3 / 5V / GND` | Power | Common `3V3` logic/backlight rail and stitched `GND` pour |

## Bill of Materials

| Ref | Qty | Component | Footprint / Package |
| --- | --- | --- | --- |
| `U1` | 1 | Seeed Studio XIAO ESP32-C3 | 14-Pin THT/SMD Module |
| `J1` | 1 | 2.25" ST7789 Wide SPI TFT LCD (284x76 IPS) | 8-Pin 2.54mm Header |
| `U2` | 1 | DS3231 Precision I2C RTC Module + CR2032 | 4-Pin 2.54mm Header |
| `BZ1` | 1 | 12mm 85dB Passive Piezo Buzzer (`PS1240P02BT`) | `MagneticBuzzer_12x9.5mm_P6.5mm` |
| `SW1-SW4` | 4 | 6x6x7mm Omron Tactile Push Buttons (`B3F-1050`) | `SW_PUSH_6mm` |
| `H1-H4` | 4 | M2.5x8mm Screws + M2.5 Brass Heat-Set Inserts | 2.5mm Mounting Holes |

## Why I built this

Using my phone as an alarm clock meant keeping notifications within arm's reach of my bed every night. I designed `BlareClock` to be a dedicated, zero-distraction hardware desk clock that syncs over Wi-Fi NTP once on boot, keeps sub-second accuracy through power outages using a battery-backed DS3231 TCXO real-time clock, displays crisp high-contrast digits on a wide 2.25" ST7789 bar screen, and wakes me up reliably with physical tactile buttons and an 85dB piezo cascade.

## Honest AI note

Built by AI coding agents (ZCode) under Aariv Patel's direction for the Hack Club Stardance BLARE alarm clock mission: Aariv specified the XIAO ESP32-C3 + 2.25" ST7789 + DS3231 hardware stack and reviewed the resulting PCB layout, CAD enclosure, firmware, and interactive simulator.

## License

MIT.
