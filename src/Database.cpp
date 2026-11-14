#include "Database.h"
#include <iostream>
#include <sstream> // <--- REQUIRED for string manipulation

Database::Database() : dbPtr(nullptr), isConnected(false) {}

Database::~Database() { close(); }

bool Database::init(const std::string &filename) {
  int rc = sqlite3_open(filename.c_str(), &dbPtr);
  if (rc) {
    std::cerr << "[DB ERROR] Can't open database: " << sqlite3_errmsg(dbPtr)
              << "\n";
    return false;
  } else {
    std::cout << "[SYSTEM] Database connection established.\n";
    isConnected = true;
    createTable();
    return true;
  }
}

void Database::close() {
  if (isConnected && dbPtr) {
    sqlite3_close(dbPtr);
    dbPtr = nullptr;
    isConnected = false;
    std::cout << "[SYSTEM] Database connection closed.\n";
  }
}

void Database::createTable() {
  const char *sql = "CREATE TABLE IF NOT EXISTS Player ("
                    "ID INT PRIMARY KEY, "
                    "Cash REAL, "
                    "LocationName TEXT, "
                    "Inventory TEXT);";

  char *errMsg = 0;
  sqlite3_exec(dbPtr, sql, 0, 0, &errMsg);
  if (errMsg) {
    std::cerr << "[DB INIT ERROR] " << errMsg << "\n";
    sqlite3_free(errMsg);
  }
}

void Database::resetGame() {
  const char *sql = "DELETE FROM Player;";
  sqlite3_exec(dbPtr, sql, 0, 0, 0);
  std::cout << "[SYSTEM] Save data wiped.\n";
}

void Database::saveGame(Merchant &player) {
  // 1. Serialize Inventory (Vector -> "10,5,20")
  std::stringstream ss;
  const auto &inv = player.getInventory();
  for (size_t i = 0; i < inv.size(); ++i) {
    ss << inv[i];
    if (i < inv.size() - 1)
      ss << ",";
  }
  std::string invStr = ss.str();

  // 2. Prepare SQL
  std::string sql = "INSERT OR REPLACE INTO Player (ID, Cash, LocationName, "
                    "Inventory) VALUES (1, ?, ?, ?);";

  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(dbPtr, sql.c_str(), -1, &stmt, 0);

  // 3. Bind Data
  sqlite3_bind_double(stmt, 1, player.getCash());
  sqlite3_bind_text(stmt, 2, player.getLocation()->name.c_str(), -1,
                    SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, invStr.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) == SQLITE_DONE) {
    std::cout << "[SYSTEM] Game Saved Successfully.\n";
  } else {
    std::cerr << "[DB ERROR] Save Failed.\n";
  }
  sqlite3_finalize(stmt);
}

bool Database::loadGame(Merchant &player,
                        std::map<std::string, City *> &world) {
  const char *sql =
      "SELECT Cash, LocationName, Inventory FROM Player WHERE ID = 1;";
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(dbPtr, sql, -1, &stmt, 0) != SQLITE_OK)
    return false;

  bool found = false;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    found = true;

    // 1. Load Cash
    double cash = sqlite3_column_double(stmt, 0);
    player.setCash(cash); // <--- Uses the new setter

    // 2. Load Location
    const char *locText =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    std::string locName = locText ? locText : "";

    if (world.count(locName)) {
      player.setLocation(world[locName]); // <--- Uses the new setter
    } else {
      std::cerr << "[WARNING] Saved location '" << locName
                << "' not found in map. Staying put.\n";
    }

    // 3. Load Inventory (Parse "10,5,20" -> Vector)
    const char *invText =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    std::string invStr = invText ? invText : "";

    std::vector<long long> loadedInv;
    std::stringstream ss(invStr);
    std::string segment;

    while (std::getline(ss, segment, ',')) {
      try {
        loadedInv.push_back(std::stoll(segment));
      } catch (...) {
        loadedInv.push_back(0); // Safe fallback
      }
    }
    player.setInventory(loadedInv); // <--- Uses the new setter
  }

  sqlite3_finalize(stmt);
  return found;
}