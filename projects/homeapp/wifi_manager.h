/**
 * @file      wifi_manager.h
 * @brief     WiFi connection management for HomeApp
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "config.h"

/**
 * @brief Initialize WiFi module
 */
void initWiFi();

/**
 * @brief Connect to WiFi network
 */
void connectToWiFi();

/**
 * @brief Check WiFi connection status
 */
void checkWiFiStatus();

#endif // WIFI_MANAGER_H