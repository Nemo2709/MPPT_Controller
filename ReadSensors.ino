// === ReadSensors.ino ===
// Only raw sensor readings — no filtering

// === Constants ===
const float adcRef = 3.3;
const float resolution = 4095.0;

// === Voltage Divider Settings ===
const float voltageDividerRatioSol = 9.9/(90.3+9.9);
const float voltageDividerRatioBat = 10/(251.4+10);
const float voltageCalibrationFactor = 1.14;  //

// === Current Sensor Settings ===
const float currentOffsetSol = 0.19;
const float currentOffsetBat = 0.21;
const float currentSensitivity = 0.150;

// === Global measurement variables ===
float Vin = 0.0;
float Iin = 0.0;
float Vout = 0.0;
float Iout = 0.0;

// === Read Solar Voltage (Vin) ===
float readSolarVoltage(int pin) {
  int adcVal = analogRead(pin);
  float vOut = (adcVal / resolution) * adcRef;
  float rawVin = vOut / voltageDividerRatioSol;
  return rawVin * voltageCalibrationFactor;
}

// === Read Battery Voltage (Vout) ===
float readBatteryVoltage(int pin) {
  int adcVal = analogRead(pin);
  float vOut = (adcVal / resolution) * adcRef;
  float rawVout = vOut / voltageDividerRatioBat;
  return rawVout * voltageCalibrationFactor;
}

// === Read Solar Current (Iin) — raw only ===
float readSolarCurrent(int pin) {
  int adcVal = analogRead(pin);
  float voltage = (adcVal / resolution) * adcRef;
  return (voltage - currentOffsetSol) / currentSensitivity;
}

// === Read Battery Current (Iout) — raw only ===
float readBatteryCurrent(int pin) {
  int adcVal = analogRead(pin);
  float voltage = (adcVal / resolution) * adcRef;
  return (voltage - currentOffsetBat) / currentSensitivity;
}

// === Update global values ===
void updateMeasurements() {
  Vin   = readSolarVoltage(solarVoltagePin);
  Iin   = readSolarCurrent(solarCurrentPin);
  Vout  = readBatteryVoltage(batteryVoltagePin);
  Iout  = readBatteryCurrent(batteryCurrentPin);
}
