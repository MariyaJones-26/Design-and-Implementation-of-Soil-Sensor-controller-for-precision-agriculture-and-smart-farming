#include "DHT.h"

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Pins
const int pumpLED = 2;      // Red LED
const int statusLED = 5;    // Green LED
const int buzzerPin = 4;    // Buzzer
const int buttonPin = 18;   // Mode button

// Moisture simulation
int moisture = 35;

// Threshold
int threshold = 40;

bool manualMode = false;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(pumpLED, OUTPUT);
  pinMode(statusLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {

  // ---- MODE TOGGLE ----
  if (digitalRead(buttonPin) == LOW) {
    manualMode = !manualMode;
    delay(300);  // debounce
  }

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  Serial.println("=================================");
  Serial.print("MODE: ");
  Serial.println(manualMode ? "MANUAL" : "AUTO");

  Serial.print("Moisture: ");
  Serial.println(moisture);

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");

  // -------- MANUAL MODE --------
  if (manualMode) {

    digitalWrite(pumpLED, HIGH);     // Pump ON
    digitalWrite(statusLED, LOW);    // Green OFF
    moisture += 2;                  // Continuous watering
    Serial.println("Manual Irrigation Active");

  }

  // -------- AUTO MODE --------
  else {

    if (moisture < threshold) {

      digitalWrite(pumpLED, HIGH);
      digitalWrite(statusLED, LOW);
      Serial.println("Auto Irrigation ON");

      int previousMoisture = moisture;
      //moisture += 3;   // Water pulse

      delay(500);

      // Dry-run detection
      if (moisture <= previousMoisture) {
        Serial.println("ALERT: Dry Run Detected!");
        tone(buzzerPin, 2000);  // 2kHz sound
        delay(500);
        noTone(buzzerPin);
      }

    } else {

      digitalWrite(pumpLED, LOW);
      digitalWrite(statusLED, HIGH);
      Serial.println("Soil Moisture Adequate");

      moisture -= 1;  // Evaporation
    }
  }

  // Limit moisture range
  if (moisture > 80) moisture = 80;
  if (moisture < 0) moisture = 0;

  Serial.println("=================================");

  delay(800);
}
