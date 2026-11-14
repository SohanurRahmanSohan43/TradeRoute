#include "City.h"
#include "ConsoleUtils.h"
#include "DataLoader.h"
#include "Database.h"
#include "Merchant.h"
#include "Navigator.h"
#include "Simulation.h"
#include "StatsValidator.h"
#include "sqlite3.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <thread>

// Global World Map
std::map<std::string, City *> world;
Database dbManager;

double calculateNetWorth(Merchant &player) {
  double cash = player.getCash();
  double inventoryValue = 0;
  const auto &inv = player.getInventory();
  City *loc = player.getLocation();

  for (size_t i = 0; i < inv.size(); ++i) {
    if (inv[i] > 0) {
      inventoryValue += inv[i] * loc->currentPrices[i];
    }
  }
  return cash + inventoryValue;
}

void gameLoop(Merchant &player, double initialNetWorth) {
  // We want to modify player object, so we pass it by reference
  bool running = true;
  std::string lastMessage =
      "Welcome to Trade Route Simulator!"; // Feedback for the user

  while (running) {
    bool validActionTaken = false;

    // --- UX: DRAW DASHBOARD ---
    Console::clear();
    Console::header("TRADE ROUTE SIMULATOR");

    // 1. Print Feedback Area
    std::cout << Console::YELLOW << " >>> " << lastMessage << Console::RESET
              << "\n\n";

    // 2. Show Player Status
    std::cout << " PLAYER STATUS:\n";
    std::cout << " Cash: " << Console::GREEN << "$" << std::fixed
              << std::setprecision(2) << player.getCash() << Console::RESET
              << "\n";

    // Inventory Row
    std::cout << " Inventory: ";
    const auto &inv = player.getInventory();
    for (size_t i = 0; i < inv.size(); ++i) {
      if (inv[i] > 0) {
        std::cout << "[" << City::COMMODITIES[i].name << ": " << inv[i] << "] ";
      }
    }
    std::cout << "\n";
    Console::line();

    // 3. Show Local City Info
    player.getLocation()
        ->printInfo(); // You might want to remove '--- Name ---' from printInfo
                       // inside City.cpp to make it cleaner

    // 4. Menu
    std::cout << Console::BOLD << " ACTIONS:" << Console::RESET << "\n";
    std::cout << " [1] Buy Commodities\n";
    std::cout << " [2] Sell Commodities\n";
    std::cout << " [3] Travel (Direct)\n";
    std::cout << " [4] GPS Travel (Multi-hop)\n";
    std::cout << " [5] Quit & Save\n";
    std::cout << "\n > ";

    int choice;
    if (!(std::cin >> choice)) {
      lastMessage = "Invalid input. Please enter a number.";
      std::cin.clear();
      std::cin.ignore(10000, '\n');
      continue;
    }

    // --- HANDLE CHOICES ---
    if (choice == 1 || choice == 2) {
      // CLEAR SCREEN FOR TRADING MENU
      Console::clear();
      Console::header(choice == 1 ? "MARKETPLACE (BUY)" : "MARKETPLACE (SELL)");

      // Print Table Header
      // Print Table Header
      std::cout << std::left << std::setw(5) << "ID" << std::setw(15) << "Name"
                << std::setw(12) << "Price" << std::setw(10) << "Owned"
                << std::setw(10) << "Max Buy" << "\n";
      std::cout << "-----------------------------------------------------------"
                   "-----\n";

      for (size_t i = 0; i < City::COMMODITIES.size(); ++i) {
        double price = player.getLocation()->currentPrices[i];
        long long owned = player.getInventory()[i];
        long long maxBuy = (long long)(player.getCash() / price);

        std::cout << std::left << std::setw(5) << i << std::setw(15)
                  << City::COMMODITIES[i].name << Console::GREEN << "$"
                  << std::fixed << std::setprecision(2) << std::setw(11)
                  << price << Console::RESET << std::setw(10) << owned
                  << std::setw(10) << maxBuy << "\n";
      }

      int itemIdx;
      long long qty;
      std::cout << "\n Select Item ID: ";
      std::cin >> itemIdx;
      std::cout << " Quantity: ";
      std::cin >> qty;

      if (itemIdx < 0 || itemIdx >= City::COMMODITIES.size()) {
        lastMessage = "Transaction Failed: Invalid Item ID.";
      } else {
        if (choice == 1) {
          if (player.buyItem(itemIdx, qty)) {
            lastMessage = "Successfully bought " + std::to_string(qty) + " " +
                          City::COMMODITIES[itemIdx].name;
            validActionTaken = true;
          } else {
            lastMessage = "Purchase Failed (Insufficient Funds or Supply).";
          }
        } else {
          if (player.sellItem(itemIdx, qty)) {
            lastMessage = "Successfully sold " + std::to_string(qty) + " " +
                          City::COMMODITIES[itemIdx].name;
            validActionTaken = true;
          } else {
            lastMessage = "Sale Failed (Insufficient Inventory).";
          }
        }
      }

    } else if (choice == 3) {
      std::cout << " Enter Destination City Name: ";
      std::string destName;
      std::cin >> destName;

      if (world.count(destName)) {
        // [UPDATE] Pass 'world' here
        if (player.travelTo(world[destName], world)) {
          lastMessage = "Traveled to " + destName;

          // [IMPORTANT] Set this to FALSE.
          // Why? Because travelTo() already looped the simulation 'N' times
          // internally. We don't want to advance it one extra time at the end
          // of the loop.
          validActionTaken = false;
        } else {
          lastMessage = "Travel failed.";
        }
      } else {
        lastMessage = "City '" + destName + "' not found.";
      }

    } else if (choice == 4) {
      Console::clear();
      Console::header("GPS NAVIGATION SYSTEM");
      std::cout << " Enter Destination City Name: ";
      std::string destName;
      std::cin >> destName;

      if (world.count(destName)) {
        City *target = world[destName];
        std::vector<City *> route =
            Navigator::findShortestPath(player.getLocation(), target, world);

        if (!route.empty()) {
          Navigator::printRoute(route);
          std::cout << "\n Confirm Travel? (y/n): ";
          char confirm;
          std::cin >> confirm;

          if (confirm == 'y' || confirm == 'Y') {
            bool allStepsSuccessful = true;
            // Skip the first node as it's the current location
            for (size_t i = 1; i < route.size(); ++i) {
              if (!player.travelTo(route[i], world)) {
                lastMessage = "GPS Travel interrupted at " + route[i]->name;
                std::cout << "\n"
                          << Console::RED << " [!] " << lastMessage
                          << Console::RESET << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                allStepsSuccessful = false;
                break;
              }
            }
            if (allStepsSuccessful) {
              lastMessage = "Successfully reached " + destName;
            }
            validActionTaken = false; // Advanced time internally
          } else {
            lastMessage = "GPS Travel cancelled.";
          }
        } else {
          lastMessage = "No path found to " + destName;
        }
      } else {
        lastMessage = "City '" + destName + "' not found.";
      }

    } else if (choice == 5) {
      running = false;
      validActionTaken = false;

      Console::clear();
      Console::header("SHUTTING DOWN");
      std::cout << " [SYSTEM] Saving progress to database...\n";
      dbManager.saveGame(player);

      StatsValidator::printReport();

      // --- PROFIT REPORT ---
      double finalNetWorth = calculateNetWorth(player);
      double profit = finalNetWorth - initialNetWorth;

      std::cout << "\n===============================\n";
      std::cout << "      FINAL PROFIT REPORT      \n";
      std::cout << "===============================\n";
      std::cout << " Initial Worth: $" << initialNetWorth << "\n";
      std::cout << " Final Worth:   $" << finalNetWorth << "\n";
      std::cout << "-------------------------------\n";

      if (profit >= 0) {
        std::cout << " TOTAL PROFIT:  " << Console::GREEN << "+$" << profit
                  << Console::RESET << " (Success!)\n";
      } else {
        std::cout << " TOTAL LOSS:    " << Console::RED << "-$" << -profit
                  << Console::RESET << " (Try harder!)\n";
      }
      std::cout << "===============================\n\n";

      std::cout << " Press Enter to exit...";
      std::cin.ignore(10000, '\n');
      std::cin.get();

    } else {
      lastMessage = "Invalid selection.";
    }
  }
}

int main() {
  srand(time(nullptr));
  Console::clear();
  Console::header("SYSTEM INITIALIZATION");

  if (!dbManager.init("trade_route.db"))
    return 1;

  // LOAD DATA
  DataLoader::loadCommodities("data/commodities.csv");
  DataLoader::loadNetwork("data/network.csv", world);

  // NEW: Load Economic Data
  DataLoader::loadEconomicRules("data/economy_rules.csv");
  DataLoader::loadCityTypes("data/city_types.csv", world);

  if (world.empty() || City::COMMODITIES.empty()) {
    Console::error("Missing data files.");
    return 1;
  }

  // NEW: Apply Economic Logic
  std::cout << ">>> Applying Economic Rules...\n";
  for (auto const &[name, city] : world) {
    city->recomputeBasePrices();
  }

  // MAIN MENU
  Console::clear();
  Console::header("MAIN MENU");
  std::cout << " [1] NEW GAME\n";
  std::cout << " [2] LOAD GAME\n";
  std::cout << "\n > ";
  int menuChoice;
  std::cin >> menuChoice;

  Merchant *playerPtr = nullptr;

  if (menuChoice == 2) {
    playerPtr = new Merchant(world.begin()->second, 0);
    if (dbManager.loadGame(*playerPtr, world)) {
      Console::success("Save loaded successfully!");
      // Wait for user to read
      std::cout << " Press Enter to continue...";
      std::cin.ignore();
      std::cin.get();
    } else {
      Console::error("No save found. Starting New Game.");
      delete playerPtr;
      playerPtr = nullptr;
      menuChoice = 1;
    }
  }

  if (menuChoice == 1 || playerPtr == nullptr) {
    dbManager.resetGame();

    double startingCash;
    std::cout << " Enter Starting Capital: ";
    if (!(std::cin >> startingCash))
      startingCash = 1000.0;

    // CITY SELECTION
    Console::clear();
    Console::header("SELECT STARTING CITY");
    int count = 0;
    for (auto const &[name, city] : world) {
      std::cout << " [" << count++ << "] " << name;
      if (count % 4 == 0)
        std::cout << "\n";
    }

    std::string startCityName;
    City *startNode = nullptr;
    std::cout << "\n\n City Name > ";

    while (startNode == nullptr) {
      std::cin >> startCityName;
      if (world.count(startCityName)) {
        startNode = world[startCityName];
      } else {
        Console::error("City not found. Try again.");
        std::cout << " City Name > ";
      }
    }
    playerPtr = new Merchant(startNode, startingCash);
  }

  // Calculate Initial Net Worth for profit tracking
  double initialNetWorth = calculateNetWorth(*playerPtr);

  gameLoop(*playerPtr, initialNetWorth);
  delete playerPtr; // because it was stored in heap and hence dynamically
                    // alloted cause we used new.
  return 0;
}