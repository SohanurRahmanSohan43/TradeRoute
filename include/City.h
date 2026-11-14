#ifndef CITY_H
#define CITY_H

#include "LinearAlgebra.h"
#include <iostream>
#include <map>
#include <string>
#include <valarray>
#include <vector>

// [LINEAR ALGEBRA]: We use a vector space R^n
// Index 0: Grain, Index 1: Iron, Index 2: Gold...

struct CommodityMeta {
  std::string name;
  double defaultBasePrice; // Loaded from CSV
  std::string unit;
  std::string category; // NEW: Tag-based system
};

class City {
public: // Public members are accessible from outside the class
  std::string name;
  std::string type; // NEW: "Agricultural", "Mining", etc.

  // [LINEAR ALGEBRA]: Econ State Vectors
  std::valarray<double> basePrices;
  std::valarray<double> currentPrices;
  std::valarray<long long> inventory; // Stores the amount of each commodity

  // [CALCULUS]: The Derivative Vector (dP/dt)
  // Stores the rate of change of prices per turn.
  std::valarray<double> priceDerivative;

  // [GRAPH THEORY]: Adjacency List Node
  struct Road {
    City *destination; // Stores the destinantion so every road is not copying a
                       // city object
    int distance;
  };
  std::vector<Road> roads;

  // [DATA DRIVEN]: The Global List of Items
  // Static because it is shared by ALL cities
  // Without static every city will store its own list of commodities
  static std::vector<CommodityMeta> COMMODITIES;

  // [DATA DRIVEN]: Economic Rules
  // Map <CityType, Map<Category, Multiplier>>
  static std::map<std::string, std::map<std::string, double>> economicRules;

  // Constructor
  City(std::string n);

  // Core Methods
  void addRoad(City *neighbor, int dist);
  void printInfo();

  // [CALCULUS REQUIREMENT]: Logic implemented in .cpp
  void updateEconomics();
  void recomputeBasePrices(); // NEW: Apply multipliers
};

#endif