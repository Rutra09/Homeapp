/**
 * @file      display.h
 * @brief     Display and UI functions
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "http_client.h"

/**
 * @brief Initialize the display
 * @return true if initialization was successful, false otherwise
 */
bool initDisplay();

/**
 * @brief Set up the UI elements
 */
void setupUI();

/**
 * @brief Button callback for manual refresh
 */
void refresh_btn_event_handler(lv_event_t *e);

#endif // DISPLAY_H

