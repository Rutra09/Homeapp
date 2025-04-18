/**
 * @file      config.cpp
 * @brief     Configuration implementation
 */

#include "config.h"

// WiFi credentials - replace with your actual WiFi credentials
const char* ssid = "aqupark";
const char* password = "ProsteHaslo12";

// Spotify API server - replace with your actual server address
const char* spotify_server_host = "http://192.168.0.31:3000";  // Use your server's actual IP address

// Variables to track connection status
unsigned long previousMillis = 0;
const long connectionCheckInterval = 30000; // Check every 30 seconds

// Display and UI elements
LilyGo_Class amoled;
lv_obj_t *status_label;
lv_obj_t *ip_label;

// Clock elements
lv_obj_t *date_label;
lv_obj_t *time_label;
lv_obj_t *weekday_label;
unsigned long lastClockUpdate = 0;
const long clockUpdateInterval = 1000; // Update every 1 second
struct tm timeinfo;

// Spotify elements
lv_obj_t *song_title_label;
lv_obj_t *artist_label;
lv_obj_t *play_btn;
lv_obj_t *prev_btn;
lv_obj_t *next_btn;
bool isPlaying = false;


// Discord state
lv_obj_t *discordPanel;
lv_obj_t *btnMute;
lv_obj_t *btnDeafen;
lv_obj_t *btnDisconnect;
lv_obj_t *lblDiscordStatus;

bool isMuted = false;
bool isDeafened = false;
bool isConnectedDiscord = false;