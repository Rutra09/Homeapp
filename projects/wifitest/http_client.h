/**
 * @file      http_client.h
 * @brief     HTTP client functions for making API requests
 */

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

/**
 * @brief Make a GET request to the API
 */
void makeApiRequest();

/**
 * @brief Parse and display API response on the screen
 * @param jsonResponse The JSON response string from the API
 */
void parseAndDisplayResponse(String jsonResponse);

/**
 * @brief Update the API data on the display
 */
void updateApiData();

/**
 * @brief Check if it's time to make an API request
 */
void checkApiRequest();

#endif // HTTP_CLIENT_H