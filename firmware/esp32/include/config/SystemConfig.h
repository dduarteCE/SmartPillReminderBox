#pragma once

#include <Arduino.h>

constexpr int MAX_DRAWERS = 7;
constexpr int MAX_SCHEDULES = 10;
constexpr int MAX_SCHEDULE_TIMES = 4;
constexpr int MAX_DAYS_OF_WEEK = 7;
constexpr int MAX_PENDING_EVENTS = 10;

constexpr unsigned long REMINDER_INTERACTION_WINDOW_MS = 5UL * 60UL * 1000UL;

constexpr const char* CONFIG_FILE_PATH = "/config.json";
constexpr const char* DEVICE_AP_SSID = "SmartPillBox";
constexpr const char* DEVICE_AP_PASSWORD = "smartpill123";
constexpr int WEBSOCKET_PORT = 81;

constexpr uint8_t LCD_I2C_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;
