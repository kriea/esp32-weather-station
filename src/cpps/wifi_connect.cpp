// For ESP8266, you would use: #include <ESP8266WiFi.h>
#include "headers/wifi_connect.h"
#include <WiFi.h> // For ESP32

// const char *ssid = "FRITZ!Box 7530 ZQ";
// const char *password = "37933457293543414693";

bool connectWiFi(const char *ssid, const char *password)
{
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); // Reduce from default 20dBm
  WiFi.begin(ssid, password);

  // Wait for the connection to establish (max 20 seconds)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi connection failed!");
    return false;
  }
}


void disconnectWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi Disconnected");
}

bool isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}