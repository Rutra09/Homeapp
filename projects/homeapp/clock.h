/**
 * @file      clock.h
 * @brief     Clock functionality for HomeApp
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "config.h"

/**
 * @brief Initialize the clock and NTP synchronization
 */
void initClock();

/**
 * @brief Update the clock display
 */
void updateClock();

#endif // CLOCK_H