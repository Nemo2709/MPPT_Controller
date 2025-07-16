// === GlobalScan.ino ===
// Performs a staged global power sweep every 30 seconds

extern float Power_now;
extern float filteredVoltage;
extern float filteredCurrent;
extern const int pwmChannel;
extern const int vinPin;
extern const int iinPin;

const unsigned long sweepInterval = 30000;  // ms
const int sweepSteps[] = {150, 165, 180, 195, 210, 225, 240, 255, 270, 285, 300};
const int numSweepSteps = sizeof(sweepSteps) / sizeof(sweepSteps[0]);

unsigned long lastSweepTime = 0;
int sweepIndex = 0;
float maxPowerSeen = 0;
int bestDuty = 150;
bool sweepInProgress = false;
bool finalConfirmationStep = false;

void checkForGlobalScan() {
  unsigned long now = millis();

  // Trigger sweep every 30 seconds
  if (!sweepInProgress && !finalConfirmationStep && (now - lastSweepTime >= sweepInterval)) {
    sweepInProgress = true;
    sweepIndex = 0;
    maxPowerSeen = 0;
    bestDuty = sweepSteps[0];
  }

  // Execute sweep step-by-ste
  if (sweepInProgress && sweepIndex < numSweepSteps) {
    int duty = sweepSteps[sweepIndex];
    ledcWrite(pwmChannel, duty);
    delay(30); // Let current/voltage settle

      float power = filteredVoltage * filteredCurrent;

    if (power > maxPowerSeen) {
      maxPowerSeen = power;
      bestDuty = duty;
    }

    sweepIndex++;

    // Finished last sweep step go to confirmation phase
    if (sweepIndex >= numSweepSteps) {
      sweepInProgress = false;
      finalConfirmationStep = true;
    }
  }

  // Confirm and apply best duty cycle
  if (finalConfirmationStep) {
    ledcWrite(pwmChannel, bestDuty);
    delay(30);

    float power = filteredVoltage * filteredCurrent;
    Power_now = power;

    finalConfirmationStep = false;
    lastSweepTime = millis();  // Now we reset the timer
  }
}
