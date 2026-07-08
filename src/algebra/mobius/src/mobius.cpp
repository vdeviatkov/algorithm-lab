#include "algebra/mobius/mobius.h"

namespace algebra::mobius {

int mu(uint64_t n) {
    if (n == 0)
        return 0;

    int result = 1;
    for (uint64_t p = 2; p * p <= n; ++p) {
        if (n % p)
            continue;
        n /= p;
        if (n % p == 0)
            return 0; // squared prime factor
        result = -result;
    }
    if (n > 1)
        result = -result; // one leftover prime factor > sqrt(original n)
    return result;
}

std::vector<int8_t> mobius_up_to(std::size_t n) {
    std::vector<int8_t> mu_table(n + 1, 0);
    if (n >= 1)
        mu_table[1] = 1;

    // Linear sieve: every composite is crossed out exactly once, by its
    // smallest prime factor. μ propagates by the recurrences
    //   μ(p·i) = -μ(i)  if p ∤ i,   μ(p·i) = 0  if p | i.
    std::vector<uint32_t> primes;
    std::vector<uint8_t> composite(n + 1, 0);
    for (std::size_t i = 2; i <= n; ++i) {
        if (!composite[i]) {
            primes.push_back(static_cast<uint32_t>(i));
            mu_table[i] = -1;
        }
        for (uint32_t p : primes) {
            if (p * 1ull * i > n)
                break;
            composite[p * i] = 1;
            if (i % p == 0) {
                mu_table[p * i] = 0; // p² divides p·i
                break;
            }
            mu_table[p * i] = static_cast<int8_t>(-mu_table[i]);
        }
    }

    return mu_table;
}

} // namespace algebra::mobius
