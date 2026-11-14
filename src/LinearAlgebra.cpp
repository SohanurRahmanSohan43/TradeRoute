#include "LinearAlgebra.h"

std::vector<double> LinearAlgebra::add(const std::vector<double> &a,
                                       const std::vector<double> &b) {
  std::vector<double> result;
  if (a.size() != b.size()) {
    std::cerr << "[MATH ERROR] Vector dimension mismatch!\n";
    return result;
  }

  for (size_t i = 0; i < a.size(); i++) {
    result.push_back(a[i] + b[i]);
  }
  return result;
}

std::vector<double> LinearAlgebra::scale(const std::vector<double> &v,
                                         double scalar) {
  std::vector<double> result;
  for (double val : v) {
    result.push_back(val * scalar);
  }
  return result;
}

double LinearAlgebra::dotProduct(const std::vector<double> &prices,
                                 const std::vector<int> &quantities) {
  double total = 0.0;
  if (prices.size() != quantities.size())
    return 0.0;

  for (size_t i = 0; i < prices.size(); i++) {
    total += prices[i] * quantities[i];
  }
  return total;
}