#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "headers/wifi_connect.h"
#include "headers/apis.h"
#include "headers/colors.h"
#include <lvgl.h>
#include "ui/ui.h"
#include "config.h"


// Pin definitions and tft pin setup
#define TFT_CS 1
#define TFT_DC 0
#define TFT_RST -1
#define TFT_MOSI 8
#define TFT_SCLK 9
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Screen dimensions
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

// LVGL display buffers
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// variables
bool wifiConnected = false;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 1000; // Update every 1 second
int lastMinute = -1;

// LVGL display flush callback
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((uint16_t *)px_map, w * h);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting Display...");

    // Initialize display hardware
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    tft.init(240, 240);
    tft.setRotation(3);
    tft.invertDisplay(true);

    // Initialize LVGL
    lv_init();

    // Create LVGL display driver
    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Create splash screen
    lv_obj_t *splash = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(splash, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_scr_load(splash);

    // Add "Connecting..." label
    lv_obj_t *label_splash = lv_label_create(splash);
    lv_obj_set_style_text_color(label_splash, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_splash, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_label_set_text(label_splash, "Connecting to wifi...");
    lv_obj_align(label_splash, LV_ALIGN_CENTER, 0, -20);

    // Update display to show splash
    lv_timer_handler();

    // Connect to WiFi
    wifiConnected = connectWiFi(WIFI_SSID, WIFI_PASSWORD);

    if (wifiConnected)
    {
        Serial.println("WiFi Connected!");
        lv_label_set_text(label_splash, "WiFi Connected!");
        lv_timer_handler();
        delay(500);

        // Sync time
        lv_label_set_text(label_splash, "Syncing time...");
        lv_timer_handler();
        delay(1000);

        if (syncTime())
        {
            Serial.println("Time synced successfully");
            lv_label_set_text(label_splash, "Time synced!");
        }
        else
        {
            Serial.println("Failed to sync time");
            lv_label_set_text(label_splash, "Time sync failed");
        }

        lv_timer_handler();
        delay(1000);

        // Initialize EEZ Studio UI (loads main screen)
        ui_init();
        Serial.println("Main UI loaded");
    }
    else
    {
        Serial.println("WiFi Failed!");
        lv_label_set_text(label_splash, "WiFi Connection Failed!");
        lv_obj_set_style_text_color(label_splash, lv_color_hex(0xFF0000), LV_PART_MAIN);
        lv_timer_handler();
    }
}

void loop()
{
    lv_tick_inc(5);
    lv_timer_handler();
    ui_tick();

    if (!wifiConnected)
    {
        delay(1000);
        return;
    }

    unsigned long currentMillis = millis();

    // Update display every second
    if (currentMillis - lastUpdate >= UPDATE_INTERVAL)
    {
        lastUpdate = currentMillis;

        // Get current time and date
        int hours, minutes;
        int day, month, year;

        getTime(hours, minutes);
        getDate(day, month, year);

        // Update time label
        char time_str[10];
        snprintf(time_str, sizeof(time_str), "%02d:%02d", hours, minutes);
        lv_label_set_text(objects.lb_time, time_str);

        // Update date label
        char date_str[20];
        snprintf(date_str, sizeof(date_str), "%02d/%02d/%04d", day, month, year);
        lv_label_set_text(objects.lb_date, date_str);
    }

    delay(5);
}

