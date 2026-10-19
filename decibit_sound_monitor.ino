/*
 * Sound Monitor System
 * --------------------
 * This project uses an Arduino to monitor environmental sound levels (dB) 
 * and frequencies. 
 */

#include <LiquidCrystal.h>

// LCD Pin Configuration: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 10);

// Pin Definitions
const int dbPin = A0;           // Sound level sensor (Analog)
const int freqPin1 = A1;        // Frequency component 1 (Analog)
const int freqPin2 = A2;        // Frequency component 2 (Analog)
const int buttonPin = 2;        // Pushbutton to cycle display modes
const int powerSwitchPin = 6;   // Slide switch for system ON/OFF
const int ledPin = 7;           // System status LED

// Global Variables
int screenMode = 0;             // Tracks current display layout (0, 1, or 2)
bool buttonWasPressed = false;  // Helper for button debouncing/state change
unsigned long lastSampleTime = 0; 
const unsigned long sampleInterval = 1000; // Update sensor data every 1000ms

bool lastSystemOn = true;       // Tracks previous power state to handle screen clearing

// Data Storage (initialized with default values)
int dbValue = 30;
int freqValue = 200;
String noiseLevel = "Quiet";
String freqBand = "Low";

void setup() {
  Serial.begin(9600);           // Initialize Serial for debugging/data logging

  // Setup Hardware Pins
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(powerSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Sound Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1200);
  lcd.clear();
}

void loop() {
  // Check if the hardware power switch is ON (HIGH)
  bool systemOn = (digitalRead(powerSwitchPin) == HIGH);

  // --- POWER OFF LOGIC ---
  if (!systemOn) {
    digitalWrite(ledPin, LOW);

    // Clear screen only once when shutting down
    if (lastSystemOn) {
      lcd.clear();
      lastSystemOn = false;
    }

    buttonWasPressed = false;
    delay(50); // Small delay to save processing power
    return;    // Skip the rest of the loop
  }

  // --- POWER ON LOGIC ---
  digitalWrite(ledPin, HIGH);

  // If the system was just switched from OFF to ON
  if (!lastSystemOn) {
    lcd.clear();
    lastSystemOn = true;
    updateDisplay(millis() / 1000, dbValue, freqValue, noiseLevel, freqBand);
  }

  handleButton(); // Check for display mode changes

  unsigned long currentTime = millis();
  unsigned long timeSec = currentTime / 1000;

  // --- DATA SAMPLING ---
  // Samples sensors at intervals defined by 'sampleInterval'
  if (currentTime - lastSampleTime >= sampleInterval) {
    lastSampleTime = currentTime;

    // Read analog values
    int dbRaw = analogRead(dbPin);
    int freqRaw1 = analogRead(freqPin1);
    int freqRaw2 = analogRead(freqPin2);

    // Map raw 10-bit analog data to human-readable units
    dbValue = map(dbRaw, 0, 1023, 30, 100);

    int freqValue1 = map(freqRaw1, 0, 1023, 100, 2000);
    int freqValue2 = map(freqRaw2, 0, 1023, 100, 2000);

    // Combine simulated frequencies and apply safety cap
    freqValue = freqValue1 + freqValue2;
    if (freqValue > 9999) freqValue = 9999;

    // Categorize Sound Level
    if (dbValue < 50)      noiseLevel = "Quiet";
    else if (dbValue < 75) noiseLevel = "Moderate";
    else                   noiseLevel = "Loud";

    // Categorize Frequency Band
    if (freqValue < 500)       freqBand = "Low";
    else if (freqValue < 2000) freqBand = "Mid";
    else                       freqBand = "High";

    // Refresh the LCD with new data
    updateDisplay(timeSec, dbValue, freqValue, noiseLevel, freqBand);

    // Output data in JSON-like format for Serial Plotter/Monitor
    Serial.print("{");
    Serial.print(timeSec);
    Serial.print(", ");
    Serial.print(dbValue);
    Serial.print(", ");
    Serial.print(freqValue);
    Serial.println("}");
  }
}

/**
 * Monitors the pushbutton to cycle through 3 display modes.
 * Uses a basic software debounce and state tracking.
 */
void handleButton() {
  int buttonState = digitalRead(buttonPin);

  // Detect transition from HIGH to LOW (button press)
  if (buttonState == LOW && !buttonWasPressed) {
    screenMode++;
    if (screenMode > 2) screenMode = 0; // Reset to first screen

    buttonWasPressed = true;
    lcd.clear(); // Clear to prevent artifacts from previous screen mode
    updateDisplay(millis() / 1000, dbValue, freqValue, noiseLevel, freqBand);
    delay(200); // Debounce delay
  }

  // Reset flag when button is released
  if (buttonState == HIGH) {
    buttonWasPressed = false;
  }
}

/**
 * Handles all LCD rendering based on the current 'screenMode'.
 * Automatically manages character spacing to avoid display "ghosting."
 */
void updateDisplay(unsigned long timeSec, int dbValue, int freqValue, String noiseLevel, String freqBand) {
  
  if (screenMode == 0) {
    // Mode 0: Time and Decibel Level
    lcd.setCursor(0, 0);
    lcd.print("Time:");
    lcd.print(timeSec);
    lcd.print("s      "); // Overwrite trailing characters

    lcd.setCursor(0, 1);
    lcd.print(dbValue);
    lcd.print(" dB ");
    lcd.print(noiseLevel);

    // Clear remaining line space
    int used = String(dbValue).length() + 4 + noiseLevel.length();
    for (int i = used; i < 16; i++) lcd.print(" ");
  }
  else if (screenMode == 1) {
    // Mode 1: Time and Frequency (Hz)
    lcd.setCursor(0, 0);
    lcd.print("Time:");
    lcd.print(timeSec);
    lcd.print("s      ");

    lcd.setCursor(0, 1);
    lcd.print(freqValue);
    lcd.print("Hz ");
    lcd.print(freqBand);

    int used = String(freqValue).length() + 3 + freqBand.length();
    for (int i = used; i < 16; i++) lcd.print(" ");
  }
  else {
    // Mode 2: Summary View (Compact)
    lcd.setCursor(0, 0);
    lcd.print("t:");
    lcd.print(timeSec);
    lcd.print(" dB:");
    lcd.print(dbValue);

    int topUsed = 2 + String(timeSec).length() + 4 + String(dbValue).length();
    for (int i = topUsed; i < 16; i++) lcd.print(" ");

    lcd.setCursor(0, 1);
    lcd.print("F:");
    lcd.print(freqValue);

    int bottomUsed = 2 + String(freqValue).length();
    for (int i = bottomUsed; i < 16; i++) lcd.print(" ");
  }
}
