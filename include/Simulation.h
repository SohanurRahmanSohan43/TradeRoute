#ifndef SIMULATION_H
#define SIMULATION_H

#include "City.h"
#include <map>
#include <string>
#include <vector>

#include <random> // For std::mt19937

class Simulation {
public:
  // The Master Function: Advances the world by 1 "Day"
  static void advanceTime(std::map<std::string, City *> &world);

private: // Only simulation should access these functions
  // [PROBABILITY]: Randomly triggers news events
  static void triggerRandomEvent(std::map<std::string, City *> &world);

  // [ECONOMICS]: Simulates passive consumption (people eating grain)
  static void consumeResources(City *c);

  // [RNG]: Mersenne Twister Engine for high-quality random numbers
  static std::mt19937 rng;
};

#endif