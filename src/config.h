#ifndef CONFIG_H
#define CONFIG_H

// ======================================================
//                  Wi-Fi Configuration
// ======================================================

// Rename this file to config.h after copying.
// Do NOT commit your real Wi-Fi credentials to GitHub.

constexpr char WIFI_SSID[] = "YOUR_WIFI_NAME";
constexpr char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";

// ======================================================
//                 Time Configuration
// ======================================================

// Indian Standard Time (UTC +5:30)
constexpr long GMT_OFFSET_SECONDS = 19800;
constexpr int DAYLIGHT_OFFSET_SECONDS = 0;

// NTP Servers
constexpr char NTP_SERVER_1[] = "pool.ntp.org";
constexpr char NTP_SERVER_2[] = "time.nist.gov";
constexpr char NTP_SERVER_3[] = "time.google.com";

// ======================================================
//                 Hardware Configuration
// ======================================================

// Relay output pin
constexpr uint8_t RELAY_PIN = 14;

// Relay logic
constexpr bool RELAY_ON = LOW;
constexpr bool RELAY_OFF = HIGH;

// ======================================================
//                 Power Saving
// ======================================================

// Go to deep sleep if next bell is farther away than this.
constexpr int SLEEP_THRESHOLD_MINUTES = 180;

// Wake this many minutes before the next bell.
constexpr int WAKEUP_BEFORE_BELL_MINUTES = 60;

// Main loop interval
constexpr unsigned long LOOP_DELAY_MS = 20000;

// Gap between multiple bell rings
constexpr unsigned long DEFAULT_RING_GAP_MS = 700;

// Wi-Fi timeout
constexpr unsigned long WIFI_TIMEOUT_MS = 15000;

#endif
