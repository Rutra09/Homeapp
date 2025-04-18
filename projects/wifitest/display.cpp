/**
 * @file      display.cpp
 * @brief     Display and UI functions implementation
 */

#include "display.h"

// Global tabview
lv_obj_t *tabview = NULL;

bool initDisplay() {
  // Automatically determine the access device
  bool result = amoled.begin();
  
  if (result) {
    // Initialize LVGL helper
    beginLvglHelper(amoled);
  }
  
  return result;
}

void setupUI() {
  // Create a tabview
  tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
  
  // Add tabs
  lv_obj_t *wifi_tab = lv_tabview_add_tab(tabview, "WiFi");
  api_tab = lv_tabview_add_tab(tabview, "API Data");
  
  // === WiFi Tab ===
  // Create a title label
  lv_obj_t *title_label = lv_label_create(wifi_tab);
  lv_label_set_text(title_label, "WiFi Status");
  lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_font(title_label, &lv_font_montserrat_22, 0);
  
  // Create a status container with a label
  status_label = lv_label_create(wifi_tab);
  lv_obj_set_style_bg_opa(status_label, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(status_label, 5, 0);
  lv_obj_set_style_pad_all(status_label, 10, 0);
  lv_label_set_text(status_label, "NOT CONNECTED");
  lv_obj_align(status_label, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_text_font(status_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_bg_color(status_label, lv_color_hex(0xAA0000), 0); // Red background initially
  
  // Create IP address label
  ip_label = lv_label_create(wifi_tab);
  lv_label_set_text(ip_label, "IP: ---.---.---.---");
  lv_obj_align(ip_label, LV_ALIGN_CENTER, 0, 20);
  lv_obj_set_style_text_font(ip_label, &lv_font_montserrat_16, 0);
  
  // Create RSSI label
  rssi_label = lv_label_create(wifi_tab);
  lv_label_set_text(rssi_label, "Signal: -- dBm");
  lv_obj_align(rssi_label, LV_ALIGN_CENTER, 0, 50);
  lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_16, 0);
  
  // === API Data Tab ===
  // Create API status label
  api_status_label = lv_label_create(api_tab);
  lv_label_set_text(api_status_label, "Waiting for WiFi...");
  lv_obj_align(api_status_label, LV_ALIGN_TOP_MID, 0, 10);
  
  // Create API error label
  api_error_label = lv_label_create(api_tab);
  lv_label_set_text(api_error_label, "");
  lv_obj_align(api_error_label, LV_ALIGN_TOP_MID, 0, 30);
  lv_obj_set_style_text_color(api_error_label, lv_color_hex(0xFF0000), 0);
  
  // Create product name label
  product_name_label = lv_label_create(api_tab);
  lv_obj_set_style_text_font(product_name_label, &lv_font_montserrat_18, 0);
  lv_label_set_text(product_name_label, "Loading data...");
  lv_obj_align(product_name_label, LV_ALIGN_TOP_MID, 0, 60);
  
  // Create product details label
  product_details_label = lv_label_create(api_tab);
  lv_label_set_text(product_details_label, "Waiting for API data...");
  lv_obj_align(product_details_label, LV_ALIGN_TOP_LEFT, 10, 100);
  
  // Create manual refresh button
  lv_obj_t *refresh_btn = lv_btn_create(api_tab);
  lv_obj_add_event_cb(refresh_btn, refresh_btn_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_align(refresh_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
  
  lv_obj_t *refresh_label = lv_label_create(refresh_btn);
  lv_label_set_text(refresh_label, "Refresh Data");
  lv_obj_center(refresh_label);
}

void refresh_btn_event_handler(lv_event_t *e) {
  makeApiRequest();
}

