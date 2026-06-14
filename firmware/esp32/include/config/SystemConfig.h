#pragma once

constexpr int MAX_DRAWERS = 7;
constexpr int MAX_SCHEDULES = 10;
constexpr int MAX_SCHEDULE_TIMES = 4;
constexpr int MAX_DAYS_OF_WEEK = 7;
constexpr int MAX_PENDING_EVENTS = 10;

constexpr unsigned long REMINDER_INTERACTION_WINDOW_MS = 5UL * 60UL * 1000UL;

constexpr const char* CONFIG_FILE_PATH = "/config.json";
