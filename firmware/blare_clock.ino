// BlareClock v1.0 - Smart Desk Alarm Clock Firmware
// Target MCU: Seeed Studio XIAO ESP32-C3 (RISC-V 160MHz + Wi-Fi NTP + Hardware RTC)
// Pinout matches pcb/blare_clock.kicad_pcb:
//   SW1 (BTN_SNOOZE): D0 (GPIO2)
//   SW2 (BTN_ALARM):  D1 (GPIO3)
//   SW3 (BTN_PLUS):   D2 (GPIO4)
//   SW4 (BTN_MINUS):  D3 (GPIO5)
//   DS3231 I2C RTC:   SDA = D4 (GPIO6), SCL = D5 (GPIO7)
//   Piezo Buzzer:     PWM = D6 (GPIO21)
//   ST7789 2.25" TFT: DC = D7 (GPIO20), SCK = D8 (GPIO8), CS = D9 (GPIO9), MOSI = D10 (GPIO10)

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define PIN_BTN_SNOOZE  2   // D0
#define PIN_BTN_ALARM   3   // D1
#define PIN_BTN_PLUS    4   // D2
#define PIN_BTN_MINUS   5   // D3
#define PIN_I2C_SDA     6   // D4
#define PIN_I2C_SCL     7   // D5
#define PIN_BUZZER      21  // D6
#define PIN_TFT_DC      20  // D7
#define PIN_TFT_SCK     8   // D8
#define PIN_TFT_CS      9   // D9
#define PIN_TFT_MOSI    10  // D10
#define PIN_TFT_RST     -1  // Tied to 3V3 on carrier PCB

// Wi-Fi credentials for automatic NTP synchronization (falls back to DS3231 RTC offline)
const char* WIFI_SSID     = "Stardance-Lab";
const char* WIFI_PASSWORD = "";
const char* NTP_SERVER    = "pool.ntp.org";
const long  GMT_OFFSET_S  = -18000; // US Central (CDT/CST)
const int   DST_OFFSET_S  = 3600;

Adafruit_ST7789 tft = Adafruit_ST7789(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
RTC_DS3231 rtc;
bool rtcAvailable = false;
bool ntpSynced    = false;

enum ClockState {
  STATE_CLOCK_NORMAL = 0,
  STATE_EDIT_ALARM_HOUR,
  STATE_EDIT_ALARM_MIN,
  STATE_ALARM_RINGING,
  STATE_SNOOZING
};

ClockState currentState = STATE_CLOCK_NORMAL;
int alarmHour   = 7;
int alarmMinute = 0;
bool alarmArmed = true;
int snoozeUntilMinute = -1;
int snoozeUntilHour   = -1;

unsigned long lastTickMs    = 0;
unsigned long alarmStartMs  = 0;
unsigned long lastButtonMs  = 0;

// Escalating piezo melody frequencies (Hz)
const int MELODY_NOTES[] = { 1047, 1319, 1568, 2093 }; // C6, E6, G6, C7
int melodyIndex = 0;

void playTone(int freqHz, int durationMs) {
  tone(PIN_BUZZER, freqHz, durationMs);
}

void syncTimeFromNtp() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 4500) {
    delay(150);
  }
  if (WiFi.status() == WL_CONNECTED) {
    configTime(GMT_OFFSET_S, DST_OFFSET_S, NTP_SERVER);
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 2000)) {
      ntpSynced = true;
      if (rtcAvailable) {
        rtc.adjust(DateTime(
          timeinfo.tm_year + 1900,
          timeinfo.tm_mon + 1,
          timeinfo.tm_mday,
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec
        ));
      }
    }
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
}

DateTime readCurrentTime() {
  if (rtcAvailable) {
    return rtc.now();
  }
  unsigned long elapsedSec = millis() / 1000;
  int hh = (7 + (elapsedSec / 3600)) % 24;
  int mm = (elapsedSec / 60) % 60;
  int ss = elapsedSec % 60;
  return DateTime(2026, 10, 25, hh, mm, ss);
}

void renderDisplay(const DateTime& now) {
  tft.fillScreen(ST77XX_BLACK);

  // Top telemetry bar
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(6, 4);
  tft.print(ntpSynced ? "NTP+DS3231 SYNC" : "DS3231 RTC MODE");

  tft.setCursor(184, 4);
  if (currentState == STATE_ALARM_RINGING) {
    tft.setTextColor(ST77XX_RED);
    tft.print("!! BLARE RINGING !!");
  } else if (currentState == STATE_SNOOZING) {
    tft.setTextColor(ST77XX_YELLOW);
    tft.print("SNOOZE ACTIVE (+9M)");
  } else if (alarmArmed) {
    tft.setTextColor(ST77XX_GREEN);
    char buf[24];
    snprintf(buf, sizeof(buf), "ALM %02d:%02d ON", alarmHour, alarmMinute);
    tft.print(buf);
  } else {
    tft.setTextColor(0x7BEF);
    tft.print("ALM DISARMED");
  }

  // Main retro clock digits
  char timeStr[16];
  if (currentState == STATE_EDIT_ALARM_HOUR || currentState == STATE_EDIT_ALARM_MIN) {
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:00", alarmHour, alarmMinute);
    tft.setTextColor(ST77XX_YELLOW);
  } else {
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    tft.setTextColor(currentState == STATE_ALARM_RINGING ? ST77XX_RED : ST77XX_WHITE);
  }

  tft.setTextSize(4);
  tft.setCursor(22, 24);
  tft.print(timeStr);

  // Bottom button legend
  tft.setTextSize(1);
  tft.setTextColor(0xAD55);
  tft.setCursor(6, 64);
  tft.print("[D0:SNOOZE]   [D1:ALARM]   [D2:PLUS+]   [D3:MINUS-]");
}

void handleButtons(const DateTime& now) {
  if (millis() - lastButtonMs < 180) return;

  bool bSnooze = (digitalRead(PIN_BTN_SNOOZE) == LOW);
  bool bAlarm  = (digitalRead(PIN_BTN_ALARM)  == LOW);
  bool bPlus   = (digitalRead(PIN_BTN_PLUS)   == LOW);
  bool bMinus  = (digitalRead(PIN_BTN_MINUS)  == LOW);

  if (!bSnooze && !bAlarm && !bPlus && !bMinus) return;
  lastButtonMs = millis();

  if (bSnooze) {
    playTone(880, 45);
    if (currentState == STATE_ALARM_RINGING) {
      noTone(PIN_BUZZER);
      currentState = STATE_SNOOZING;
      int totalMin = now.hour() * 60 + now.minute() + 9;
      snoozeUntilHour   = (totalMin / 60) % 24;
      snoozeUntilMinute = totalMin % 60;
    } else {
      alarmArmed = !alarmArmed;
      if (!alarmArmed) currentState = STATE_CLOCK_NORMAL;
    }
  } else if (bAlarm) {
    playTone(1175, 45);
    if (currentState == STATE_ALARM_RINGING) {
      noTone(PIN_BUZZER);
      currentState = STATE_CLOCK_NORMAL;
    } else if (currentState == STATE_CLOCK_NORMAL) {
      currentState = STATE_EDIT_ALARM_HOUR;
    } else if (currentState == STATE_EDIT_ALARM_HOUR) {
      currentState = STATE_EDIT_ALARM_MIN;
    } else {
      currentState = STATE_CLOCK_NORMAL;
      alarmArmed = true;
    }
  } else if (bPlus) {
    playTone(1319, 35);
    if (currentState == STATE_EDIT_ALARM_HOUR) {
      alarmHour = (alarmHour + 1) % 24;
    } else {
      alarmMinute = (alarmMinute + 1) % 60;
    }
  } else if (bMinus) {
    playTone(988, 35);
    if (currentState == STATE_EDIT_ALARM_HOUR) {
      alarmHour = (alarmHour + 23) % 24;
    } else {
      alarmMinute = (alarmMinute + 59) % 60;
    }
  }
  renderDisplay(now);
}

void setup() {
  pinMode(PIN_BTN_SNOOZE, INPUT_PULLUP);
  pinMode(PIN_BTN_ALARM,  INPUT_PULLUP);
  pinMode(PIN_BTN_PLUS,   INPUT_PULLUP);
  pinMode(PIN_BTN_MINUS,  INPUT_PULLUP);
  pinMode(PIN_BUZZER,     OUTPUT);

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  rtcAvailable = rtc.begin();

  SPI.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, PIN_TFT_CS);
  tft.init(76, 284);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  syncTimeFromNtp();
  playTone(1568, 80);
}

void loop() {
  DateTime now = readCurrentTime();
  handleButtons(now);

  if (millis() - lastTickMs >= 500) {
    lastTickMs = millis();

    // Check alarm trigger
    if (alarmArmed && currentState == STATE_CLOCK_NORMAL) {
      if (now.hour() == alarmHour && now.minute() == alarmMinute && now.second() == 0) {
        currentState = STATE_ALARM_RINGING;
        alarmStartMs = millis();
      }
    } else if (currentState == STATE_SNOOZING) {
      if (now.hour() == snoozeUntilHour && now.minute() == snoozeUntilMinute && now.second() == 0) {
        currentState = STATE_ALARM_RINGING;
        alarmStartMs = millis();
      }
    }

    // Drive escalating piezo buzzer when ringing
    if (currentState == STATE_ALARM_RINGING) {
      int note = MELODY_NOTES[melodyIndex % 4];
      melodyIndex++;
      playTone(note, 220);
    }

    renderDisplay(now);
  }
}
