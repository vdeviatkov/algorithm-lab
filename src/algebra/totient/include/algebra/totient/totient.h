#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace algebra::totient {

// Euler's totient function φ(n): the number of integers x in [1, n] for which
// gcd(x, n) = 1. By convention, φ(0) = 0 and φ(1) = 1.
//
// Computes one value by trial division in O(sqrt(n)) time and O(1) memory.
uint64_t phi(uint64_t n);

// Return φ(0), φ(1), ..., φ(n) using a linear sieve.
// Result size is n + 1; totients[0] = 0 and totients[1] = 1 when present.
// Complexity: O(n) time and O(n) memory.
std::vector<uint64_t> totients_up_to(std::size_t n);

} // namespace algebra::totient
