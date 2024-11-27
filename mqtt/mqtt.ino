#include "HX711.h"
#include <WiFi.h>
#include <PubSubClient.h>

// Pin Definitions
#define DOUT 22 // Pin DOUT HX711
#define CLK 23  // Pin SCK HX711

// MQTT Configuration
const char* mqtt_server = "192.168.18.250";  
const int mqtt_port = 1883;             
const char* mqtt_topic = "test/topic";  
const char* mqtt_client_id = "ESP32_LoadCell";

// WiFi Configuration
const char* ssid = "testering";
const char* password = "TINI0656";

// HX711 Configuration
HX711 scale;
const int numReadings = 10;
long zeroValue = 0;
float calibrationFactor = -430;

// Timing Configuration
unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Function to connect to WiFi
void setupWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect/reconnect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    if (client.connect(mqtt_client_id)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Function to take multiple readings and average them
long readAverageValue() {
  long sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += scale.read();
    delay(10);
  }
  return sum / numReadings;
}

// Function to publish weight to MQTT broker
void publishWeight(float weight) {
  char weightStr[10];
  dtostrf(weight, 1, 2, weightStr);  // Convert float to string
  
  String jsonPayload = "{\"weight\": " + String(weightStr) + "}";
  
  if (client.publish(mqtt_topic, jsonPayload.c_str())) {
    Serial.println("Weight published to MQTT");
  } else {
    Serial.println("Failed to publish weight");
  }
}

void setup() {
  Serial.begin(9600);
  
  // Initialize HX711
  scale.begin(DOUT, CLK);
  Serial.println("Load Cell Test");
  
  // Setup WiFi
  setupWiFi();
  
  // Configure MQTT
  client.setServer(mqtt_server, mqtt_port);
  
  // Tare the scale
  zeroValue = readAverageValue();
  Serial.print("Tare Value: ");
  Serial.println(zeroValue);
}

void loop() {
  // Ensure MQTT connection
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (scale.is_ready()) {
      long currentReading = readAverageValue();
      long adjustedReading = currentReading - zeroValue;
      float weightInGrams = (float)adjustedReading / calibrationFactor;
      
      Serial.print("Weight (grams): ");
      Serial.println(weightInGrams, 2);
      
      // Publish weight to MQTT broker
      publishWeight(weightInGrams);
    } else {
      Serial.println("HX711 not found. Check connections.");
    }
  }
}