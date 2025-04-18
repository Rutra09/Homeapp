/**
 * @file      clock.cpp
 * @brief     Clock implementation for HomeApp
 */

#include "clock.h"

// Days of the week names
const char* const DAYS_OF_WEEK[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// Month names
const char* const MONTH_NAMES[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void initClock() {
  // Configure NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  
  // Set timezone to your local timezone
  // Based on the 6-hour difference, you seem to be in UTC+1 (Central European Time)
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();
  
  // Force a time sync
  Serial.println("Syncing time with NTP server...");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 10000)) {  // 10-second timeout for sync
    Serial.println("Failed to obtain time from NTP");
  } else {
    Serial.println("Time synchronized successfully");
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    Serial.print("Current time: ");
    Serial.println(timeStr);
  }
  
  Serial.println("Clock initialized with NTP");
}

void updateClock() {
  unsigned long currentMillis = millis();
  
  // Only update every clockUpdateInterval ms to avoid excessive updates
  if (currentMillis - lastClockUpdate >= clockUpdateInterval) {
    lastClockUpdate = currentMillis;
    
    // Get current time
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    
    // Format date string: "DD.MM.YY"
    char dateStr[10];
    snprintf(dateStr, sizeof(dateStr), "%02d.%02d.%02d", 
             timeinfo.tm_mday,
             timeinfo.tm_mon + 1, 
             (timeinfo.tm_year + 1900) % 100);
    lv_label_set_text(date_label, dateStr);
    
    // Format time string: "HH:MM" (24-hour format)
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", 
             timeinfo.tm_hour, 
             timeinfo.tm_min);
    lv_label_set_text(time_label, timeStr);
  }
}