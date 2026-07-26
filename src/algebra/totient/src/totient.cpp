#include "algebra/totient/totient.h"

namespace algebra::totient {

uint64_t phi(uint64_t n) {
    if (n == 0)
        return 0;

    uint64_t result = n;

    // Remove each distinct prime factor p and apply result *= (1 - 1/p).
    // p <= n / p is equivalent to p*p <= n without multiplication overflow.
    for (uint64_t p = 2; p <= n / p; ++p) {
        if (n % p != 0)
            continue;

        while (n % p == 0)
            n /= p;
        result -= result / p;
    }

    // At most one prime factor greater than sqrt(original n) remains.
    if (n > 1)
        result -= result / n;

    return result;
}

std::vector<uint64_t> totients_up_to(std::size_t n) {
    std::vector<uint64_t> totients(n + 1, 0);
    if (n == 0)
        return totients;

    totients[1] = 1;

    // Linear sieve. Each composite m is generated once as p*i, where p is
    // its smallest prime factor.
    std::vector<std::size_t> primes;
    std::vector<uint8_t> composite(n + 1, 0);

    for (std::size_t i = 2; i <= n; ++i) {
        if (!composite[i]) {
            primes.push_back(i);
            totients[i] = i - 1;
        }

        for (std::size_t p : primes) {
            if (i > n / p)
                break;

            const std::size_t multiple = p * i;
            composite[multiple] = 1;

            if (i % p == 0) {
                totients[multiple] = totients[i] * p;
                break;
            }

            totients[multiple] = totients[i] * (p - 1);
        }
    }

    return totients;
}

} // namespace algebra::totient
