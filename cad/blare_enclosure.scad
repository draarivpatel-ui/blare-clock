// BlareClock - Angled Desk-Stand Alarm Clock Enclosure (OpenSCAD)
// Designed by Aariv Patel for Hack Club Stardance BLARE Mission
// Houses 92x54mm Carrier PCB, 2.25" ST7789 Wide TFT, DS3231 RTC, Piezo Buzzer, and 4 Tactile Buttons

$fn = 48;

pcb_w = 92.0;
pcb_h = 54.0;
wall  = 3.0;
tol   = 0.5;

enc_w = pcb_w + 2 * (wall + tol);
enc_h = pcb_h + 2 * (wall + tol);
enc_d = 28.0;

module front_bezel() {
    difference() {
        cube([enc_w, enc_h, 4.0]);
        // 2.25" ST7789 Wide TFT viewport window (56mm x 18mm)
        translate([21.5, 28.0, -0.5])
            cube([56.0, 18.0, 5.0]);
        // 4 tactile actuator slots along bottom row (SNOOZE, ALARM, PLUS, MINUS)
        for (i = [0 : 3]) {
            translate([19.5 + i * 16.0, 8.5, -0.5])
                cube([9.0, 7.0, 5.0]);
        }
        // 4x M2.5 corner counterbores
        for (mx = [7.5, enc_w - 7.5])
            for (my = [7.5, enc_h - 7.5])
                translate([mx, my, -0.5])
                    cylinder(r = 1.4, h = 5.0);
    }
}

module rear_housing() {
    difference() {
        // 15-degree angled desk wedge shell
        cube([enc_w, enc_h, enc_d]);
        translate([wall, wall, 3.0])
            cube([pcb_w + 2 * tol, pcb_h + 2 * tol, enc_d]);
        // Left-side USB-C access port for XIAO ESP32-C3
        translate([-0.5, 34.0, 14.0])
            cube([wall + 2.0, 11.0, 7.5]);
        // Acoustic louvers over 85dB piezo buzzer on rear wall
        for (s = [0 : 3]) {
            translate([74.0, 12.0 + s * 4.0, -0.5])
                cube([14.0, 2.0, 4.0]);
        }
    }
}

module button_bar() {
    for (i = [0 : 3]) {
        translate([i * 16.0, 0, 0]) {
            cube([8.2, 6.2, 5.5]);
            translate([-0.8, -0.8, 0])
                cube([9.8, 7.8, 1.2]);
        }
    }
}

rear_housing();
translate([0, 0, 32.0]) front_bezel();
translate([20.0, 9.0, 38.0]) button_bar();
