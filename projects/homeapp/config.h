/**
 * @file      config.h
 * @brief     Configuration settings for the Home App
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <WiFi.h>
#include <time.h>

// WiFi credentials - replace with your actual WiFi credentials
extern const char* ssid;
extern const char* password;

// Spotify API host - replace with your actual server address on the local network
extern const char* spotify_server_host;

// Variables to track connection status
extern unsigned long previousMillis;
extern const long connectionCheckInterval;

// Display and UI elements
extern LilyGo_Class amoled;
extern lv_obj_t *status_label;
extern lv_obj_t *ip_label;

// Clock elements
extern lv_obj_t *date_label;
extern lv_obj_t *time_label;
extern lv_obj_t *weekday_label;
extern unsigned long lastClockUpdate;
extern const long clockUpdateInterval;
extern struct tm timeinfo;

// Spotify elements
extern lv_obj_t *song_title_label;
extern lv_obj_t *artist_label;
extern lv_obj_t *play_btn;
extern lv_obj_t *prev_btn;
extern lv_obj_t *next_btn;
extern bool isPlaying;


// Discord UI elements
extern lv_obj_t *discordPanel;
extern lv_obj_t *btnMute;
extern lv_obj_t *btnDeafen;
extern lv_obj_t *btnDisconnect;
extern lv_obj_t *lblDiscordStatus;

// Discord state
extern bool isMuted;
extern bool isDeafened;
extern bool isConnectedDiscord;


// WiFi functions defined in other files
void initWiFi();
void connectToWiFi();
void checkWiFiStatus();

#endif // CONFIG_H