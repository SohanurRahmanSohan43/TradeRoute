#include "DataLoader.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

void DataLoader::loadCommodities(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "[CRITICAL] Failed to open " << filename << "\n";
    exit(1);
  }

  std::string line;
  // Skip Header
  std::getline(file, line);

  std::cout << ">>> Loading Economy from: " << filename << "...\n";

  // Clear any existing defaults
  City::COMMODITIES.clear();

  while (std::getline(file, line)) {
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::string name, priceStr, unit, category;

    std::getline(ss, name, ',');
    std::getline(ss, priceStr, ',');
    std::getline(ss, unit, ',');
    std::getline(ss, category, ',');

    // Clean up strings
    if (!unit.empty() && unit.back() == '\r')
      unit.pop_back();
    if (!category.empty() && category.back() == '\r')
      category.pop_back();

    // Remove any generic whitespace
    // (Simple trim matching common issues)
    if (!category.empty()) {
      size_t first = category.find_first_not_of(" \t\r\n");
      if (std::string::npos == first)
        category = "";
      else {
        size_t last = category.find_last_not_of(" \t\r\n");
        category = category.substr(first, (last - first + 1));
      }
    }

    // Default Category if missing
    if (category.empty()) {
      std::cerr << "   [WARNING] Item '" << name
                << "' is missing a category. Defaulting to 'General'.\n";
      category = "General";
    }

    try {
      double price = std::stod(priceStr);

      CommodityMeta meta = {name, price, unit, category};

      City::COMMODITIES.push_back(meta);
      std::cout << "   + Defined Item: " << name << " [" << category << "] ($"
                << price << "/" << unit << ")\n";
    } catch (...) {
      std::cerr << "[ERROR] Bad commodity data: " << line << "\n";
    }
  }
}

void DataLoader::loadEconomicRules(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "[CRITICAL] Failed to open " << filename << "\n";
    exit(1);
  }

  std::cout << ">>> Loading Economic Rules...\n";
  std::string line;
  std::getline(file, line); // Skip Header

  while (std::getline(file, line)) {
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::string cityType, category, multStr;

    std::getline(ss, cityType, ',');
    std::getline(ss, category, ',');
    std::getline(ss, multStr, ',');

    // Clean
    if (!multStr.empty() && multStr.back() == '\r')
      multStr.pop_back();

    try {
      double multiplier = std::stod(multStr);
      City::economicRules[cityType][category] = multiplier;
      // std::cout << "   Rule: " << cityType << " cities buy " << category << "
      // at x" << multiplier << "\n";
    } catch (...) {
      std::cerr << "[ERROR] Bad rule data: " << line << "\n";
    }
  }
}

void DataLoader::loadCityTypes(const std::string &filename,
                               std::map<std::string, City *> &world) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "[CRITICAL] Failed to open " << filename << "\n";
    exit(1);
  }

  std::cout << ">>> Loading City Types...\n";
  std::string line;
  std::getline(file, line); // Skip Header

  while (std::getline(file, line)) {
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::string cityName, type;

    std::getline(ss, cityName, ',');
    std::getline(ss, type, ',');

    // Clean
    if (!type.empty() && type.back() == '\r')
      type.pop_back();

    // Assign type if city exists
    City *c = getCity(world, cityName);
    c->type = type;
    // std::cout << "   > " << cityName << " is now " << type << "\n";
  }
}

void DataLoader::loadNetwork(const std::string &filename,
                             std::map<std::string, City *> &world) {

  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "[CRITICAL] Failed to open file: " << filename << "\n";
    exit(1);
  }

  std::string line;
  int lineNum = 0;

  std::cout << ">>> Parsing Data Source: " << filename << "...\n";

  if (std::getline(file, line)) {
    lineNum++;
  }

  while (std::getline(file, line)) {
    lineNum++;
    if (line.empty())
      continue;

    std::stringstream ss(line);
    std::string cityA_Name, cityB_Name, distStr;

    std::getline(ss, cityA_Name, ',');
    std::getline(ss, cityB_Name, ',');
    std::getline(ss, distStr, ',');

    if (cityA_Name.empty() || cityB_Name.empty() || distStr.empty()) {
      std::cerr << "   [WARNING] Line " << lineNum
                << ": Missing values. Skipped.\n";
      continue;
    }

    try {
      int dist = std::stoi(distStr);
      City *nodeA = getCity(world, cityA_Name);
      City *nodeB = getCity(world, cityB_Name);

      nodeA->addRoad(nodeB, dist);
      nodeB->addRoad(nodeA, dist);

    } catch (...) {
      std::cerr << "   [WARNING] Line " << lineNum
                << ": Invalid Distance (NaN). Skipped.\n";
    }
  }
  std::cout << ">>> Data Load Complete. Total Cities: " << world.size() << "\n";
}

City *DataLoader::getCity(std::map<std::string, City *> &world,
                          const std::string &name) {
  if (world.find(name) == world.end()) {
    world[name] = new City(name);
  }
  return world[name];
}