#ifndef STATSVALIDATOR_H
#define STATSVALIDATOR_H

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

class StatsValidator {
public:
  // Categories for our random events
  enum EventType {
    EVENT_NONE = 0,
    EVENT_BUMPER_CROP = 1,
    EVENT_GOLD_RUSH = 2,
    EVENT_BOOM = 3,
    EVENT_GLOBAL_BOOM = 4,
    EVENT_GLOBAL_WAR = 5
  };

  // Log an observation (Data Science: Data Collection)
  static void logEvent(EventType e);

  // Run the Chi-Square Goodness of Fit Test (Statistics)
  static void printReport();

private:
  // Store frequency of events: Map<EventType, Count>
  static std::map<int, int> eventCounts;
  static int totalObservations;
};

#endif