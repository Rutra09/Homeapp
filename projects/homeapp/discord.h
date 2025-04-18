/**
 * @file      discord.h
 * @brief     Discord control interface using HTTP API
 */

 #ifndef DISCORD_H
 #define DISCORD_H
 
 #include "config.h"
 #include <HTTPClient.h>
 #include <ArduinoJson.h>
 #include <lvgl.h>
 
 // Button event handlers - DECLARATIONS ONLY
 void discordMuteEvent(lv_event_t *e);
 void discordDeafenEvent(lv_event_t *e);
 void discordDisconnectEvent(lv_event_t *e);
 void discordUpdateStatus();
 
 #endif // DISCORD_H