/*
   ================================================================
    Automatic School Bell System  —  ESP32
    Built with platformio / Arduino IDE
    Author:  <your name>
    License: MIT
    Repository: https://github.com/...
   ================================================================

   Features:
   - Rings physical bell via relay on a daily schedule.
   - Syncs time via NTP once a day.
   - Deep‑sleeps when far from next bell to save power.
   - Skips Sundays automatically.
   - All configuration is in config.h (Wi‑Fi, pins, timing).
   - Schedule is defined in schedule.h (easy to edit).
*/

#include <WiFi.h>
#include "time.h"
#include "config.h"
#include "schedule.h"

// ================== Global State ==================
int  lastSyncDay   = -1;         // track last NTP sync day of month
int  lastRungDay   = -1;         // day on which last bell was rung
int  lastRungBellID = -1;        // prevent ringing same bell twice in one day

// ================== Function Prototypes ==================
void ringBell(int count, int onDurationMS, int offDurationMS);
void checkAndSleepUntilNextBell();
void goToDeepSleepUntil(int nextHour, int nextMinute, int wakeBeforeMinutes);
bool connectAndSyncNTP();

// ================== Setup ==================
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);   // relay inactive at boot

  // ── First time synchronisation on power‑up ──
  // This loop retries until we have a valid time (either from NTP or RTC backup).
  while (true) {
    if (connectAndSyncNTP()) {
      Serial.println("✅ Initial NTP sync OK");
      break;
    }

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      Serial.printf("🕒 Using RTC backup: %02d:%02d:%02d\n",
                    timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      break;
    }

    Serial.println("⏳ No valid time – retrying in 1 s…");
    delay(1000);
  }
}

// ================== Main Loop ==================
void loop() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("⚠️  RTC read failed, trying NTP…");
    connectAndSyncNTP();
    delay(1000);
    return;
  }

  int currentHour    = timeinfo.tm_hour;
  int currentMinute  = timeinfo.tm_min;
  int currentDay     = timeinfo.tm_mday;
  int currentWeekday = timeinfo.tm_wday;  // 0 = Sunday

  // ── Daily NTP sync (once per day) ──
  if (currentDay != lastSyncDay) {
    Serial.println("🔄 Daily NTP sync…");
    if (connectAndSyncNTP())
      lastSyncDay = currentDay;
  }

  // ── Reset bell‑ID tracker on a new day ──
  if (currentDay != lastRungDay) {
    lastRungBellID = -1;          // allow all bells to ring today
    lastRungDay    = currentDay;
  }

  // ── Sunday: no school → deep‑sleep until Monday morning ──
  if (currentWeekday == 0) {
    Serial.println("📅 Sunday – sleeping until Monday 8:00…");
    goToDeepSleepUntil(schedule[0].hour, schedule[0].minute, WAKEUP_BEFORE_BELL_MINUTES);
    return; // never reaches here (deep sleep reboots)
  }

  // ── Check if we are inside a bell’s minute window ──
  int nowMins = currentHour * 60 + currentMinute;
  for (int i = 0; i < NUM_PERIODS; i++) {
    int bellMins = schedule[i].hour * 60 + schedule[i].minute;

    // Skip if already rung today
    if (schedule[i].id == lastRungBellID) continue;

    // Accept a ±1 minute window (the loop runs every 20 s, so this is safe)
    if (abs(nowMins - bellMins) <= 1) {
      Serial.printf("🔔 Ringing: %s (%02d:%02d)\n",
                    schedule[i].label.c_str(), currentHour, currentMinute);

      ringBell(schedule[i].ringCount, schedule[i].ringDurationMS, DEFAULT_RING_GAP_MS);

      lastRungBellID = schedule[i].id;    // mark as rung today
      break;                               // only one bell per loop iteration
    }
  }

  // ── Decide whether to stay awake or sleep ──
  checkAndSleepUntilNextBell();

  // ── Loop delay (short enough to catch a bell, long enough to save CPU) ──
  delay(LOOP_DELAY_MS);
}

// ================== Helper Functions ==================

// ---------- Ring the physical bell ----------
void ringBell(int count, int onDurationMS, int offDurationMS) {
  for (int i = 0; i < count; i++) {
    digitalWrite(RELAY_PIN, RELAY_ON);      // use the config constants
    delay(onDurationMS);
    digitalWrite(RELAY_PIN, RELAY_OFF);
    if (i < count - 1) delay(offDurationMS); // gap between rings (except last)
  }
}

// ---------- Smart sleep when far from next bell ----------
void checkAndSleepUntilNextBell() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int nowMins = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  int minDiff = 24 * 60;                     // start with worst case (tomorrow)
  int nextHour = -1, nextMinute = -1;

  // Find the next bell today or tomorrow
  for (int i = 0; i < NUM_PERIODS; i++) {
    int bellMins = schedule[i].hour * 60 + schedule[i].minute;
    int diff = bellMins - nowMins;
    if (diff <= 0) diff += 24 * 60;          // if already passed, look at tomorrow
    if (diff < minDiff) {
      minDiff    = diff;
      nextHour   = schedule[i].hour;
      nextMinute = schedule[i].minute;
    }
  }

  // If next bell is far away → deep sleep
  if (minDiff > SLEEP_THRESHOLD_MINUTES) {
    Serial.printf("💤 Next bell in %d min (%02d:%02d) – deep sleeping…\n",
                  minDiff, nextHour, nextMinute);
    goToDeepSleepUntil(nextHour, nextMinute, WAKEUP_BEFORE_BELL_MINUTES);
  } else {
    Serial.printf("⏰ Next bell in %d min – staying awake.\n", minDiff);
  }
}

// ---------- Enter deep sleep, wake up before next bell ----------
void goToDeepSleepUntil(int nextHour, int nextMinute, int wakeBeforeMinutes) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int nowMins = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  int targetMins = nextHour * 60 + nextMinute - wakeBeforeMinutes;
  if (targetMins <= nowMins) targetMins += 24 * 60;   // wrap to tomorrow

  int sleepMinutes = targetMins - nowMins;
  if (sleepMinutes <= 0) sleepMinutes = 1;            // safety minimum

  // ESP32 sleep timer takes microseconds (uint64_t)
  uint64_t sleepUs = (uint64_t)sleepMinutes * 60ULL * 1000000ULL;
  Serial.printf("😴 Sleeping %d min (wake %d min before bell).\n", sleepMinutes, wakeBeforeMinutes);

  esp_sleep_enable_timer_wakeup(sleepUs);
  esp_deep_sleep_start();         // ESP32 reboots after wake-up
}

// ---------- Connect to Wi‑Fi and sync time via NTP ----------
bool connectAndSyncNTP() {
  Serial.print("📶 Connecting to Wi‑Fi…");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);       // ← from config.h

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n❌ Wi‑Fi connection failed.");
    return false;
  }

  Serial.println("\n✅ Connected! Syncing NTP…");

  // Configure time using the servers and offset from config.h
  configTime(GMT_OFFSET_SECONDS, DAYLIGHT_OFFSET_SECONDS,
             NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {       // 5 s timeout
    Serial.printf("🕒 Time set: %02d:%02d:%02d\n",
                  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    lastSyncDay = timeinfo.tm_mday;           // remember successful sync day
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return true;
  } else {
    Serial.println("❌ NTP sync failed.");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    return false;
  }
}
