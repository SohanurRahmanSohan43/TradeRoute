#ifndef DATABASE_H
#define DATABASE_H

#include "Merchant.h"
#include "sqlite3.h"
#include <iostream>
#include <map>
#include <string>

class Database {
private:
  sqlite3 *dbPtr; // The raw C pointer to the DB engine
  bool isConnected;

public:
  Database();
  ~Database(); // Destructor handles cleanup automatically

  // Connect to the file
  bool init(const std::string &filename);

  // We will add executeQuery() here in Phase 5
  void close();

  void createTable();
  void saveGame(Merchant &player);
  bool loadGame(Merchant &player, std::map<std::string, City *> &world);
  void resetGame();
};

#endif