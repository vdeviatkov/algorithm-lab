#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace algebra::mobius {

// Möbius function μ(n):
//   μ(1) = 1
//   μ(n) = 0        if n has a squared prime factor
//   μ(n) = (-1)^k   if n is a product of k distinct primes
// Convention: μ(0) = 0.

// Single value via trial division. O(sqrt(n)).
int mu(uint64_t n);

// Table of μ(0..n) via a linear sieve. O(n) time, O(n) memory.
// Result size is n+1; mobius[0] = 0.
std::vector<int8_t> mobius_up_to(std::size_t n);

// ---- Divisor-sum (Dirichlet) transform and Möbius inversion ----
//
// Both operate in place on a[0..N] where a[n] is a value attached to the
// integer n; index 0 is ignored. See proof.md for correctness.

// Forward transform: a[n] <- F(n) = Σ_{d | n} a(d).   O(N log N)
//
// Iterates d from high to low so that when divisor d contributes to its
// multiples, a[d] still holds the original f(d).
template <typename T>
void divisor_sum_transform(std::vector<T>& a) {
    const std::size_t n = a.empty() ? 0 : a.size() - 1;
    for (std::size_t d = n; d >= 1; --d) {
        for (std::size_t m = 2 * d; m <= n; m += d) {
            a[m] += a[d];
        }
    }
}

// Inverse transform (Möbius inversion): given a[n] = F(n) = Σ_{d | n} f(d),
// recover a[n] <- f(n) = Σ_{d | n} μ(n/d) F(d).   O(N log N)
//
// Iterates d from low to high: by the time d is reached, a[d] has already
// been reduced to f(d), which is then subtracted from every proper multiple.
template <typename T>
void mobius_inversion(std::vector<T>& a) {
    const std::size_t n = a.empty() ? 0 : a.size() - 1;
    for (std::size_t d = 1; d <= n; ++d) {
        for (std::size_t m = 2 * d; m <= n; m += d) {
            a[m] -= a[d];
        }
    }
}

} // namespace algebra::mobius
