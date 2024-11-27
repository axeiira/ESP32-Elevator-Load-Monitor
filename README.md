# ESP32-Elevator-Load-Monitor

An IoT-based system for monitoring elevator weight using a load cell and HX711 module, powered by ESP32. This system provides real-time data over both MQTT and HTTP protocols and integrates seamlessly with Node-RED for visualization and predictive maintenance.

---

## Features
- **Load Cell with HX711:** Measures the elevator's load with high precision.
- **ESP32 Microcontroller:** Ensures reliable communication and data processing.
- **Communication Protocols:** Supports both:
  - **MQTT:** For lightweight, real-time messaging.
  - **HTTP:** For direct API-based communication.
- **Node-RED Integration:** Provides a user-friendly interface for monitoring and analyzing load data.

---

## Usage
1. **Setup Hardware:**
   - Connect the load cell to the HX711 module.
   - Connect the HX711 module to the ESP32.

2. **Configure Communication:**
   - Set up your MQTT broker details or HTTP endpoint in the firmware.

3. **Deploy Firmware:**
   - Flash the ESP32 with the provided code.

4. **Integrate with Node-RED:**
   - Use MQTT or HTTP nodes in Node-RED to receive and visualize elevator load data.

5. **Monitor & Maintain:**
   - Use the data for real-time load monitoring and predictive maintenance insights.

---

## Dependencies
- **Hardware:**
  - ESP32
  - Load Cell
  - HX711 Module
- **Software:**
  - Arduino IDE or PlatformIO for firmware
  - Node-RED for data visualization
- **Libraries:**
  - `HX711`
  - `PubSubClient` (for MQTT)
  - `ESPAsyncWebServer` or `HTTPClient` (for HTTP)

---

## Getting Started
1. Clone this repository:
   ```bash
   git clone https://github.com/your-username/ESP32-Elevator-Load-Monitor.git
