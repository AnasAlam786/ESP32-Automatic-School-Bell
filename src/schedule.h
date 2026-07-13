#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <Arduino.h>

struct BellEvent {
  int   id;             // unique ID (resets daily)
  int   hour;
  int   minute;
  int   ringCount;      // how many rings
  int   ringDurationMS; // each ring on‑time (ms)
  String label;         // description
};

// Daily schedule – edit times here
BellEvent schedule[] = {
  { 1,  8, 20, 1, 2000, "Prayer"       },
  { 2,  8, 30, 1,  500, "1st Period"   },
  { 3,  9, 20, 2,  500, "2nd Period"   },
  { 4, 10, 10, 3,  500, "3rd Period"   },
  { 5, 11,  0, 1, 2000, "Lunch"        },
  { 6, 11, 15, 4,  500, "4th Period"   },
  { 7, 12, 10, 5,  500, "5th Period"   },
  { 8, 13,  0, 1, 3000, "School Ends"  },
};

constexpr int NUM_PERIODS = sizeof(schedule) / sizeof(schedule[0]);

#endif
