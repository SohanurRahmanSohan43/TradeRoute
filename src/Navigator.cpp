#include "Navigator.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>

// Helper struct for the Priority Queue
// We need to store {Cost, City*} and sort by Cost (Ascending)
struct NodeRecord {
  int currentCost;
  City *city;

  // Operator overload for the priority_queue to sort correctly (Min-Heap)
  bool operator>(const NodeRecord &other) const {
    return currentCost > other.currentCost;
  }
};

std::vector<City *>
Navigator::findShortestPath(City *start, City *end,
                            std::map<std::string, City *> &world) {
  // 1. Setup Data Structures
  std::map<City *, int> minDistance;
  std::map<City *, City *> previousNode; // To reconstruct the path later
  std::priority_queue<NodeRecord, std::vector<NodeRecord>,
                      std::greater<NodeRecord>>
      pq;

  // Initialize distances to Infinity
  for (auto const &[name, cityPtr] : world) {
    minDistance[cityPtr] = std::numeric_limits<int>::max();
  }

  // 2. Start at the source
  minDistance[start] = 0;
  pq.push({0, start});

  while (!pq.empty()) {
    // Pop the city with the lowest known cost
    int cost = pq.top().currentCost;
    City *current = pq.top().city;
    pq.pop();

    // If we reached the destination, we can stop early
    if (current == end)
      break;

    // Optimization: If we found a faster way to this node already, skip
    if (cost > minDistance[current])
      continue;

    // 3. Explore Neighbors (Relaxation Step)
    for (auto &road : current->roads) {
      City *neighbor = road.destination;
      int edgeWeight = road.distance;
      int newCost = cost + edgeWeight;

      // If we found a cheaper path to the neighbor
      if (newCost < minDistance[neighbor]) {
        minDistance[neighbor] = newCost;
        previousNode[neighbor] = current; // Track path
        pq.push({newCost, neighbor});
      }
    }
  }

  // 4. Reconstruct the Path (Backtracking)
  std::vector<City *> path;
  City *crawl = end;

  if (minDistance[end] == std::numeric_limits<int>::max()) {
    return path; // Empty path means unreachable
  }

  while (crawl != nullptr) {
    path.push_back(crawl);
    // Move to the previous node, or null if we are at start
    if (crawl == start)
      break;
    crawl = previousNode[crawl];
  }

  // The path is currently End -> Start, so reverse it
  std::reverse(path.begin(), path.end());
  return path;
}

void Navigator::printRoute(const std::vector<City *> &path) {
  if (path.empty()) {
    std::cout << "[NAVIGATOR] Destination is unreachable!\n";
    return;
  }

  std::cout << ">>> OPTIMAL ROUTE CALCULATED: \n    ";
  int totalCost = 0;

  for (size_t i = 0; i < path.size(); ++i) {
    std::cout << path[i]->name;
    if (i < path.size() - 1) {
      std::cout << " -> ";
      // Simple lookahead to find distance (for display only)
      // In a real app, we'd cache this total cost
      for (auto &r : path[i]->roads) {
        if (r.destination == path[i + 1])
          totalCost += r.distance;
      }
    }
  }
  std::cout << "\n    Total Distance: " << totalCost << " km\n";
}