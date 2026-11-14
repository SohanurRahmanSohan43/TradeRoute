#include "StatsValidator.h"

// Initialize static members
std::map<int, int> StatsValidator::eventCounts;
int StatsValidator::totalObservations = 0;

void StatsValidator::logEvent(EventType e) {
  eventCounts[e]++;
  totalObservations++;
}

void StatsValidator::printReport() {
  std::cout << "\n============================================\n";
  std::cout << "   STATISTICAL VALIDATION (CHI-SQUARE TEST) \n";
  std::cout << "============================================\n";

  if (totalObservations < 10) {
    std::cout << "[WARNING] Sample size too small for valid stats (n="
              << totalObservations << ").\n";
    std::cout << "          Play longer to generate more data.\n";
    return;
  }

  // 1. Define Hypotheses
  // Null Hypothesis (H0): Events are distributed according to design
  // None: 84%, Crops: 5%, Gold: 5%, Boom: 5%, G.Boom: 0.8%, G.War: 0.2%

  double expectedProbs[] = {0.84, 0.05, 0.05, 0.05, 0.008, 0.002};
  std::string labels[] = {"No Event",  "Bumper Crop", "Gold Rush",
                          "Ind. Boom", "Tech Boom",   "Global War"};

  double chiSquareVal = 0.0;

  std::cout << "Category\tObserved(O)\tExpected(E)\t(O-E)^2 / E\n";
  std::cout << "------------------------------------------------------------\n";

  for (int i = 0; i < 6; i++) {
    int observed = eventCounts[i]; // Defaults to 0 if empty
    double expected = totalObservations * expectedProbs[i];

    // Avoid division by zero
    if (expected == 0)
      expected = 0.001;

    double term = std::pow(observed - expected, 2) / expected;
    chiSquareVal += term;

    std::cout << labels[i] << "\t" << observed << "\t\t" << expected << "\t\t"
              << term << "\n";
  }

  std::cout << "------------------------------------------------------------\n";
  std::cout << "CALCULATED CHI-SQUARE: " << chiSquareVal << "\n";

  // Critical Value for 5 Degrees of Freedom (6 categories - 1) at p=0.05
  // is 11.070
  double criticalValue = 11.070;

  if (chiSquareVal < criticalValue) {
    std::cout << "RESULT: [PASSED] H0 Accepted.\n";
    std::cout << "        The Random Number Generator is FAIR (Unbiased).\n";
  } else {
    std::cout << "RESULT: [FAILED] H0 Rejected.\n";
    std::cout << "        Significant deviation detected. RNG may be biased.\n";
  }
  std::cout << "============================================\n";
}