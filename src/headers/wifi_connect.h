#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <Arduino.h>

// Function declarations
bool connectWiFi(const char* ssid, const char* password);
void disconnectWiFi();
bool isWiFiConnected();

#endif