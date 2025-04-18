/**
 * @file      config.cpp
 * @brief     Configuration settings implementation
 */

#include "config.h"

// WiFi credentials
const char* ssid = "aqupark";
const char* password = "ProsteHaslo12";

// Variables to track connection status
unsigned long previousMillis = 0;
const long connectionCheckInterval = 10000;  // Check WiFi status every 10 seconds

// Display and UI elements
LilyGo_Class amoled;
lv_obj_t *status_label = NULL;
lv_obj_t *ip_label = NULL;
lv_obj_t *rssi_label = NULL;

// API display elements
lv_obj_t *api_tab = NULL;
lv_obj_t *api_status_label = NULL;
lv_obj_t *api_error_label = NULL;
lv_obj_t *product_name_label = NULL;
lv_obj_t *product_details_label = NULL;