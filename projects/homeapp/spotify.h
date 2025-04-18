/**
 * @file      spotify.h
 * @brief     Spotify control interface using HTTP API
 */

#ifndef SPOTIFY_H
#define SPOTIFY_H

#include "config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Server configuration

#define Starting_Server_Host 0
#define SPOTIFY_SERVER_HOST_LEN 40
#define SPOTIFY_SERVER_PORT 3000
#define SPOTIFY_SERVER_PATH "/spotify"
#define SPOTIFY_SERVER_HOST_prefix "http://192.168.0."

/**
 * @brief Initialize Spotify API connection
 */
void initSpotify();

/**
 * @brief Update currently playing song info from API
 */
void updateNowPlaying();

/**
 * @brief Update album cover display using the API data
 */
void updateCoverArt();

/**
 * @brief Toggle play/pause via API
 */
void togglePlayPause();

/**
 * @brief Skip to next track via API
 */
void nextTrack();

/**
 * @brief Go back to previous track via API
 */
void previousTrack();

/**
 * @brief Check Spotify authentication status
 * @return true if authenticated, false otherwise
 */
bool checkSpotifyStatus();

/**
 * @brief Callback for play/pause button
 */
void spotify_play_callback(lv_event_t *e);

/**
 * @brief Callback for next track button
 */
void spotify_next_callback(lv_event_t *e);

/**
 * @brief Callback for previous track button
 */
void spotify_prev_callback(lv_event_t *e);

/**
 * @brief HTTP request to Spotify API server
 * @param endpoint API endpoint
 * @param method HTTP method (GET, PUT, POST)
 * @return JSON response as String
 */
String makeSpotifyRequest(const char* endpoint, const char* method);

/**
 * @brief Parse base64 image data from API response
 * @param base64Data Base64 encoded image
 * @return true if successful, false otherwise
 */
bool parseBase64Image(const char* base64Data);

/**
 * @brief Download image from URL
 * @param url URL of the image
 * @return true if successful, false otherwise
 */
bool downloadImageFromUrl(const char* url);

/**
 * @brief Process image data
 * @param data_len Length of the image data
 * @return true if successful, false otherwise
 */
bool processImageData(size_t data_len);

// Global reference to cover art placeholder
extern lv_obj_t *cover_img;

// Last fetched track data
extern bool lastFetchSuccess;
extern unsigned long lastFetchTime;
extern const long fetchInterval;

// Spotify API status
extern bool isConnected;


extern String host;

#endif // SPOTIFY_H