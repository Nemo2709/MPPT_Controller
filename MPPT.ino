// === MPPT.ino ===

extern float filteredCurrent;
extern float filteredVoltage;
extern const int pwmChannel;

// Flags from GlobalScan
extern bool sweepInProgress;
extern bool finalConfirmationStep;

float Pin = 0.0;

void runMPPT() {
  // Prevent MPPT from running during global sweep
  if (sweepInProgress || finalConfirmationStep) return;

  static float Voltage_prev = 0.0;
  static float Current_prev = 0.0;
  static float Power_prev = 0.0;
  static int dutyCycle = 128;
  static unsigned long lastPrintTime = 0;
  const unsigned long printInterval = 500;

  const float delta = 1.4;
  const int minDuty = 15;
  const int maxDuty = 450;
  const float startupCurrentThreshold = 0.7;

  float Power_now = filteredVoltage * filteredCurrent;
  float deltaV = filteredVoltage - Voltage_prev;
  float deltaI = filteredCurrent - Current_prev;

  // Continuous startup sweep
  if (filteredCurrent < startupCurrentThreshold && filteredVoltage > 0 && dutyCycle < maxDuty) {
    dutyCycle++;
  } else {
    // Core MPPT logic
    if (deltaV == 0) {
      if (deltaI > 0) {
        dutyCycle -= delta;
      } else if (deltaI < 0) {
        dutyCycle += delta;
      }
    } else {
      float dIdV = (deltaI / deltaV) + (filteredCurrent / filteredVoltage);
      if (abs((filteredVoltage * deltaI) + (filteredCurrent * deltaV)) > 0.2) {
        if (dIdV > 0) {
          dutyCycle -= delta;
        } else {
          dutyCycle += delta;
        }
      }
    }
  }

  // Clamp and apply PWM
  dutyCycle = constrain(dutyCycle, minDuty, maxDuty);
  ledcWrite(pwmChannel, dutyCycle);
  Pin = Power_now;

  // Save for next iteration
  Voltage_prev = filteredVoltage;
  Current_prev = filteredCurrent;
  Power_prev = Power_now;

  // Debug print
  unsigned long now = millis();
  if (now - lastPrintTime >= printInterval) {
    Serial.print("Vin: ");  Serial.print(filteredVoltage, 2);  Serial.print(" V, ");
    Serial.print("Iin: ");  Serial.print(filteredCurrent, 2);  Serial.print(" A, ");
    Serial.print("Pin: ");  Serial.print(Power_now, 2);     Serial.print(" W, ");
    Serial.print("Duty: "); Serial.println(dutyCycle);
    lastPrintTime = now;
  }
}
