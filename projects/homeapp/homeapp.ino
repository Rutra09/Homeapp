/**
 * @file      homeapp.ino
 * @license   MIT
 * @copyright Copyright (c) 2024
 * @date      2024-04-16
 * @brief     Home app showing date, time, and Spotify controls for LilyGo AMOLED Series
 */

#include "config.h"
#include "display.h"
#include "clock.h"
#include "spotify.h"
#include "discord.h"
#include "wifi_manager.h"



// Variables for Spotify update timing
unsigned long lastSpotifyUpdate = 0;
const long spotifyUpdateInterval = 5000; // Update every 5 seconds

// Variables for Discord update timing
unsigned long lastDiscordUpdate = 0;
const long discordUpdateInterval = 10000; // Update every 10 seconds

void setup() {
  Serial.begin(115200);
  delay(3000); // Wait for serial monitor to open
  Serial.println("LilyGo-AMOLED-Series Home App");

  // Initialize display first
  if (!initDisplay()) {
    Serial.println("Display initialization failed!");
    while (1) {
      delay(1000);
    }
  }
  
  // Setup UI before initializing network connections
  setupUI();
  
  // Now initialize WiFi (after UI is ready)
  initWiFi();
  
  // Connect to WiFi (UI elements now exist)
  connectToWiFi();
  
  // Initialize clock
  initClock();
  
  // Initialize Spotify integration with API server
  initSpotify();

  // initDiscord(); // Initialize Discord integration
}

void loop() {
  // Update clock display
  updateClock();
  
  // Check WiFi connection status periodically
  checkWiFiStatus();
  
  // Update Spotify information periodically
  updateSpotifyStatus();
  
  // Update Discord status periodically
  // updateDiscordState();
  
  // Handle LVGL tasks
  lv_task_handler();
  delay(5);
}

void updateSpotifyStatus() {
  // Only update every spotifyUpdateInterval ms
  unsigned long currentMillis = millis();
  if (currentMillis - lastSpotifyUpdate >= spotifyUpdateInterval) {
    lastSpotifyUpdate = currentMillis;
    
    // Only check Spotify if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
      updateNowPlaying();
    }
  }
}

void updateDiscordState() {
  // Only update every discordUpdateInterval ms
  unsigned long currentMillis = millis();
  if (currentMillis - lastDiscordUpdate >= discordUpdateInterval) {
    lastDiscordUpdate = currentMillis;
    
    // Only check Discord if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
      // updateDiscordStatus();
    }
  }
}