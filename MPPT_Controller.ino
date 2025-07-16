// === MPPT_Controller.ino ===
// Main loop for MPPT boost converter
#include <Arduino.h>

// Pin Definitions
const int solarVoltagePin = 27;  // Voltage divider output
const int solarCurrentPin = 32;  // TMCS1133 current sensor output
const int batteryVoltagePin = 28;  // Voltage divider output
const int batteryCurrentPin = 33;  // TMCS1133 current sensor output
const int pwmPin          = 25;  // PWM output to gate driver
const int pwmChannel      = 0;

// Timing Config
const unsigned long loopInterval = 40;  // milliseconds giving a frequency of 25Hz
unsigned long lastLoopTime = 0;


// Function declarations
extern void updateMeasurements();     // From ReadSensors.ino
extern void applyIIRFilter();         // From IIR.ino
extern void checkForGlobalscan();     // From Global_scan.ino
extern void runMPPT();                // From MPPT.ino

// PWM Setup
void setupPWM() {
  const int pwmFreq       = 100000;
  const int pwmResolution = 9; //setting duty cycel resolution to 512 steps

  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(pwmPin, pwmChannel);
}

// Arduino Setup
void setup() {
  Serial.begin(115200);
  setupPWM();
}

// Arduino Loop
void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastLoopTime >= loopInterval) {
    lastLoopTime = currentTime;

    updateMeasurements();      // Read voltage and current (raw)
    applyIIRFilter();          // Filter voltage and current
    runMPPT();                 // Use filtered current and filtered voltaage to adjust PWM
    checkForGlobalscan();      // PSC mitigation code runs every x seconds
  }
}
