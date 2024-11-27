#include "HX711.h"
#include <WiFi.h>
#include <HTTPClient.h>

// 192.168.18.250

#define DOUT 22  // Pin DOUT HX711
#define CLK 23   // Pin SCK HX711

HX711 scale;

const int numReadings = 10;      // Number of readings to average
long zeroValue = 0;              // Variable to store tare value (no-weight reading)
float calibrationFactor = -430;  // Calibration factor (to convert to grams, adjust based on your known weight)

const char* ssid = "manuw";
const char* password = "rahasiaini";
const char* serverName = "http://192.168.43.246:1880/home";

unsigned long previousMillis = 0;  // Stores last time the weight was sent
const long interval = 10000;       // Interval at which to send weight (10 seconds)

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT, CLK); 

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Serial.println("Load Cell Test");
  // Tare the scale at the start to get the zero value
  zeroValue = readAverageValue();
  Serial.print("Tare Value: ");
  Serial.println(zeroValue);
}

// Function to take multiple readings and average them
long readAverageValue() {
  long sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += scale.read();
    delay(10);  // Small delay between readings to reduce noise
  }
  return sum / numReadings;
}

void sendWeightToNodeRED(float weight) {
  if (WiFi.status() == WL_CONNECTED) {  // Check if the ESP32 is connected to Wi-Fi
    HTTPClient http;
    
    http.begin(serverName);  // Specify the URL of the Node-RED HTTP endpoint
    http.addHeader("Content-Type", "application/json");  // Specify the content-type as JSON
    
    // Create a JSON payload to send the weight data
    String jsonPayload = "{\"weight\": " + String(weight, 2) + "}";
    
    int httpResponseCode = http.POST(jsonPayload);  // Send the POST request
    
    if (httpResponseCode > 0) {
      String response = http.getString();  // Get the response from the server
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error on HTTP request");
    }
    
    http.end();  // Close the connection
  } else {
    Serial.println("WiFi not connected");
  }
}

void loop() {
  unsigned long currentMillis = millis();  // Get the current time

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Save the last time the weight was sent
    
    if (scale.is_ready()) {
      long currentReading = readAverageValue();  // Get the averaged reading
      long RawValueWeight = scale.read();
      
      long adjustedReading = currentReading - zeroValue;  // Adjust for tare/zero value
      
      // Convert the adjusted reading to grams using the calibration factor
      float weightInGrams = (float)adjustedReading / calibrationFactor;
      Serial.print("Weight (grams): ");
      Serial.println(weightInGrams, 2);  // Display weight with 2 decimal places
      sendWeightToNodeRED(weightInGrams);  // Send weight to Node-RED every 10 seconds
    } else {
      Serial.println("HX711 not found. Check connections.");
    }
  }
}
