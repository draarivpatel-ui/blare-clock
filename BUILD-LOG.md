# BlareClock build log

## 2026-10-25 - Carrier PCB, angled desk enclosure, ESP32-C3 NTP/RTC firmware, and simulator

1. **Schematic and GPIO mapping (`pcb/blare_clock.kicad_sch`)**:
   - Designed the carrier circuit around a Seeed Studio XIAO ESP32-C3 (`U1`), a 2.25" ST7789 wide-aspect SPI TFT display (`J1`, 284x76 IPS), a DS3231 temperature-compensated I2C real-time clock module with CR2032 backup (`U2`), a 12mm 85dB passive piezo transducer (`BZ1`), and four 6x6mm tactile push switches (`SW1`..`SW4`).
   - Mapped the four tactile buttons (`SNOOZE`, `ALARM`, `PLUS`, `MINUS`) to `D0`..`D3` (`GPIO2`..`GPIO5`), the DS3231 RTC to hardware I2C on `D4`/`D5` (`GPIO6`/`GPIO7`), the piezo buzzer to PWM output `D6` (`GPIO21`), and the ST7789 TFT display to hardware SPI on `D7`..`D10` (`DC=GPIO20`, `SCK=GPIO8`, `CS=GPIO9`, `MOSI=GPIO10`).
2. **2-layer PCB routing and Gerbers (`pcb/blare_clock.kicad_pcb`, `pcb/gerbers/blare-clock-gerbers.zip`)**:
   - Routed a 92.0mm x 54.0mm 2-layer FR4 carrier PCB with four M2.5 mounting holes at 4.0mm corner offsets, placing the wide ST7789 display window top-center above the four front-accessible tactile switches.
   - Kept high-speed 40MHz SPI clock/MOSI lines and the buzzer PWM trace short on `F.Cu`, routed button returns and I2C lines on `B.Cu`, and poured dual-sided `GND` planes.
   - Generated complete Gerber RS-274X layers and Excellon drill file zipped in `pcb/gerbers/blare-clock-gerbers.zip`.
3. **Angled desk-stand enclosure (`cad/blare_enclosure.scad`, `cad/*.stl`)**:
   - Modeled a 15-degree angled desktop housing (`rear_housing.stl`) with left-side USB-C cutout and rear acoustic louvers over the piezo buzzer, a recessed window frame (`front_bezel.stl`) for the 2.25" ST7789 TFT, and a captive actuator bar (`button_bar.stl`) for the four tactile switches.
4. **ESP32-C3 firmware and browser hardware simulator (`firmware/blare_clock.ino`, `index.html`)**:
   - Implemented Wi-Fi NTP synchronization on boot that automatically disciplines the DS3231 hardware RTC, a five-state alarm/snooze state machine (`STATE_CLOCK_NORMAL`, `STATE_EDIT_ALARM_HOUR`, `STATE_EDIT_ALARM_MIN`, `STATE_ALARM_RINGING`, `STATE_SNOOZING`), and a four-note escalating piezo melody (`C6`..`C7`).
   - Built `index.html` with a live playable 2.25" ST7789 display, Web Audio piezo alarm synthesizer, interactive 3D exploded CAD viewer, and 2-layer PCB layer inspector.
