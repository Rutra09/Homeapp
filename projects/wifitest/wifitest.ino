/**
 * @file      wifitest.ino
 * @license   MIT
 * @copyright Copyright (c) 2024
 * @date      2024-04-16
 * @brief     WiFi connection example for LilyGo AMOLED Series with status display
 */

#include "config.h"
#include "display.h"
#include "wifi_manager.h"
#include "http_client.h"

void setup() {
  Serial.begin(115200);
  delay(3000); // Wait for serial monitor to open
  Serial.println("LilyGo-AMOLED-Series WiFi Connection Example");

  // Initialize display
  if (!initDisplay()) {
    Serial.println("Display initialization failed!");
    while (1) {
      delay(1000);
    }
  }

  // Create UI elements
  setupUI();
  
  // Initialize WiFi
  initWiFi();
  
  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  // Check WiFi connection status periodically
  checkWiFiStatus();
  
  // Check if it's time to make an API request
  checkApiRequest();
  
  // Handle LVGL tasks
  lv_task_handler();
  delay(5);
}