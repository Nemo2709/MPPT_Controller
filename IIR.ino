// === IIR.ino ===
// Noise-tolerant IIR filtering

// --- Filter Constants ---
const float alphaVoltage = 0.05;
const float alphaCurrent = 0.05;
const float deadbandCurrent = 0.02;   // Ignore noise wiggles <±20 mA
Search


// --- External raw sensor inputs ---
extern float Iin;   // Raw solar current
extern float Vin;   // Raw solar voltage
extern float Iout;  // Raw battery current
extern float Vout;  // Raw battery voltage

// --- Filtered outputs ---
float filteredCurrent = 0.0;      // Filtered solar current
float filteredVoltage = 0.0;      // Filtered solar voltage
float filteredBatteryCurrent = 0.0;
float filteredBatteryVoltage = 0.0;

// === Apply IIR filters ===
void applyIIRFilter() {
  // Solar voltage — regular IIR
  filteredVoltage = (1 - alphaVoltage) * filteredVoltage + alphaVoltage * Vin;

  // Solar current — deadband IIR
  if (abs(Iin - filteredCurrent) > deadbandCurrent) {
    filteredCurrent = (1 - alphaCurrent) * filteredCurrent + alphaCurrent * Iin;
  }

  // Battery voltage — regular IIR
  filteredBatteryVoltage = (1 - alphaVoltage) * filteredBatteryVoltage + alphaVoltage * Vout;

  // Battery current — deadband IIR
  if (abs(Iout - filteredBatteryCurrent) > deadbandCurrent) {
    filteredBatteryCurrent = (1 - alphaCurrent) * filteredBatteryCurrent + alphaCurrent * Iout;
  }
}
