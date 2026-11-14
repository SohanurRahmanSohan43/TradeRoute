#include "Simulation.h"
#include "ConsoleUtils.h"
#include "StatsValidator.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

// Initialize static member
std::mt19937 Simulation::rng(std::random_device{}());

void Simulation::advanceTime(std::map<std::string, City *> &world) {
  // 1. Update every single city in the network
  for (auto const &[name, city] : world) {
    // People eat food / use iron every turn
    consumeResources(city);

    // Recalculate prices based on new inventory levels
    city->updateEconomics();
  }

  // 2. Trigger Random Events (Probability)
  // Probabilities:
  // 0-839 (84%): No Event
  // 840-989 (15%): Local Event
  // 990-999 (1%): Global Event
  //    990-997 (0.8%): Tech Boom
  //    998-999 (0.2%): War

  std::uniform_int_distribution<int> dist(0, 999);
  int roll = dist(rng);

  if (roll >= 990) { // 1% chance of GLOBAL event
    // Global Event Sub-roll
    // 990-997: Tech Boom
    // 998-999: War

    bool isWar = (roll >= 998);

    std::cout << "\n"
              << Console::RED << " [GLOBAL NEWS SHOCK] " << Console::RESET;

    if (isWar) {
      std::cout
          << "WAR declared! Iron and Steel prices skyrocket everywhere!\n";
      for (auto const &[name, city] : world) {
        if (city->currentPrices.size() > 3) {
          city->currentPrices[2] *= 1.5; // Iron
          city->currentPrices[3] *= 1.4; // Steel
        }
      }
      StatsValidator::logEvent(StatsValidator::EVENT_GLOBAL_WAR);
    } else {
      std::cout << "Tech Boom! Gold and Electronics prices surge!\n";
      for (auto const &[name, city] : world) {
        if (city->currentPrices.size() > 9) {
          city->currentPrices[8] *= 1.2; // Gold
          city->currentPrices[9] *= 1.3; // Electronics
        }
      }
      StatsValidator::logEvent(StatsValidator::EVENT_GLOBAL_BOOM);
    }
    std::cout << " (Global market adjusted)\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  } else if (roll >= 840) { // 15% chance of LOCAL event
    triggerRandomEvent(world);
  } else {
    // Log that nothing happened
    StatsValidator::logEvent(StatsValidator::EVENT_NONE);
  }
}

void Simulation::consumeResources(City *c) {
  // [ECONOMICS]: Constant Demand Simulation
  // Every turn, a city consumes 1-2 units of Grain and 0-1 units of Iron
  // This forces prices to rise naturally over time (Inflation/Scarcity)

  std::uniform_int_distribution<int> chance(0, 1);
  if (c->inventory[0] > 0)
    c->inventory[0] -= (chance(rng)) + 1; // Grain consumption always happen
  if (c->inventory[1] > 0)
    c->inventory[1] -= (chance(rng)); // Iron consumption sometimes happens

  // Ensure we don't go negative
  if (c->inventory[0] < 0)
    c->inventory[0] = 0;
  if (c->inventory[1] < 0)
    c->inventory[1] = 0;
}

void Simulation::triggerRandomEvent(std::map<std::string, City *> &world) {
  // Pick a random city
  std::uniform_int_distribution<int> cityDist(0, world.size() - 1);
  auto it = world.begin();
  std::advance(it, cityDist(rng));
  City *luckyCity = it->second;

  std::uniform_int_distribution<int> eventDist(0, 2);
  int eventType = eventDist(rng);

  std::cout << "\n>>> [BREAKING NEWS] ";

  switch (eventType) {
  case 0: // Bumper Crop (Grain Supply ++, Price --)
    std::cout << "Massive harvest in " << luckyCity->name
              << "! Grain is everywhere!\n";
    luckyCity->inventory[0] += 50;
    break;
  case 1: // Gold Rush (Gold Supply ++, Price --)
    std::cout << "New mine discovered in " << luckyCity->name
              << "! Gold prices crashing!\n";
    luckyCity->inventory[2] += 20;
    break;
  case 2: // Industrial Boom (Iron Demand ++, Price ++)
    std::cout << "Construction boom in " << luckyCity->name
              << "! Iron stock depleted!\n";
    luckyCity->inventory[1] = 0; // Supply shock!
    break;
  }
  std::cout << "\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  switch (eventType) {
  case 0:
    StatsValidator::logEvent(StatsValidator::EVENT_BUMPER_CROP);
    break;
  case 1:
    StatsValidator::logEvent(StatsValidator::EVENT_GOLD_RUSH);
    break;
  case 2:
    StatsValidator::logEvent(StatsValidator::EVENT_BOOM);
    break;
  }
}