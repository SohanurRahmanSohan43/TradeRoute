#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "City.h"
#include <map>
#include <string>
#include <vector>

class Navigator {
public:
  // [GRAPH THEORY]: Dijkstra's Algorithm
  // Calculates the shortest path between start and end nodes.
  // Returns a vector of City pointers representing the path.
  static std::vector<City *>
  findShortestPath(City *start, City *end,
                   std::map<std::string, City *> &world);

  // Helper to print the route nicely to console
  static void printRoute(const std::vector<City *> &path);
};

#endif