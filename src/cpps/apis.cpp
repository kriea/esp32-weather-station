#include <headers/apis.h>
#include <WiFi.h>
#include <time.h>

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 1*3600; 


//Syncs with NTP and sets system clock to correct time
bool syncTime() {
    Serial.println("Syncing time with NTP server...");
    
    configTime(gmtOffset_sec, 0, ntpServer);
    struct tm timeinfo;
    int attempts = 0;

    while (!getLocalTime(&timeinfo) && attempts < 10) {
        Serial.print(".");
        delay(1000);
        attempts++;
    }
    
    if (attempts < 10) {
        Serial.println("\nTime synchronized!");
        Serial.print("Current time: ");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
        return true;
    } else {
        Serial.println("\nFailed to sync time!");
        return false;
    }
}

//asks system clock the time, no NTP calls
void getTime(int &hours, int &minutes) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        hours = timeinfo.tm_hour;
        minutes = timeinfo.tm_min;
    } else {
        hours = 0;
        minutes = 0;
    }
}
//asks system clock the date, no NTP calls
void getDate(int &day, int &month, int &year) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        day = timeinfo.tm_mday;
        month = timeinfo.tm_mon + 1;
        year = timeinfo.tm_year + 1900;
    } else {
        day = 6;
        month = 9;
        year = 420;
    }
}

