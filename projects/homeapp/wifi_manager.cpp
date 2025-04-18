/**
 * @file      wifi_manager.cpp
 * @brief     WiFi connection management implementation
 */

#include "wifi_manager.h"

// Dark theme colors
#define ACCENT_GREEN        0x1DB954 // Spotify green
#define ACCENT_RED          0xE53935 // Red for errors
#define ACCENT_ORANGE       0xFF9800 // Orange for warnings

void initWiFi() {
  Serial.println("Initializing WiFi...");
  WiFi.mode(WIFI_STA);
}

void connectToWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  
  // Update display to indicate connection attempt
  if (status_label != NULL) {
    lv_label_set_text(status_label, "WiFi: CONNECTING...");
    // No background color changes in dark theme, just text
  }
  
  // Start connection
  WiFi.begin(ssid, password);
  
  // Initial connection attempt with timeout
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print(".");
    lv_task_handler(); // Keep the UI responsive during connection
  }
  
  // Check if connected
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Update display with connected info
    if (status_label != NULL) {
      lv_label_set_text(status_label, "WiFi: CONNECTED");
      lv_obj_set_style_text_color(status_label, lv_color_hex(ACCENT_GREEN), 0);
    }
    
    // Update IP display
    if (ip_label != NULL) {
      char ipStr[20];
      snprintf(ipStr, sizeof(ipStr), "IP: %s", WiFi.localIP().toString().c_str());
      lv_label_set_text(ip_label, ipStr);
    }
  } else {
    Serial.println("\nWiFi connection failed!");
    
    // Update display with connection failure
    if (status_label != NULL) {
      lv_label_set_text(status_label, "WiFi: CONNECTION FAILED");
      lv_obj_set_style_text_color(status_label, lv_color_hex(ACCENT_RED), 0);
    }
  }
}

void checkWiFiStatus() {
  unsigned long currentMillis = millis();
  
  // Check connection status every connectionCheckInterval ms
  if (currentMillis - previousMillis >= connectionCheckInterval) {
    previousMillis = currentMillis;
    
    if (WiFi.status() != WL_CONNECTED) {
      // If we've lost connection, update the display
      if (status_label != NULL && strcmp(lv_label_get_text(status_label), "WiFi: CONNECTED") == 0) {
        Serial.println("WiFi connection lost! Attempting to reconnect...");
        lv_label_set_text(status_label, "WiFi: RECONNECTING...");
        lv_obj_set_style_text_color(status_label, lv_color_hex(ACCENT_ORANGE), 0);
      }
      
      // Attempt to reconnect
      WiFi.disconnect();
      WiFi.reconnect();
      
      // Wait a bit for reconnection
      delay(500);
      
      // Check if reconnected
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi reconnected!");
        
        // Only update UI if elements exist
        if (status_label != NULL) {
          lv_label_set_text(status_label, "WiFi: CONNECTED");
          lv_obj_set_style_text_color(status_label, lv_color_hex(ACCENT_GREEN), 0);
        }
        
        if (ip_label != NULL) {
          // Update IP display
          char ipStr[20];
          snprintf(ipStr, sizeof(ipStr), "IP: %s", WiFi.localIP().toString().c_str());
          lv_label_set_text(ip_label, ipStr);
        }
      }
    }
  }
}