#include "Merchant.h"
#include "Simulation.h" // <--- Needed to advance time
#include <algorithm>    // For std::max
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <thread>

Merchant::Merchant(City *startCity, double startCash) {
  // this means using objects own variables
  this->currentLocation = startCity;
  this->cash = startCash;
  this->inventory.resize(City::COMMODITIES.size(), 0);
}

void Merchant::printStatus() {
  std::cout << "\n========================================\n";
  std::cout << " MERCHANT PROFILE | Location: " << currentLocation->name
            << "\n";
  std::cout << "========================================\n";
  std::cout << " CASH: $" << std::fixed << std::setprecision(2) << cash << "\n";

  std::cout << " INVENTORY:\n";
  for (size_t i = 0; i < inventory.size(); ++i) {
    if (inventory[i] > 0) {
      std::cout << "   - " << City::COMMODITIES[i].name << ": " << inventory[i]
                << " " << City::COMMODITIES[i].unit << "\n";
    }
  }
  std::cout << "========================================\n";
}

// [UPDATED LOGIC]
bool Merchant::travelTo(City *destination,
                        std::map<std::string, City *> &world) {
  if (!destination)
    return false;

  // 1. Find Distance
  int dist = -1;
  for (auto &road : currentLocation->roads) {
    if (road.destination == destination) {
      dist = road.distance;
      break;
    }
  }

  if (dist == -1) {
    std::cout << "[ERROR] No direct road to " << destination->name << ".\n";
    return false;
  }

  // 2. Logistics & Math
  long long totalWeight =
      std::accumulate(inventory.begin(), inventory.end(), 0LL);
  int freightCost = static_cast<int>(totalWeight * 0.5);
  double totalCost = dist + freightCost;

  if (cash < totalCost) {
    std::cout << "[FAIL] Insufficient funds (Need $" << totalCost << ").\n";
    return false;
  }

  // 3. CALCULATE TIME (100 km = 1 Day)
  int days = dist / 100;
  if (days < 1)
    days = 1; // Minimum 1 day travel

  // 4. EXECUTE TRAVEL
  cash -= totalCost;
  currentLocation = destination;

  std::cout << "[TRAVEL] Departing for " << destination->name << " (" << dist
            << " km)...\n";
  std::cout << "         Trip Duration: " << days << " Days.\n";
  std::cout << "         Paid $" << totalCost << ".\n";

  // 5. [SIMULATION LOOP] - The "Time Travel" Logic
  // We advance the world simulation for every day spent on the road.
  for (int i = 1; i <= days; ++i) {
    // Optional: Print a dot for every day to show progress
    if (i % 5 == 0)
      std::cout << ".";
    std::cout << std::flush;

    Simulation::advanceTime(world);
  }
  std::cout << "\n[ARRIVAL] You have reached " << destination->name << "!\n";

  return true;
}

bool Merchant::buyItem(int itemIndex, long long quantity) {
  if (itemIndex < 0 || itemIndex >= (int)currentLocation->currentPrices.size())
    return false;

  double unitPrice = currentLocation->currentPrices[itemIndex];
  double totalCost = unitPrice * quantity;

  if (quantity <= 0)
    return false;
  if (cash < totalCost) {
    std::cout << "[FAIL] Insufficient funds.\n";
    return false;
  }
  if (currentLocation->inventory[itemIndex] < quantity) {
    std::cout << "[FAIL] Not enough supply.\n";
    return false;
  }

  cash -= totalCost;
  inventory[itemIndex] += quantity;
  currentLocation->inventory[itemIndex] -= quantity;

  std::cout << "[TRADE] Bought " << quantity << " "
            << City::COMMODITIES[itemIndex].name << " for $" << totalCost
            << ".\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Note: We do NOT call updateEconomics here immediately,
  // because the Main Loop will call Simulation::advanceTime for us.
  return true;
}

bool Merchant::sellItem(int itemIndex, long long quantity) {
  if (itemIndex < 0 || itemIndex >= (int)currentLocation->currentPrices.size())
    return false;

  double marketPrice = currentLocation->currentPrices[itemIndex];
  double sellPrice = marketPrice * 0.90; // 10% Spread
  double totalRevenue = sellPrice * quantity;

  if (quantity <= 0)
    return false;
  if (inventory[itemIndex] < quantity) {
    std::cout << "[FAIL] Not enough items.\n";
    return false;
  }

  cash += totalRevenue;
  inventory[itemIndex] -= quantity;
  currentLocation->inventory[itemIndex] += quantity;

  std::cout << "[TRADE] Sold " << quantity << " "
            << City::COMMODITIES[itemIndex].name << " for $" << totalRevenue
            << ".\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  return true;
}