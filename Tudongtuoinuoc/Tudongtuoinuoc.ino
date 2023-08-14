#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "NpOeY7nYXEAmFoqAPMVjBUkOgW8OebYV";
char ssid[] = "SieuAiQuoc1";
char pass[] = "Yeuhoangthaiha1";
#define DHTPIN D1
#define DHTTYPE DHT11
#define RELAY_PIN D2
float humidityValue;
float temperatureValue;
int targetMoisture = 50; // Default value for target moisture
bool pumpStatus = false;
int lastButtonState = HIGH;
DHT dht(DHTPIN, DHTTYPE);
BLYNK_WRITE(V0) {
  targetMoisture = param.asInt();
}
BLYNK_WRITE(V1) {
  int value = param.asInt();
  if (value == 1 && !pumpStatus) {
    pumpStatus = true;
    digitalWrite(RELAY_PIN, HIGH); // Turn on the pump
  } else if (value == 0 && pumpStatus) {
    pumpStatus = false;
    digitalWrite(RELAY_PIN, LOW); // Turn off the pump
  }
  Blynk.virtualWrite(V2, pumpStatus); // Update the LED widget with the pump status
}
BLYNK_WRITE(V7) {
  targetMoisture = param.asInt(); // Read the target moisture value from the Slider
}
void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  dht.begin();
}
void loop() {
  Blynk.run();

  humidityValue = dht.readHumidity();
  temperatureValue = dht.readTemperature();

  if (isnan(humidityValue) || isnan(temperatureValue)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V5, humidityValue);
  Blynk.virtualWrite(V6, temperatureValue);
  int soilMoisture = analogRead(A0);
  int moisturePercentage = map(soilMoisture, 0, 1023, 0, 100);
  Blynk.virtualWrite(V4, moisturePercentage); // Update the Gauge widget with soil moisture value
  if (moisturePercentage < targetMoisture && pumpStatus) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Pump ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Pump OFF");
  }
  delay(1000);
}
