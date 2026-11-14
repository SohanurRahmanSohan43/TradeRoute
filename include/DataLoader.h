#ifndef DATALOADER_H
#define DATALOADER_H

#include "City.h"
#include <map>
#include <string>

class DataLoader {

public:
  // Reads a CSV file and populates the world map
  static void loadNetwork(const std::string &filename,
                          std::map<std::string, City *> &world);

  static void loadCommodities(const std::string &filename);

  static void loadEconomicRules(const std::string &filename);
  static void loadCityTypes(const std::string &filename,
                            std::map<std::string, City *> &world);

private:
  // Helper to find or create a city pointer safely
  static City *getCity(std::map<std::string, City *> &world,
                       const std::string &name);
};

#endif