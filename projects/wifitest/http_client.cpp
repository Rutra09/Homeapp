/**
 * @file      http_client.cpp
 * @brief     HTTP client functions implementation
 */

#include "http_client.h"

// Last time API request was made
unsigned long lastApiRequestTime = 0;
const long apiRequestInterval = 30000; // Request every 30 seconds

// Product data from API
String productName = "Loading...";
String productYear = "----";
String productPrice = "----.--";
String productCPU = "Loading...";
String productDisk = "Loading...";

void makeApiRequest() {
  // Only make request if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    Serial.println("Making API request...");
    lv_label_set_text(api_status_label, "Requesting data...");
    
    // Specify the URL
    http.begin("https://api.restful-api.dev/objects/7");
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println("API Response:");
      Serial.println(payload);
      
      parseAndDisplayResponse(payload);
      lv_label_set_text(api_status_label, "Request successful");
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      lv_label_set_text(api_status_label, "Request failed");
      lv_label_set_text_fmt(api_error_label, "Error: %d", httpResponseCode);
    }
    
    // Free resources
    http.end();
  } else {
    lv_label_set_text(api_status_label, "WiFi not connected");
  }
  
  // Update last request time
  lastApiRequestTime = millis();
}

void parseAndDisplayResponse(String jsonResponse) {
  // Allocate the JSON document
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<512> doc;

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, jsonResponse);
  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    lv_label_set_text(api_error_label, "JSON parse error");
    return;
  }

  // Extract values
  productName = doc["name"].as<String>();
  productYear = doc["data"]["year"].as<String>();
  productPrice = doc["data"]["price"].as<String>();
  productCPU = doc["data"]["CPU model"].as<String>();
  productDisk = doc["data"]["Hard disk size"].as<String>();
  
  // Update the UI with the data
  updateApiData();
}

void updateApiData() {
  lv_label_set_text(product_name_label, productName.c_str());
  lv_label_set_text_fmt(product_details_label, 
    "Year: %s\nPrice: $%s\nCPU: %s\nDisk: %s",
    productYear.c_str(),
    productPrice.c_str(),
    productCPU.c_str(),
    productDisk.c_str()
  );
}

void checkApiRequest() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastApiRequestTime >= apiRequestInterval) {
    makeApiRequest();
  }
}