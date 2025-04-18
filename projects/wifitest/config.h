/**
 * @file      config.h
 * @brief     Configuration settings for the WiFi example
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <WiFi.h>

// WiFi credentials
extern const char* ssid;
extern const char* password;

// Variables to track connection status
extern unsigned long previousMillis;
extern const long connectionCheckInterval;

// Display and UI elements
extern LilyGo_Class amoled;
extern lv_obj_t *status_label;
extern lv_obj_t *ip_label;
extern lv_obj_t *rssi_label;

// API display elements
extern lv_obj_t *api_tab;
extern lv_obj_t *api_status_label;
extern lv_obj_t *api_error_label;
extern lv_obj_t *product_name_label;
extern lv_obj_t *product_details_label;

#endif // CONFIG_H