#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long energy = 0;
unsigned long lastTime = 0;

int oldVoltage = 0;
int oldCurrent = 0;
float voltageCalibration = 1.0;
float currentCalibration = 1.0;

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lastTime = millis();
}

void loop() {
  int v = analogRead(A0);
  int c = analogRead(A1);

  long sumV = 0;
  long sumC = 0;

  for (int i = 0; i < 10; i++) {
    int sv = analogRead(A0);
    int sc = analogRead(A1);

    sumV = sumV + (long)sv * sv;
    sumC = sumC + (long)sc * sc;

    delay(2);
  }

  long meanV = sumV / 10;
  long meanC = sumC / 10;

  long rmsV = meanV;
  long rmsC = meanC;

  for (int i = 0; i < 5; i++) {
    rmsV = (rmsV + meanV / rmsV) / 2;
    rmsC = (rmsC + meanC / rmsC) / 2;
  }

 int voltage = ((rmsV * 230) / 4095) * voltageCalibration;
 int current = ((rmsC * 10) / 4095) * currentCalibration;

  int filteredVoltage = (oldVoltage * 3 + voltage) / 4;
  int filteredCurrent = (oldCurrent * 3 + current) / 4;

  oldVoltage = filteredVoltage;
  oldCurrent = filteredCurrent;

  int power = filteredVoltage * filteredCurrent;
  Serial.print("Vrms: ");
  Serial.print(filteredVoltage);
  Serial.print(" V | Irms: ");
  Serial.print(filteredCurrent);
  Serial.print(" A | Power: ");
  Serial.print(power);
  Serial.println(" W");

  unsigned long now = millis();
  unsigned long elapsed = now - lastTime;

  energy = energy + (power * elapsed) / 3600;
  lastTime = now;

  lcd.setCursor(0, 0);
  lcd.print("SMART ENERGY");

  lcd.setCursor(0, 1);
  lcd.print("Vrms: ");
  lcd.print(filteredVoltage);
  lcd.print(" V   ");

  lcd.setCursor(0, 2);
  lcd.print("Irms: ");
  lcd.print(filteredCurrent);
  lcd.print(" A   ");

  lcd.setCursor(0, 3);
  lcd.print("Power: ");
  lcd.print(power);
  lcd.print(" W   ");

  delay(500);
}