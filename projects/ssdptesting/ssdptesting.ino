/**
 * @file      ssdptesting.ino
 * @license   MIT
 * @copyright Copyright (c) 2025  Shenzhen Xinyuan Electronic Technology Co., Ltd

 */
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>

// Network credentials - replace with your WiFi details
const char* ssid = "aqupark";
const char* password = "ProsteHaslo12";

// SSDP settings
const char* deviceType = "urn:schemas-upnp-org:device:ESP32:1";  // Must match what Node.js client is looking for
const char* deviceUUID = "38323636-4558-4dda-9188-cda0e6000000"; // Unique device UUID
const char* friendlyName = "ESP32 SSDP Device";
const int SSDP_PORT = 1900;
const IPAddress SSDP_MULTICAST_ADDR(239, 255, 255, 250);

// HTTP server on port 80 to serve device description
WebServer server(80);
// UDP instance for SSDP communication
WiFiUDP udp;

// Send SSDP NOTIFY announcement
void sendSSDP_NOTIFY() {
  // Format the LOCATION URL with the device's IP address
  char locationURL[64];
  sprintf(locationURL, "http://%s/description.xml", WiFi.localIP().toString().c_str());
  
  // Prepare NOTIFY message
  String notify = String("NOTIFY * HTTP/1.1\r\n") +
                  "HOST: 239.255.255.250:1900\r\n" +
                  "NT: " + deviceType + "\r\n" +
                  "NTS: ssdp:alive\r\n" +
                  "USN: uuid:" + deviceUUID + "::urn:schemas-upnp-org:device:ESP32:1\r\n" +
                  "CACHE-CONTROL: max-age=1800\r\n" +
                  "SERVER: Arduino/ESP32 UPnP/1.1 SSDP/1.0\r\n" +
                  "LOCATION: " + locationURL + "\r\n" +
                  "\r\n";
  
  // Send message
  udp.beginPacket(SSDP_MULTICAST_ADDR, SSDP_PORT);
  udp.write((const uint8_t*)notify.c_str(), notify.length());
  udp.endPacket();
  
  Serial.println("Sent SSDP NOTIFY announcement");
}

// Handle SSDP M-SEARCH discovery requests
void handleSSDP_MSEARCH(String requestString, IPAddress remoteIP, uint16_t remotePort) {
  // Check if it's an M-SEARCH request
  if (requestString.indexOf("M-SEARCH") < 0) return;
  
  // Check if it's looking for all devices or our specific type
  bool searchAll = (requestString.indexOf("ssdp:all") > 0);
  bool searchTarget = (requestString.indexOf(deviceType) > 0);
  
  if (searchAll || searchTarget) {
    // Format the LOCATION URL with the device's IP address
    char locationURL[64];
    sprintf(locationURL, "http://%s/description.xml", WiFi.localIP().toString().c_str());

    // Prepare M-SEARCH response
    String response = String("HTTP/1.1 200 OK\r\n") +
                      "EXT:\r\n" +
                      "CACHE-CONTROL: max-age=1800\r\n" +
                      "LOCATION: " + locationURL + "\r\n" +
                      "SERVER: Arduino/ESP32 UPnP/1.1 SSDP/1.0\r\n" +
                      "ST: " + deviceType + "\r\n" +
                      "USN: uuid:" + deviceUUID + "::" + deviceType + "\r\n" +
                      "\r\n";

    // Send response to the requester
    udp.beginPacket(remoteIP, remotePort);
    udp.write((const uint8_t*)response.c_str(), response.length());
    udp.endPacket();
    
    Serial.println("Responded to SSDP M-SEARCH from " + remoteIP.toString());
  }
}

// Serve the XML description document
void handleDescription() {
  String xml = "<?xml version=\"1.0\"?>\r\n"
               "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"
               "  <specVersion>\r\n"
               "    <major>1</major>\r\n"
               "    <minor>0</minor>\r\n"
               "  </specVersion>\r\n"
               "  <device>\r\n"
               "    <deviceType>" + String(deviceType) + "</deviceType>\r\n"
               "    <friendlyName>" + String(friendlyName) + "</friendlyName>\r\n"
               "    <manufacturer>ESP32 Maker</manufacturer>\r\n"
               "    <manufacturerURL>https://github.com/LilyGO</manufacturerURL>\r\n"
               "    <modelDescription>ESP32 SSDP Device</modelDescription>\r\n"
               "    <modelName>ESP32</modelName>\r\n"
               "    <UDN>uuid:" + String(deviceUUID) + "</UDN>\r\n"
               "    <presentationURL>/</presentationURL>\r\n"
               "  </device>\r\n"
               "</root>\r\n";
               
  server.send(200, "text/xml", xml);
  Serial.println("Served device description XML");
}

// Serve the root page
void handleRoot() {
  String html = "<html><head><title>ESP32 SSDP Device</title></head>"
                "<body><h1>ESP32 SSDP Device</h1>"
                "<p>This is an ESP32 running an SSDP server.</p>"
                "</body></html>";
                
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for Serial to initialize
  Serial.println("\n\nESP32 SSDP Server Starting...");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
  
  // Set up HTTP server routes
  server.on("/description.xml", HTTP_GET, handleDescription);
  server.on("/", HTTP_GET, handleRoot);
  server.begin();
  Serial.println("HTTP server started on port 80");
  
  // Start listening for UDP packets on the SSDP port
  udp.beginMulticast(SSDP_MULTICAST_ADDR, SSDP_PORT);
  Serial.println("SSDP service started");

  // Send initial SSDP presence announcement
  sendSSDP_NOTIFY();
}

void loop() {
  // Handle HTTP requests
  server.handleClient();
  
  // Handle incoming SSDP requests
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buffer[512] = {0};
    int len = udp.read(buffer, sizeof(buffer) - 1);
    if (len > 0) {
      buffer[len] = 0; // Null terminate
      String request = String(buffer);
      handleSSDP_MSEARCH(request, udp.remoteIP(), udp.remotePort());
    }
  }
  
  // Send NOTIFY message periodically (every 5 minutes)
  static unsigned long lastNotifyTime = 0;
  const unsigned long notifyInterval = 10* 1000; // 5 minutes
  
  if (millis() - lastNotifyTime > notifyInterval) {
    sendSSDP_NOTIFY();
    lastNotifyTime = millis();
  }
}