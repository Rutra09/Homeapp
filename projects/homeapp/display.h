/**
 * @file      display.h
 * @brief     Display and UI functions
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"


/**
 * @brief Initialize the display
 * @return true if initialization was successful, false otherwise
 */
bool initDisplay();

/**
 * @brief Set up the UI elements for the home app
 */
void setupUI();

#endif // DISPLAY_H