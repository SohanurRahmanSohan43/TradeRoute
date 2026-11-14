#ifndef LINEARALGEBRA_H
#define LINEARALGEBRA_H

#include <cmath>
#include <iostream>
#include <vector>

class LinearAlgebra {
public:
  // [LINEAR ALGEBRA]: Element-wise Vector Addition (v1 + v2)
  static std::vector<double> add(const std::vector<double> &a,
                                 const std::vector<double> &b);

  // [LINEAR ALGEBRA]: Scalar Multiplication (v * scalar)
  static std::vector<double> scale(const std::vector<double> &v, double scalar);

  // [LINEAR ALGEBRA]: Dot Product (v1 . v2)
  // Used to calculate total value of assets (Price * Quantity)
  static double dotProduct(const std::vector<double> &prices,
                           const std::vector<int> &quantities);
};

#endif