/**
 * @file      spotify.cpp
 * @brief     Spotify control interface implementation using HTTP API
 */

#include "spotify.h"
#include <lvgl.h>
#include <WiFiUdp.h>


static StaticJsonDocument<16384> jsonBuffer;

// Last fetched track data
bool lastFetchSuccess = false;
unsigned long lastFetchTime = 0;
const long fetchInterval = 5000; // Fetch every 5 seconds
int currentHost = Starting_Server_Host;
// Spotify API status
bool isConnected = false;
String currentTrackId = "";

String host = "";

// SSDP settings
const char* deviceType = "urn:schemas-upnp-org:device:SpotifyServer:1";  // Device type to search for
const int SSDP_PORT = 1900;
const IPAddress SSDP_MULTICAST_ADDR(239, 255, 255, 250);
WiFiUDP udp;  // Global UDP instance for SSDP discovery

bool findSpotifyServerWithSSDP() {
  Serial.println("Looking for Spotify server via SSDP...");
  
  // Start the UDP multicast listener
  if (!udp.beginMulticast(SSDP_MULTICAST_ADDR, SSDP_PORT)) {
    Serial.println("Failed to set up SSDP multicast listener");
    return false;
  }
  
  // Send SSDP M-SEARCH request
  String searchMessage = 
    "M-SEARCH * HTTP/1.1\r\n"
    "HOST: 239.255.255.250:1900\r\n"
    "MAN: \"ssdp:discover\"\r\n"
    "MX: 3\r\n"  // Response delay in seconds
    "ST: " + String(deviceType) + "\r\n"  // Search Target - the device type we're looking for
    "\r\n";
  
  // Send the search message
  udp.beginPacket(SSDP_MULTICAST_ADDR, SSDP_PORT);
  udp.write((const uint8_t*)searchMessage.c_str(), searchMessage.length());
  udp.endPacket();
  
  Serial.println("SSDP M-SEARCH request sent");
  
  // Wait for SSDP responses (with timeout)
  unsigned long startTime = millis();
  const unsigned long timeout = 5000; // 5 second timeout
  
  while (millis() - startTime < timeout) {
    // Check for incoming packets
    int packetSize = udp.parsePacket();
    if (packetSize) {
      char buffer[512] = {0};
      int len = udp.read(buffer, sizeof(buffer) - 1);
      if (len > 0) {
        buffer[len] = 0; // Null terminate
        String response = String(buffer);
        
        // Check if this is an SSDP response to our search
        if (response.indexOf("HTTP/1.1 200 OK") >= 0 && 
            response.indexOf(deviceType) >= 0) {
          
          // Extract LOCATION URL from the response
          int locStart = response.indexOf("LOCATION: ") + 10;
          if (locStart > 10) {  // Found LOCATION header
            int locEnd = response.indexOf("\r\n", locStart);
            if (locEnd > locStart) {
              String locationUrl = response.substring(locStart, locEnd);
              locationUrl.trim();
              
              // Extract host and port from the URL
              // Example: http://192.168.0.100:3000/description.xml
              int protocolEnd = locationUrl.indexOf("://") + 3;
              int pathStart = locationUrl.indexOf("/", protocolEnd);
              int portStart = locationUrl.indexOf(":", protocolEnd);
              
              String serverAddress;
              int serverPort = SPOTIFY_SERVER_PORT; // Default port
              
              if (portStart > 0 && portStart < pathStart) {
                // Port is specified in the URL
                serverAddress = locationUrl.substring(protocolEnd, portStart);
                serverPort = locationUrl.substring(portStart + 1, pathStart).toInt();
              } else {
                // No port in URL, use default port
                serverAddress = locationUrl.substring(protocolEnd, pathStart);
              }
              
              // Set the host string for API requests
              host = "http://" + serverAddress + ":" + String(serverPort);
              
              Serial.printf("Spotify server found at %s\n", host.c_str());
              udp.stop();
              return true;
            }
          }
        }
      }
    }
    delay(50); // Short delay to prevent CPU hogging
  }
  
  udp.stop();
  Serial.println("No SSDP Spotify services found within timeout");
  return false;
}


void initSpotify() {
  Serial.println("Initializing Spotify API integration...");

  // First try SSDP discovery
  if (!findSpotifyServerWithSSDP()) {
    Serial.println("Failed to find Spotify server via SSDP.");
    // If SSDP fails, try mDNS discovery
   
  } 

  // Check if Spotify server is available
  if (checkSpotifyStatus()) {
    Serial.println("Spotify API connected!");
    isConnected = true;
    // Get initial playback state
    updateNowPlaying();
  } else {
    Serial.println("Failed to connect to Spotify API.");
    isConnected = false;
    // Update UI to indicate disconnected state
    lv_label_set_text(song_title_label, "Spotify not connected");
    lv_label_set_text(artist_label, "Check server status");
  }
}

String makeSpotifyRequest(const char* endpoint, const char* method) {
  HTTPClient http;
  String url = String(host) + String(endpoint);


  Serial.print("Making request to: ");
  Serial.println(url);

  http.begin(url);

  int httpCode = 0;
  if (strcmp(method, "GET") == 0) {
    httpCode = http.GET();
  } else if (strcmp(method, "PUT") == 0) {
    httpCode = http.PUT("");
  } else if (strcmp(method, "POST") == 0) {
    httpCode = http.POST("");
  }

  String payload = "";
  if (httpCode > 0) {
    payload = http.getString();
  } else {
    Serial.print("HTTP request failed, error: ");
    Serial.print(http.errorToString(httpCode));
    Serial.print(", code: ");
    Serial.println(httpCode);

  }

  http.end();
  return payload;
}

bool checkSpotifyStatus() {
  String response = makeSpotifyRequest("/spotify/status", "GET");

  if (response.length() > 0) {
    DeserializationError error = deserializeJson(jsonBuffer, response);
    if (!error) {
      return jsonBuffer["isLoggedIn"];
    } else {
        Serial.print("JSON parsing error in checkSpotifyStatus: ");
        Serial.println(error.c_str());
    }
  }

  return false;
}

void updateNowPlaying() {
  unsigned long currentTime = millis();

  // Only fetch new data every fetchInterval ms
  if (currentTime - lastFetchTime < fetchInterval) {
    return;
  }

  lastFetchTime = currentTime;

  if (!isConnected) {
    // Try to reconnect
    isConnected = checkSpotifyStatus();
    if (!isConnected) {
        // Update UI if still not connected
        lv_label_set_text(song_title_label, "Spotify not connected");
        lv_label_set_text(artist_label, "Check server status");
        // Ensure play/pause button shows play when disconnected
        lv_obj_t *play_label = lv_obj_get_child(play_btn, 0);
        if (play_label) {
            lv_label_set_text(play_label, LV_SYMBOL_PLAY);
        }
        isPlaying = false; // Assume not playing if disconnected
        return;
    }
    // If reconnected, proceed to fetch data
    Serial.println("Spotify reconnected!");
  }

  String response = makeSpotifyRequest("/spotify/now-playing", "GET");

  if (response.length() > 0) {
    DeserializationError error = deserializeJson(jsonBuffer, response);

    if (!error) {
      lastFetchSuccess = true;

      // Check if a track is playing
      isPlaying = jsonBuffer["isPlaying"];

      // Update play/pause button icon
      lv_obj_t *play_label = lv_obj_get_child(play_btn, 0);
      if (play_label) { // Check if play_label exists
          if (isPlaying) {
            lv_label_set_text(play_label, LV_SYMBOL_PAUSE);
          } else {
            lv_label_set_text(play_label, LV_SYMBOL_PLAY);
          }
      } else {
          Serial.println("Error: play_btn label not found!");
      }

      if (isPlaying) {
        // Extract track info
        const char* title = jsonBuffer["title"] | "Unknown Title"; // Provide default
        String artists = "";

        JsonArray artistsArray = jsonBuffer["artists"];
        if (!artistsArray.isNull()) {
            for (int i = 0; i < artistsArray.size(); i++) {
              if (i > 0) artists += ", ";
              artists += artistsArray[i].as<String>();
            }
        } else {
            artists = "Unknown Artist";
        }

        // Track ID
        currentTrackId = jsonBuffer["id"].as<String>();

        // Update UI
        lv_label_set_text(song_title_label, title);
        lv_label_set_text(artist_label, artists.c_str());
      } else {
        // Nothing playing
        lv_label_set_text(song_title_label, "Not playing");
        lv_label_set_text(artist_label, "");
      }
    } else {
      Serial.print("JSON parsing error in updateNowPlaying: ");
      Serial.println(error.c_str());
      lastFetchSuccess = false;
    }
  } else {
    lastFetchSuccess = false;
    Serial.println("Failed to get now-playing data (empty response)");
  }

  if (!lastFetchSuccess) {
    // Update UI to indicate error state if fetch failed
    lv_label_set_text(song_title_label, "Spotify API error");
    lv_label_set_text(artist_label, "Check connection");
    // Ensure play/pause button shows play on error
    lv_obj_t *play_label = lv_obj_get_child(play_btn, 0);
    if (play_label) {
        lv_label_set_text(play_label, LV_SYMBOL_PLAY);
    }
    isPlaying = false; // Assume not playing on error
  }
}

void togglePlayPause() {
  if (!isConnected) {
    Serial.println("Spotify not connected, attempting reconnect...");
    initSpotify(); // Try to reconnect
    if (!isConnected) {
        Serial.println("Reconnect failed.");
        return;
    }
  }

  Serial.println("Toggle play/pause via API");

  const char* endpoint = isPlaying ? "/spotify/playback/pause" : "/spotify/playback/play";
  String response = makeSpotifyRequest(endpoint, "PUT");

  // Clear previous JSON data before parsing new response
  jsonBuffer.clear();
  DeserializationError error = deserializeJson(jsonBuffer, response);

  bool success = false;
  if (!error && jsonBuffer.containsKey("success")) {
      success = jsonBuffer["success"];
  }

  if (success) {
      Serial.printf("Playback %s successful\n", isPlaying ? "pause" : "play");
      // Toggle state immediately for responsiveness
      isPlaying = !isPlaying;
      // Update button icon immediately
      lv_obj_t *play_label = lv_obj_get_child(play_btn, 0);
      if (play_label) {
          lv_label_set_text(play_label, isPlaying ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
      }
  } else {
      Serial.printf("Failed to %s playback. Error: %s\n", isPlaying ? "pause" : "play", error.c_str());
      // Don't change isPlaying state if API call failed
  }

  // Fetch updated state after a short delay to confirm
  delay(200); // Increased delay slightly
  updateNowPlaying();
}

void nextTrack() {
  if (!isConnected) {
    Serial.println("Spotify not connected");
    return;
  }
   if (!isPlaying) {
    Serial.println("Not playing, cannot skip next");
    return;
  }

  Serial.println("Skip to next track via API");

  String response = makeSpotifyRequest("/spotify/skip/next", "POST");
  // Clear previous JSON data before parsing new response
  jsonBuffer.clear();
  DeserializationError error = deserializeJson(jsonBuffer, response);
  if (!error && jsonBuffer.containsKey("success") && jsonBuffer["success"]) {
    Serial.println("Skipped to next track successfully");
    // Optimistically update UI slightly faster
    lv_label_set_text(song_title_label, "Loading next...");
    lv_label_set_text(artist_label, "");
  } else {
    Serial.printf("Failed to skip to next track. Error: %s\n", error.c_str());
  }

  // Update the UI after a delay to get new track info
  delay(500); // Increased delay to allow Spotify state to update
  updateNowPlaying();
}

void previousTrack() {
   if (!isConnected) {
    Serial.println("Spotify not connected");
    return;
  }
   if (!isPlaying) {
    Serial.println("Not playing, cannot skip previous");
    return;
  }

  Serial.println("Go to previous track via API");

  String response = makeSpotifyRequest("/spotify/skip/previous", "POST");
  // Clear previous JSON data before parsing new response
  jsonBuffer.clear();
  DeserializationError error = deserializeJson(jsonBuffer, response);
  if (!error && jsonBuffer.containsKey("success") && jsonBuffer["success"]) {
    Serial.println("Skipped to previous track successfully");
    // Optimistically update UI slightly faster
    lv_label_set_text(song_title_label, "Loading previous...");
    lv_label_set_text(artist_label, "");
  } else {
    Serial.printf("Failed to skip to previous track. Error: %s\n", error.c_str());
  }

  // Update the UI after a delay to get new track info
  delay(500); // Increased delay to allow Spotify state to update
  updateNowPlaying();
}

// Button callbacks
void spotify_play_callback(lv_event_t *e) {
  togglePlayPause();
}

void spotify_next_callback(lv_event_t *e) {
  nextTrack();
}

void spotify_prev_callback(lv_event_t *e) {
  previousTrack();
}