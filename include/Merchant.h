#ifndef MERCHANT_H
#define MERCHANT_H

#include "City.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

class Merchant {
private:
  double cash;
  City *currentLocation;
  std::vector<long long> inventory;

public:
  Merchant(City *startCity, double startCash);

  // [UPDATED]: Now accepts 'world' to trigger simulation loops
  bool travelTo(City *destination, std::map<std::string, City *> &world);

  bool buyItem(int itemIndex, long long quantity);
  bool sellItem(int itemIndex, long long quantity);

  void printStatus();

  // Getters
  City *getLocation() { return currentLocation; }
  double getCash() { return cash; }
  const std::vector<long long> &getInventory() const { return inventory; }

  // Database Helpers
  void setCash(double amount) { cash = amount; }
  void setLocation(City *c) { currentLocation = c; }
  void setInventory(const std::vector<long long> &inv) { inventory = inv; }
};

#endif