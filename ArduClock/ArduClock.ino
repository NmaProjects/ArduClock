#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int h = 12, m = 0, s = 0;
String ampm = "--";
unsigned long lastTick = 0;
bool hasSynced = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  CURRENT TIME: ");
  updateDisplay();
}

void loop() {
  // 1. SYNC FROM PC
  if (Serial.available() > 0) {
    String timeString = Serial.readStringUntil('\n'); 
    
    if (timeString.length() >= 8) {
      h = timeString.substring(0, 2).toInt();
      m = timeString.substring(3, 5).toInt();
      s = timeString.substring(6, 8).toInt();
      
      if (timeString.indexOf("PM") != -1 || timeString.indexOf("pm") != -1) {
        ampm = "PM";
      } else {
        ampm = "AM";
      }
      
      hasSynced = true; 
      lastTick = millis(); 
      updateDisplay();           
    }
    while(Serial.available() > 0) { Serial.read(); } 
  }

  // 2. SEAMLESS TICKING WITH AM/PM ROLLOVER
  if (millis() - lastTick >= 1000) {
    lastTick += 1000;
    
    if (hasSynced) {
      s++;
      if (s >= 60) { s = 0; m++; }
      if (m >= 60) { m = 0; h++; }
      
      // AM/PM ROLLOVER LOGIC
      if (h == 12 && m == 0 && s == 0) {
        if (ampm == "AM") ampm = "PM";
        else if (ampm == "PM") ampm = "AM";
      }
      
      if (h > 12) { h = 1; }
      
      updateDisplay();
    }
  }
}

void updateDisplay() {
  lcd.setCursor(2, 1);
  if (!hasSynced) {
    lcd.print("--:--:-- --");
  } else {
    if (h < 10) lcd.print("0");
    lcd.print(h);
    lcd.print(":");
    if (m < 10) lcd.print("0");
    lcd.print(m);
    lcd.print(":");
    if (s < 10) lcd.print("0");
    lcd.print(s);
    lcd.print(" ");
    lcd.print(ampm);
  }
}
