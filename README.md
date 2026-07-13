# ESP32 Automatic School Bell

An ESP32-based automatic school bell system that rings a physical bell according to a configurable schedule. The system synchronizes time using NTP, falls back to the ESP32 RTC when Wi-Fi is unavailable, and uses deep sleep to reduce power consumption.

---

## Features

- Automatic bell ringing based on a daily schedule
- Accurate time synchronization using NTP
- RTC fallback when Wi-Fi is unavailable
- Deep sleep for lower power consumption
- Automatic daily time synchronization
- Sunday holiday mode (no bells)
- Fully configurable bell schedule
- Easy configuration through separate header files

---

## Hardware Required

- ESP32 Development Board
- Relay Module (5V or 3.3V compatible)
- Electric School Bell
- Wi-Fi Network
- Power Supply or 5V Charger

---

## Project Structure

```
ESP32-Automatic-School-Bell/
│
├── ESP32-Automatic-School-Bell.ino    # Main application
├── config.h                           # Wi-Fi and hardware configuration
├── schedule.h                         # Bell schedule
├── README.md
├── LICENSE
└── images/
```

---

## Configuration

All project settings are stored in **config.h**.

### Wi-Fi

```cpp
constexpr char WIFI_SSID[] = "YOUR_WIFI_NAME";
constexpr char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
```

---

### Relay Pin

```cpp
constexpr uint8_t RELAY_PIN = 14;
```

Change this if your relay is connected to another GPIO.

---

### Time Zone

Default configuration is for **India (IST)**.

```cpp
constexpr long GMT_OFFSET_SECONDS = 19800;
constexpr int DAYLIGHT_OFFSET_SECONDS = 0;
```

---

### NTP Servers

```cpp
constexpr char NTP_SERVER_1[] = "pool.ntp.org";
constexpr char NTP_SERVER_2[] = "time.nist.gov";
constexpr char NTP_SERVER_3[] = "time.google.com";
```

---

### Bell Schedule

The daily timetable is stored in **schedule.h**.

Example:

```cpp
Period schedule[] = {
    {1, 8, 20, 1, 500, "First Period"},
    {2, 9, 20, 2, 500, "Second Period"},
    {3,10,10,3,500,"Third Period"}
};
```

Each entry contains:

| Field | Description |
|-------|-------------|
| id | Unique bell ID |
| hour | Hour (24-hour format) |
| minute | Minute |
| ringCount | Number of bell rings |
| ringDurationMS | Duration of each ring (milliseconds) |
| label | Bell name |

---

# How It Works

1. ESP32 starts.
2. Connects to Wi-Fi.
3. Synchronizes time using an NTP server.
4. Disconnects Wi-Fi to save power.
5. Continuously checks the current time.
6. Rings the bell when a scheduled time is reached.
7. Synchronizes time once every day.
8. Enters deep sleep when the next bell is several hours away.
9. Skips all bells on Sunday.

---

# Installation

## 1. Clone the repository

```bash
git clone https://github.com/YOUR_USERNAME/ESP32-Automatic-School-Bell.git
```

---

## 2. Open the project

Open the project in:

- Arduino IDE
- PlatformIO (VS Code)

---

## 3. Install ESP32 Board Package

Install the ESP32 board package from Arduino Boards Manager if it is not already installed.

---

## 4. Edit Configuration

Open **config.h** and update:

- Wi-Fi SSID
- Wi-Fi Password
- Relay Pin (if needed)
- Time Zone (if required)

---

# Changing the Bell Schedule

All bell timings are stored in the **schedule.h** file.

Open **schedule.h** and you will see something similar to this:

```cpp
Period schedule[] = {
    {1, 8, 20, 1, 500, "First Period"},
    {2, 9, 20, 2, 500, "Second Period"},
    {3,10,10,3,500,"Third Period"},
};
```

Each row represents **one bell**.

### Understanding One Schedule Entry

```cpp
{1, 8, 20, 1, 500, "First Period"}
```

This means:

| Value | Meaning |
|------|---------|
| `1` | Unique ID of the bell (keep each ID different) |
| `8` | Hour (24-hour format) |
| `20` | Minute |
| `1` | Number of times the bell rings |
| `500` | Duration of each ring in milliseconds |
| `"First Period"` | Name of the bell (shown in Serial Monitor only) |

---

## Time Format

The project uses **24-hour time**.

| Time | Write |
|------|------|
| 8:20 AM | `8, 20` |
| 9:45 AM | `9, 45` |
| 12:30 PM | `12, 30` |
| 1:15 PM | `13, 15` |
| 2:45 PM | `14, 45` |
| 5:00 PM | `17, 0` |

---

## Changing Bell Time

Suppose your first bell should ring at **8:30 AM** instead of **8:20 AM**.

Current:

```cpp
{1, 8, 20, 1, 500, "First Period"}
```

Change it to:

```cpp
{1, 8, 30, 1, 500, "First Period"}
```

Only the **20** changes to **30**.

---

## Changing Number of Rings

Current:

```cpp
{2, 9, 20, 2, 500, "Second Period"}
```

This rings **2 times**.

To ring **5 times**, change it to:

```cpp
{2, 9, 20, 5, 500, "Second Period"}
```

---

## Changing Ring Duration

Current:

```cpp
{2, 9, 20, 2, 500, "Second Period"}
```

The bell stays ON for **500 milliseconds** (half a second).

To keep the bell ON for **2 seconds**, change it to:

```cpp
{2, 9, 20, 2, 2000, "Second Period"}
```

Common values:

| Value | Time |
|------|------|
| 300 | 0.3 second |
| 500 | 0.5 second |
| 1000 | 1 second |
| 2000 | 2 seconds |
| 3000 | 3 seconds |

---

## Changing the Bell Name

Current:

```cpp
{3,10,10,3,500,"Third Period"}
```

You can rename it to anything:

```cpp
{3,10,10,3,500,"Lunch Break"}
```

or

```cpp
{3,10,10,3,500,"Assembly"}
```

The name is only displayed in the Serial Monitor for debugging. It does **not** affect how the bell works.

---

## Adding a New Bell

Copy an existing row.

Example:

```cpp
{4,11,30,1,500,"Lunch Break"},
```

Paste it below and modify it:

```cpp
{5,12,15,2,500,"Sports Period"},
```

Make sure the **ID number is unique**.

---

## Removing a Bell

Simply delete the corresponding row.

Example:

```cpp
{4,11,30,1,500,"Lunch Break"},
```

After deleting it, that bell will no longer ring.

---

## Important Notes

- Every bell must have a **unique ID**.
- Use **24-hour time**.
- Keep the commas exactly as shown.
- Do not remove the opening `{` or closing `}` braces.
- After making changes, save the file and upload the program to the ESP32 again.

---

## Example Schedule

```cpp
Period schedule[] = {
    {1, 8, 20, 1, 500, "Assembly"},
    {2, 8, 30, 1, 500, "First Period"},
    {3, 9, 20, 2, 500, "Second Period"},
    {4,10,10,3,500,"Third Period"},
    {5,11,00,1,2000,"Lunch Break"},
    {6,11,30,1,500,"Fourth Period"},
    {7,12,20,2,500,"Fifth Period"},
    {8,13,10,1,3000,"School Ends"}
};
```

----------------------------------------------------------------

## 6. Upload

Select your ESP32 board and upload the sketch.

----------------------------------------------------------------

## Wiring

| ESP32 | Relay |
|-------|-------|
| GPIO 14 | IN |
| 5V | VCC |
| GND | GND |

Connect the relay output to your electric bell according to your relay module specifications.

----------------------------------------------------------------

## Power Saving

The ESP32 automatically enters deep sleep when the next bell is far away.

This significantly reduces power consumption while keeping accurate timing.

----------------------------------------------------------------

## Customization

You can easily customize:

- Bell timings
- Number of rings
- Ring duration
- Relay GPIO
- Time zone
- Wi-Fi credentials
- Sleep behavior

without modifying the main program.

----------------------------------------------------------------

## Future Improvements

Possible future features include:

- Web interface for schedule editing
- OTA firmware updates
- Multiple weekday schedules
- LCD/OLED display support
- Buzzer support
- Holiday calendar
- Backup battery support
- Web dashboard

----------------------------------------------------------------

## License

This project is licensed under the MIT License.

See the **LICENSE** file for details.

----------------------------------------------------------------

## Author

**Anas**

If you find this project useful, consider giving it a ⭐ on GitHub.
