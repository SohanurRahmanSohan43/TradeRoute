#include "City.h"
#include "LinearAlgebra.h"
#include <algorithm> // For std::max
#include <cstdlib>   // For rand()
#include <iomanip>   // For std::setprecision

// 1. Define the static global list
std::vector<CommodityMeta> City::COMMODITIES;
// 2. Define the static economic rules
std::map<std::string, std::map<std::string, double>> City::economicRules;

// Constructor Implementation
// Member initialization list
City::City(std::string n) : name(n), type("Normal") {

  // [DYNAMIC INITIALIZATION]
  size_t numItems = COMMODITIES.size();
  basePrices.resize(numItems);
  currentPrices.resize(numItems);
  inventory.resize(numItems);
  priceDerivative.resize(numItems, 0.0);

  for (size_t i = 0; i < numItems; ++i) {
    basePrices[i] = COMMODITIES[i].defaultBasePrice;

    // [PROBABILITY]: Give random starting inventory (5 to 55 units)
    inventory[i] = (rand() % 50) + 5;
  }

  // Set current prices equal to base initially
  currentPrices = basePrices;
}

void City::recomputeBasePrices() {
  for (size_t i = 0; i < COMMODITIES.size(); ++i) {
    double multiplier = 1.0;
    std::string category = COMMODITIES[i].category;

    // Check if rules exist for this city type AND this category
    if (economicRules.count(this->type) &&
        economicRules[this->type].count(category)) {
      multiplier = economicRules[this->type][category];
    }

    // Apply Multiplier to Base Price
    // Base Price is what everything tends towards.
    // A "Mining" city has a lower base price for "Mineral" items.
    // basePrices[i] = Default * Multiplier
    basePrices[i] = COMMODITIES[i].defaultBasePrice * multiplier;

    // Add Organic Noise (+/- 5%)
    // This makes every city slightly different even if they are the same type
    double noise = ((rand() % 10) - 5) / 100.0;
    basePrices[i] *= (1.0 + noise);
  }
  // Update current prices to reflect the new base immediately (before
  // simulation starts)
  currentPrices = basePrices;
}

// Graph Theory: Add Edge
void City::addRoad(City *neighbor, int dist) {
  roads.push_back({neighbor, dist});
}

// TUI Helper: Print Status
void City::printInfo() {
  std::cout << "--- " << name << " (" << type << ") ---\n";

  // Linear Algebra: Print the Vector
  std::cout << "  [Market Status]:\n";
  std::cout << std::fixed << std::setprecision(2); // Format to 2 decimal places

  // [DYNAMIC PRINTING]
  // Loops through the global commodity list
  for (size_t i = 0; i < COMMODITIES.size(); ++i) {
    std::cout << "    " << COMMODITIES[i].name << ": $" << currentPrices[i]
              << " (Qty: " << inventory[i] << " " << COMMODITIES[i].unit
              << ")\n";
  }

  // Graph Theory: Print Connections
  std::cout << "  [Connections]:\n";
  if (roads.empty()) {
    std::cout << "    (Isolated Node)\n";
  } else {
    for (auto &r : roads) {
      std::cout << "    -> " << r.destination->name << " (" << r.distance
                << " km)\n";
    }
  }
  std::cout << "\n";
}

// [CALCULUS LOGIC]
// Calculates the rate of change (derivative) of price based on Scarcity
void City::updateEconomics() {
  // 1. Save state at time (t-1)
  std::valarray<double> oldPrices = currentPrices;

  // 2. Calculate new prices at time (t) based on Supply
  std::valarray<double> supply(inventory.size());
  for (size_t i = 0; i < inventory.size(); ++i) {
    supply[i] = static_cast<double>(std::max(0LL, inventory[i]));
  }

  // Vectorized Math:
  // Scarcity = 15.0 / (supply + 5.0)
  // Price = Base * (1.0 + Scarcity)
  std::valarray<double> scarcityFactor = 15.0 / (supply + 5.0);
  currentPrices = basePrices * (1.0 + scarcityFactor);

  // 3. [CALCULUS]: Calculate Rate of Change (Derivative)
  // dP/dt = Price(t) - Price(t-1)
  // vectorized subtraction
  priceDerivative = currentPrices - oldPrices;
}