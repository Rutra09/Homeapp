/**
 * @file      wifi_manager.cpp
 * @brief     WiFi connection management functions implementation
 */

#include "wifi_manager.h"
#include "http_client.h"

void initWiFi() {
  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  
  lv_obj_set_style_bg_color(status_label, lv_color_hex(0xAAAA00), 0); // Yellow background
  lv_label_set_text(status_label, "CONNECTING...");
  lv_label_set_text(ip_label, "IP: ---.---.---.---");
  lv_label_set_text(rssi_label, "Signal: -- dBm");
  
  WiFi.begin(ssid, password);
  
  // Wait for connection for up to 20 seconds
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Blink the status label while connecting
    if (attempts % 2 == 0) {
      lv_obj_set_style_bg_color(status_label, lv_color_hex(0xAAAA00), 0); // Yellow
    } else {
      lv_obj_set_style_bg_color(status_label, lv_color_hex(0x555500), 0); // Dark yellow
    }
    lv_task_handler(); // Update the display
  }
  
  updateWiFiStatus();
  
  // If connected, make the initial API request
  if (WiFi.status() == WL_CONNECTED) {
    makeApiRequest();
  }
}

void updateWiFiStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    lv_obj_set_style_bg_color(status_label, lv_color_hex(0x00AA00), 0); // Green background
    lv_label_set_text(status_label, "CONNECTED");
    lv_label_set_text_fmt(ip_label, "IP: %s", WiFi.localIP().toString().c_str());
    lv_label_set_text_fmt(rssi_label, "Signal: %d dBm", WiFi.RSSI());
  } else {
    lv_obj_set_style_bg_color(status_label, lv_color_hex(0xAA0000), 0); // Red background
    lv_label_set_text(status_label, "NOT CONNECTED");
    lv_label_set_text(ip_label, "IP: ---.---.---.---");
    lv_label_set_text(rssi_label, "Signal: -- dBm");
  }
}

void checkWiFiStatus() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= connectionCheckInterval) {
    previousMillis = currentMillis;
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi connection lost! Attempting to reconnect...");
      connectToWiFi();
    } else {
      // Just update the WiFi status (signal strength might change)
      updateWiFiStatus();
    }
  }
}