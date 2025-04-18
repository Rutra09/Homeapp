/**
 * @file      homeapp.ino
 * @license   MIT
 * @copyright Copyright (c) 2024
 * @date      2024-04-16
 * @brief     Home app showing date, time, and Spotify controls for LilyGo AMOLED Series
 */
#include <Arduino.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include "ui.h" // Include the UI header

LilyGo_AMOLED amoled;

void setup() {
    Serial.begin(115200);
    Serial.println("HomeAppV2 Starting");

    // Initialize the AMOLED display
    if (!amoled.begin()) {
        Serial.println("Failed to initialize AMOLED!");
        while (1);
    }

    // Initialize LVGL
    beginLvglHelper(amoled);

    // Initialize the UI defined in ui.cpp
    ui_init();

    Serial.println("Setup complete");
}


void loop() {
    // Handle LVGL tasks
    lv_timer_handler();
    delay(5); // Give LVGL time to process

    // Add logic here to:
    // - Update the time and date labels (ui_lbl_time, ui_lbl_date)
    // - Handle button events (ui_btn_m, ui_btn_h, ui_btn_d, ui_btn_prev, ui_btn_playpause, ui_btn_next)
    // - Update the album cover (ui_img_album_cover)
    // - Update the play/pause button icon based on state
}