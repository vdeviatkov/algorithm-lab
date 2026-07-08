#include "algebra/mobius/mobius.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <numeric>
#include <random>
#include <vector>

namespace mob = algebra::mobius;

// ==================== mu: known values ====================

TEST(Mobius, KnownSmallValues) {
    // OEIS A008683: μ(1..20)
    const std::vector<int> expected = {1,  -1, -1, 0, -1, 1,  -1, 0,  0, 1,
                                       -1, 0,  -1, 1, 1,  0,  -1, 0,  -1, 0};
    for (std::size_t n = 1; n <= expected.size(); ++n) {
        EXPECT_EQ(mob::mu(n), expected[n - 1]) << "n=" << n;
    }
}

TEST(Mobius, ZeroConvention) {
    EXPECT_EQ(mob::mu(0), 0);
    EXPECT_EQ(mob::mobius_up_to(5)[0], 0);
}

TEST(Mobius, SquarefreeProducts) {
    EXPECT_EQ(mob::mu(30), -1);       // 2·3·5
    EXPECT_EQ(mob::mu(210), 1);       // 2·3·5·7
    EXPECT_EQ(mob::mu(2310), -1);     // 2·3·5·7·11
    EXPECT_EQ(mob::mu(30030), 1);     // 2·3·5·7·11·13
}

TEST(Mobius, SquaredFactorsGiveZero) {
    EXPECT_EQ(mob::mu(4), 0);
    EXPECT_EQ(mob::mu(12), 0);
    EXPECT_EQ(mob::mu(9), 0);
    EXPECT_EQ(mob::mu(49), 0);
    EXPECT_EQ(mob::mu(360), 0);
    EXPECT_EQ(mob::mu(1024), 0);
}

TEST(Mobius, LargeValues) {
    EXPECT_EQ(mob::mu(999999937ULL), -1);              // prime
    EXPECT_EQ(mob::mu(999999937ULL * 2), 1);           // 2 · prime
    EXPECT_EQ(mob::mu(1000003ULL * 1000003ULL), 0);    // prime squared
    EXPECT_EQ(mob::mu(1000003ULL * 1000033ULL), 1);    // two distinct primes
}

// ==================== mobius_up_to: sieve table ====================

TEST(Mobius, TableEdgeCases) {
    auto t0 = mob::mobius_up_to(0);
    ASSERT_EQ(t0.size(), 1u);
    EXPECT_EQ(t0[0], 0);

    auto t1 = mob::mobius_up_to(1);
    ASSERT_EQ(t1.size(), 2u);
    EXPECT_EQ(t1[1], 1);
}

TEST(Mobius, TableMatchesSingleValue) {
    const std::size_t N = 10000;
    auto table = mob::mobius_up_to(N);
    ASSERT_EQ(table.size(), N + 1);
    for (std::size_t n = 1; n <= N; ++n) {
        EXPECT_EQ(static_cast<int>(table[n]), mob::mu(n)) << "n=" << n;
    }
}

TEST(Mobius, DivisorSumIdentity) {
    // Σ_{d|n} μ(d) = [n == 1]
    const std::size_t N = 5000;
    auto table = mob::mobius_up_to(N);
    for (std::size_t n = 1; n <= N; ++n) {
        int sum = 0;
        for (std::size_t d = 1; d * d <= n; ++d) {
            if (n % d) continue;
            sum += table[d];
            if (d != n / d) sum += table[n / d];
        }
        EXPECT_EQ(sum, n == 1 ? 1 : 0) << "n=" << n;
    }
}

TEST(Mobius, MertensFunction) {
    // M(n) = Σ_{k≤n} μ(k); classic values (OEIS A002321)
    auto table = mob::mobius_up_to(10000);
    int64_t m = 0;
    for (std::size_t n = 1; n <= 10000; ++n) {
        m += table[n];
        if (n == 10) EXPECT_EQ(m, -1);
        if (n == 100) EXPECT_EQ(m, 1);
        if (n == 1000) EXPECT_EQ(m, 2);
        if (n == 10000) EXPECT_EQ(m, -23);
    }
}

TEST(Mobius, Multiplicativity) {
    std::mt19937 rng(42u);
    std::uniform_int_distribution<uint64_t> dist(1, 3000);
    for (int trial = 0; trial < 500; ++trial) {
        uint64_t m = dist(rng), n = dist(rng);
        if (std::gcd(m, n) != 1) continue;
        EXPECT_EQ(mob::mu(m * n), mob::mu(m) * mob::mu(n))
            << "m=" << m << " n=" << n;
    }
}

// ==================== Transforms and inversion ====================

TEST(Mobius, TransformRoundTrip) {
    std::mt19937 rng(777u);
    std::uniform_int_distribution<int64_t> dist(-100, 100);

    std::vector<int64_t> f(501);
    for (auto& x : f) x = dist(rng);
    f[0] = 0;

    auto a = f;
    mob::divisor_sum_transform(a);
    mob::mobius_inversion(a);
    EXPECT_EQ(a, f);

    a = f;
    mob::mobius_inversion(a);
    mob::divisor_sum_transform(a);
    EXPECT_EQ(a, f);
}

TEST(Mobius, TransformMatchesBruteForce) {
    std::mt19937 rng(555u);
    std::uniform_int_distribution<int64_t> dist(-50, 50);

    const std::size_t N = 200;
    std::vector<int64_t> f(N + 1);
    for (auto& x : f) x = dist(rng);

    auto a = f;
    mob::divisor_sum_transform(a);
    for (std::size_t n = 1; n <= N; ++n) {
        int64_t expected = 0;
        for (std::size_t d = 1; d <= n; ++d)
            if (n % d == 0) expected += f[d];
        EXPECT_EQ(a[n], expected) << "n=" << n;
    }
}

TEST(Mobius, InversionMatchesExplicitFormula) {
    // f(n) = Σ_{d|n} μ(n/d) F(d), computed with the μ table directly.
    std::mt19937 rng(333u);
    std::uniform_int_distribution<int64_t> dist(-50, 50);

    const std::size_t N = 300;
    std::vector<int64_t> F(N + 1);
    for (auto& x : F) x = dist(rng);

    auto table = mob::mobius_up_to(N);
    auto a = F;
    mob::mobius_inversion(a);
    for (std::size_t n = 1; n <= N; ++n) {
        int64_t expected = 0;
        for (std::size_t d = 1; d <= n; ++d)
            if (n % d == 0) expected += static_cast<int64_t>(table[n / d]) * F[d];
        EXPECT_EQ(a[n], expected) << "n=" << n;
    }
}

TEST(Mobius, RecoverEulerTotient) {
    // n = Σ_{d|n} φ(d), so inverting F(n) = n yields φ(n).
    const std::size_t N = 500;
    std::vector<int64_t> a(N + 1);
    std::iota(a.begin(), a.end(), 0);
    mob::mobius_inversion(a);

    for (std::size_t n = 1; n <= N; ++n) {
        int64_t phi = 0;
        for (std::size_t x = 1; x <= n; ++x)
            phi += (std::gcd(x, n) == 1);
        EXPECT_EQ(a[n], phi) << "n=" << n;
    }
}

TEST(Mobius, EmptyAndTrivialTransforms) {
    std::vector<int64_t> empty;
    mob::divisor_sum_transform(empty);
    mob::mobius_inversion(empty);
    EXPECT_TRUE(empty.empty());

    std::vector<int64_t> one = {7};  // only index 0 → untouched
    mob::divisor_sum_transform(one);
    mob::mobius_inversion(one);
    EXPECT_EQ(one[0], 7);
}

// ==================== Applications ====================

TEST(Mobius, CoprimePairCounting) {
    // #{(a,b) : 1 ≤ a,b ≤ n, gcd(a,b) = 1} = Σ_{d=1}^{n} μ(d)·⌊n/d⌋²
    auto table = mob::mobius_up_to(200);
    for (std::size_t n = 1; n <= 200; n += 13) {
        int64_t brute = 0;
        for (std::size_t a = 1; a <= n; ++a)
            for (std::size_t b = 1; b <= n; ++b)
                brute += (std::gcd(a, b) == 1);

        int64_t formula = 0;
        for (std::size_t d = 1; d <= n; ++d) {
            const int64_t q = static_cast<int64_t>(n / d);
            formula += static_cast<int64_t>(table[d]) * q * q;
        }
        EXPECT_EQ(formula, brute) << "n=" << n;
    }
}

TEST(Mobius, SquarefreeCounting) {
    // Q(n) = Σ_{d² ≤ n} μ(d)·⌊n/d²⌋ counts squarefree numbers ≤ n.
    const std::size_t N = 3000;
    auto table = mob::mobius_up_to(N);

    int64_t brute = 0;
    for (std::size_t n = 1; n <= N; ++n) {
        brute += (table[n] != 0); // squarefree ⟺ μ(n) ≠ 0
        int64_t formula = 0;
        for (std::size_t d = 1; d * d <= n; ++d)
            formula += static_cast<int64_t>(table[d]) * static_cast<int64_t>(n / (d * d));
        ASSERT_EQ(formula, brute) << "n=" << n;
    }
}
