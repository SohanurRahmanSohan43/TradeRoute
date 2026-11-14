#ifndef CONSOLEUTILS_H
#define CONSOLEUTILS_H

#include <iostream>
#include <string>

class Console {
public:
  // Standard Colors
  static const std::string RED;
  static const std::string GREEN;
  static const std::string YELLOW;
  static const std::string CYAN;
  static const std::string RESET;
  static const std::string BOLD;

  // Clear the terminal screen (Works on all modern OS)
  static void clear() {
    // \033[2J clears screen, \033[1;1H moves cursor to top-left
    std::cout << "\033[2J\033[1;1H";
  }

  // Print a separator line
  static void line() {
    std::cout
        << CYAN
        << "============================================================\n"
        << RESET;
  }

  // Print a centered header
  static void header(const std::string &title) {
    line();
    std::cout << BOLD << "   " << title << "\n" << RESET;
    line();
  }

  // Helper for error messages
  static void error(const std::string &msg) {
    std::cout << RED << " [!] " << msg << RESET << "\n";
  }

  // Helper for success messages
  static void success(const std::string &msg) {
    std::cout << GREEN << " [OK] " << msg << RESET << "\n";
  }
};

// Define the static strings
inline const std::string Console::RED = "\033[31m";
inline const std::string Console::GREEN = "\033[32m";
inline const std::string Console::YELLOW = "\033[33m";
inline const std::string Console::CYAN = "\033[36m";
inline const std::string Console::RESET = "\033[0m";
inline const std::string Console::BOLD = "\033[1m";

#endif