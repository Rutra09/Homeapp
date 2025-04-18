/**
 * @file      wifi_manager.h
 * @brief     WiFi connection management functions
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "config.h"

/**
 * @brief Initialize WiFi settings
 */
void initWiFi();

/**
 * @brief Connect to WiFi network
 */
void connectToWiFi();

/**
 * @brief Update WiFi status on the display
 */
void updateWiFiStatus();

/**
 * @brief Check WiFi connection status and reconnect if necessary
 */
void checkWiFiStatus();

#endif // WIFI_MANAGER_H