/**
 * @file      display.cpp
 * @brief     Display and UI functions implementation
 */

#include "display.h"
#include "spotify.h"
#include "discord.h"

// Global variables
lv_obj_t *info_container = NULL;
lv_obj_t *cover_img = NULL;
bool showing_info = false;

// Dark theme colors
#define DARK_BG_COLOR       0x181818 // Dark background
#define DARK_ACCENT_COLOR   0x303030 // Slightly lighter for containers
#define TEXT_COLOR          0xFFFFFF // White text
#define TEXT_SECONDARY      0xAAAAAA // Gray text for secondary info
#define ACCENT_GREEN        0x1DB954 // Spotify green
#define ACCENT_RED          0xE53935 // Red for errors
#define ACCENT_ORANGE       0xFF9800 // Orange for warnings

bool initDisplay() {
  // Automatically determine the access device
  bool result = amoled.begin();
  
  if (result) {
    // Initialize LVGL helper
    beginLvglHelper(amoled);
    
    // Set display brightness to medium level (adjust as needed)
    amoled.setBrightness(128); // 0-255
    
    // Set screen background to dark
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(DARK_BG_COLOR), 0);
  }
  
  return result;
}

void setupUI() {
  // Create a main screen without tabs
  lv_obj_t *main_screen = lv_scr_act();
  
  // === Time and Date (Left Side) ===
  // Create time label with large font
  time_label = lv_label_create(main_screen);
  lv_label_set_text(time_label, "00:00");
  lv_obj_align(time_label, LV_ALIGN_LEFT_MID, 20, -20);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(time_label, lv_color_hex(TEXT_COLOR), 0);
  
  // Create date label below time
  date_label = lv_label_create(main_screen);
  lv_label_set_text(date_label, "00.00.00");
  lv_obj_align(date_label, LV_ALIGN_LEFT_MID, 20, 30);
  lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(date_label, lv_color_hex(TEXT_SECONDARY), 0);
  
  // === Spotify Controls (Right Side) - Now wider ===
  // Use full right half of screen
  int spotify_width = 200;
  int spotify_right_margin = 10;
  
  // Album Cover Placeholder (square at top)
  cover_img = lv_obj_create(main_screen);
  lv_obj_set_size(cover_img, 80, 80);
  lv_obj_align(cover_img, LV_ALIGN_TOP_RIGHT, -spotify_right_margin - (spotify_width - 80) / 2, 20);
  lv_obj_set_style_bg_color(cover_img, lv_color_hex(DARK_ACCENT_COLOR), 0);
  lv_obj_set_style_border_width(cover_img, 1, 0);
  lv_obj_set_style_border_color(cover_img, lv_color_hex(ACCENT_GREEN), 0);
  lv_obj_set_style_radius(cover_img, 5, 0);
  
  // Create song title label
  song_title_label = lv_label_create(main_screen);
  lv_label_set_text(song_title_label, "Not playing");
  lv_obj_set_width(song_title_label, spotify_width);
  lv_label_set_long_mode(song_title_label, LV_LABEL_LONG_WRAP);
  lv_obj_align(song_title_label, LV_ALIGN_TOP_RIGHT, -spotify_right_margin, 110);
  lv_obj_set_style_text_font(song_title_label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(song_title_label, lv_color_hex(TEXT_COLOR), 0);
  lv_obj_set_style_text_align(song_title_label, LV_TEXT_ALIGN_CENTER, 0);
  
  // Create artist label
  artist_label = lv_label_create(main_screen);
  lv_label_set_text(artist_label, "");
  lv_obj_set_width(artist_label, spotify_width);
  lv_label_set_long_mode(artist_label, LV_LABEL_LONG_WRAP);
  lv_obj_align(artist_label, LV_ALIGN_TOP_RIGHT, -spotify_right_margin, 140);
  lv_obj_set_style_text_font(artist_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(artist_label, lv_color_hex(TEXT_SECONDARY), 0);
  lv_obj_set_style_text_align(artist_label, LV_TEXT_ALIGN_CENTER, 0);
  
  // Create button container
  lv_obj_t *btn_container = lv_obj_create(main_screen);
  lv_obj_remove_style_all(btn_container);
  lv_obj_set_size(btn_container, spotify_width, 60);
  lv_obj_align(btn_container, LV_ALIGN_BOTTOM_RIGHT, -spotify_right_margin, -20);
  lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  
  // Previous button - dark themed
  prev_btn = lv_btn_create(btn_container);
  lv_obj_set_size(prev_btn, 45, 45);
  lv_obj_add_event_cb(prev_btn, spotify_prev_callback, LV_EVENT_CLICKED, NULL);
  lv_obj_set_style_bg_color(prev_btn, lv_color_hex(DARK_ACCENT_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(prev_btn, lv_color_hex(ACCENT_GREEN), LV_STATE_PRESSED);
  lv_obj_set_style_shadow_width(prev_btn, 0, 0);
  lv_obj_set_style_radius(prev_btn, 22, 0); // Make it round
  
  lv_obj_t *prev_label = lv_label_create(prev_btn);
  lv_label_set_text(prev_label, LV_SYMBOL_PREV);
  lv_obj_center(prev_label);
  lv_obj_set_style_text_color(prev_label, lv_color_hex(ACCENT_GREEN), 0);
  
  // Play/Pause button - dark themed
  play_btn = lv_btn_create(btn_container);
  lv_obj_set_size(play_btn, 45, 45);
  lv_obj_add_event_cb(play_btn, spotify_play_callback, LV_EVENT_CLICKED, NULL);
  lv_obj_set_style_bg_color(play_btn, lv_color_hex(DARK_ACCENT_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(play_btn, lv_color_hex(ACCENT_GREEN), LV_STATE_PRESSED);
  lv_obj_set_style_shadow_width(play_btn, 0, 0);
  lv_obj_set_style_radius(play_btn, 22, 0); // Make it round
  
  lv_obj_t *play_label = lv_label_create(play_btn);
  lv_label_set_text(play_label, LV_SYMBOL_PLAY);
  lv_obj_center(play_label);
  lv_obj_set_style_text_color(play_label, lv_color_hex(ACCENT_GREEN), 0);
  
  // Next button - dark themed
  next_btn = lv_btn_create(btn_container);
  lv_obj_set_size(next_btn, 45, 45);
  lv_obj_add_event_cb(next_btn, spotify_next_callback, LV_EVENT_CLICKED, NULL);
  lv_obj_set_style_bg_color(next_btn, lv_color_hex(DARK_ACCENT_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(next_btn, lv_color_hex(ACCENT_GREEN), LV_STATE_PRESSED);
  lv_obj_set_style_shadow_width(next_btn, 0, 0);
  lv_obj_set_style_radius(next_btn, 22, 0); // Make it round
  
  lv_obj_t *next_label = lv_label_create(next_btn);
  lv_label_set_text(next_label, LV_SYMBOL_NEXT);
  lv_obj_center(next_label);
  lv_obj_set_style_text_color(next_label, lv_color_hex(ACCENT_GREEN), 0);
  
  // === Discord Controls (Center) ===
  // Create a panel for Discord controls - centered without border
  discordPanel = lv_obj_create(main_screen);
  lv_obj_set_size(discordPanel, 80, 240); // Full height and narrow width
  lv_obj_align(discordPanel, LV_ALIGN_CENTER, 0, 0);
  
  // Make it transparent (no visible border or background)
  lv_obj_set_style_bg_opa(discordPanel, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_opa(discordPanel, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_shadow_opa(discordPanel, LV_OPA_TRANSP, LV_PART_MAIN);
  
  // Status label at the top
  lblDiscordStatus = lv_label_create(discordPanel);
  lv_label_set_text(lblDiscordStatus, "Discord");
  lv_obj_align(lblDiscordStatus, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_text_color(lblDiscordStatus, lv_color_hex(TEXT_COLOR), LV_PART_MAIN);
  
  // Create vertical stack of Discord control buttons
  
  // Mute button with "M" letter (at the top)
  btnMute = lv_btn_create(discordPanel);
  lv_obj_set_size(btnMute, 60, 60);
  lv_obj_align_to(btnMute, lblDiscordStatus, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
  lv_obj_add_event_cb(btnMute, discordMuteEvent, LV_EVENT_CLICKED, NULL);
  lv_obj_set_style_bg_color(btnMute, lv_color_hex(0x00FF00), LV_PART_MAIN); // Green when unmuted
  
  // Add letter "M" to mute button
  lv_obj_t *lblMute = lv_label_create(btnMute);
  lv_label_set_text(lblMute, "M");
  lv_obj_set_style_text_font(lblMute, &lv_font_montserrat_24, LV_PART_MAIN);
  lv_obj_center(lblMute);
  
  // Deafen button with "H" letter (in the middle)
  btnDeafen = lv_btn_create(discordPanel);
  lv_obj_set_size(btnDeafen, 60, 60);
  lv_obj_align_to(btnDeafen, btnMute, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
  lv_obj_add_event_cb(btnDeafen, discordDeafenEvent, LV_EVENT_CLICKED, NULL);
  lv_obj_set_style_bg_color(btnDeafen, lv_color_hex(0x5865F2), LV_PART_MAIN); // Discord blue color
  
  // Add letter "H" to deafen button
  lv_obj_t *lblDeafen = lv_label_create(btnDeafen);
  lv_label_set_text(lblDeafen, "H");
  lv_obj_set_style_text_font(lblDeafen, &lv_font_montserrat_24, LV_PART_MAIN);
  lv_obj_center(lblDeafen);
  
  // Disconnect button with "D" letter (at the bottom)
  btnDisconnect = lv_btn_create(discordPanel);
  lv_obj_set_size(btnDisconnect, 70, 70); // Larger size for better visibility
  lv_obj_align(btnDisconnect, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_add_event_cb(btnDisconnect, discordDisconnectEvent, LV_EVENT_CLICKED, NULL);
  lv_obj_set_style_bg_color(btnDisconnect, lv_color_hex(0xED4245), LV_PART_MAIN); // Discord red color
  lv_obj_set_style_shadow_width(btnDisconnect, 10, LV_PART_MAIN); // Add shadow for visibility
  lv_obj_set_style_shadow_opa(btnDisconnect, LV_OPA_50, LV_PART_MAIN);
  lv_obj_set_style_shadow_color(btnDisconnect, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_border_width(btnDisconnect, 2, LV_PART_MAIN); // Add border
  lv_obj_set_style_border_color(btnDisconnect, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // White border
  
  // Add letter "D" to disconnect button with larger font
  lv_obj_t *lblDisconnect = lv_label_create(btnDisconnect);
  lv_label_set_text(lblDisconnect, "D");
  lv_obj_set_style_text_font(lblDisconnect, &lv_font_montserrat_28, LV_PART_MAIN); // Larger font
  lv_obj_center(lblDisconnect);
  
  // Initially hide disconnect button if not connected
  lv_obj_add_flag(btnDisconnect, LV_OBJ_FLAG_HIDDEN);
  
  // === Hidden Info Panel (shows on home button press) ===
  info_container = lv_obj_create(main_screen);
  lv_obj_set_size(info_container, lv_pct(90), lv_pct(40));
  lv_obj_align(info_container, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_set_style_radius(info_container, 10, 0);
  lv_obj_set_style_bg_color(info_container, lv_color_hex(DARK_ACCENT_COLOR), 0);
  lv_obj_set_style_bg_opa(info_container, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(info_container, 1, 0);
  lv_obj_set_style_border_color(info_container, lv_color_hex(ACCENT_GREEN), 0);
  lv_obj_set_style_shadow_width(info_container, 15, 0);
  lv_obj_set_style_shadow_color(info_container, lv_color_hex(0x000000), 0);
  lv_obj_set_style_pad_all(info_container, 10, 0);
  
  // Create WiFi status label
  status_label = lv_label_create(info_container);
  lv_label_set_text(status_label, "WiFi: NOT CONNECTED");
  lv_obj_align(status_label, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(status_label, lv_color_hex(TEXT_COLOR), 0);
  
  // Create IP address label
  ip_label = lv_label_create(info_container);
  lv_label_set_text(ip_label, "IP: ---.---.---.---");
  lv_obj_align(ip_label, LV_ALIGN_TOP_LEFT, 0, 30);
  lv_obj_set_style_text_font(ip_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(ip_label, lv_color_hex(TEXT_SECONDARY), 0);
  
  // Hide the info panel by default
  lv_obj_add_flag(info_container, LV_OBJ_FLAG_HIDDEN);
  
  // Register home button callback
  amoled.setHomeButtonCallback([](void *ptr) {
    Serial.println("Home key pressed!");
    static uint32_t checkMs = 0;
    if (millis() > checkMs) {
      showing_info = !showing_info;
      if (showing_info) {
        lv_obj_clear_flag(info_container, LV_OBJ_FLAG_HIDDEN);
      } else {
        lv_obj_add_flag(info_container, LV_OBJ_FLAG_HIDDEN);
      }
    }
    checkMs = millis() + 200;
  }, NULL);
}