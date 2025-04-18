#ifndef SSDP_DISCOVERY_H
#define SSDP_DISCOVERY_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#include "config.h"

// SSDP Constants
#define SSDP_PORT 1900
#define SSDP_MULTICAST_ADDR IPAddress(239, 255, 255, 250)
#define SSDP_DEVICE_TYPE_TO_FIND "urn:schemas-upnp-org:device:ESP32:1"

// Discovery timeout in milliseconds
#define SSDP_DISCOVERY_TIMEOUT 10000

// Structure to hold discovered server information
struct DiscoveredServer {
    IPAddress ip;
    String locationUrl;
    bool found;
};

// UDP instance for SSDP discovery
WiFiUDP ssdpUDP;

// Function to perform SSDP discovery and find the Spotify server
DiscoveredServer discoverSpotifyServer() {
    DiscoveredServer result;
    result.found = false;
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Cannot discover server: WiFi not connected");
        return result;
    }
    
    Serial.println("Starting SSDP discovery for Spotify server...");
    
    // Begin listening for UDP packets on the SSDP port
    if (!ssdpUDP.beginMulticast(SSDP_MULTICAST_ADDR, SSDP_PORT)) {
        Serial.println("Failed to begin multicast");
        return result;
    }
    
    // Prepare M-SEARCH request
    String msearch = String("M-SEARCH * HTTP/1.1\r\n") +
                    "HOST: 239.255.255.250:1900\r\n" +
                    "MAN: \"ssdp:discover\"\r\n" +
                    "MX: 5\r\n" +
                    "ST: " + SSDP_DEVICE_TYPE_TO_FIND + "\r\n" +
                    "\r\n";
    
    // Send M-SEARCH request
    ssdpUDP.beginPacket(SSDP_MULTICAST_ADDR, SSDP_PORT);
    ssdpUDP.write((uint8_t*)msearch.c_str(), msearch.length());
    ssdpUDP.endPacket();
    
    Serial.println("Sent SSDP M-SEARCH request, waiting for responses...");
    
    // Start the discovery timeout
    unsigned long startTime = millis();
    
    // Listen for responses until timeout
    while (millis() - startTime < SSDP_DISCOVERY_TIMEOUT) {
        // Check for incoming packets
        int packetSize = ssdpUDP.parsePacket();
        if (packetSize) {
            // Read the response into a buffer
            char buffer[512] = {0};
            int len = ssdpUDP.read(buffer, sizeof(buffer) - 1);
            if (len > 0) {
                buffer[len] = 0; // Null terminate
                String response = String(buffer);
                
                // Check if this is a response to our M-SEARCH
                if (response.indexOf("HTTP/1.1 200 OK") >= 0 && 
                    response.indexOf(SSDP_DEVICE_TYPE_TO_FIND) >= 0) {
                    
                    // Extract the LOCATION URL
                    int locationStart = response.indexOf("LOCATION:") + 9;
                    if (locationStart > 9) { // Found "LOCATION:"
                        int locationEnd = response.indexOf("\r\n", locationStart);
                        String locationUrl = response.substring(locationStart, locationEnd);
                        locationUrl.trim();
                        
                        // Extract IP address from the location URL
                        int protocolEnd = locationUrl.indexOf("://") + 3;
                        int portStart = locationUrl.indexOf(":", protocolEnd);
                        String ipStr = locationUrl.substring(protocolEnd, portStart);
                        
                        // Store the server information
                        result.ip = IPAddress();
                        result.ip.fromString(ipStr);
                        result.locationUrl = locationUrl;
                        result.found = true;
                        
                        Serial.println("Found server via SSDP: " + ipStr);
                        Serial.println("Location URL: " + locationUrl);
                        break;
                    }
                }
            }
        }
        delay(100); // Short delay between checks
    }
    
    // Stop UDP 
    ssdpUDP.stop();
    
    if (!result.found) {
        Serial.println("No matching server found within timeout");
    }
    
    return result;
}

#endif // SSDP_DISCOVERY_H