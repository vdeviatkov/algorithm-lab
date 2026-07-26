#include "algebra/totient/totient.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <numeric>
#include <random>
#include <vector>

namespace tot = algebra::totient;

TEST(Totient, KnownSmallValues) {
    const std::vector<uint64_t> expected = {
        0, 1, 1, 2, 2, 4, 2, 6, 4, 6, 4, 10, 4, 12, 6, 8, 8, 16, 6, 18, 8,
    };

    for (std::size_t n = 0; n < expected.size(); ++n)
        EXPECT_EQ(tot::phi(n), expected[n]) << "n=" << n;
}

TEST(Totient, BoundaryConventions) {
    EXPECT_EQ(tot::phi(0), 0);
    EXPECT_EQ(tot::phi(1), 1);

    const auto t0 = tot::totients_up_to(0);
    ASSERT_EQ(t0.size(), 1u);
    EXPECT_EQ(t0[0], 0);

    const auto t1 = tot::totients_up_to(1);
    ASSERT_EQ(t1.size(), 2u);
    EXPECT_EQ(t1[0], 0);
    EXPECT_EQ(t1[1], 1);
}

TEST(Totient, PrimesAndPrimePowers) {
    EXPECT_EQ(tot::phi(2), 1);
    EXPECT_EQ(tot::phi(97), 96);
    EXPECT_EQ(tot::phi(1'000'000'007ULL), 1'000'000'006ULL);

    EXPECT_EQ(tot::phi(8), 4);
    EXPECT_EQ(tot::phi(81), 54);
    EXPECT_EQ(tot::phi(1ULL << 63), 1ULL << 62);
}

TEST(Totient, CompositeValues) {
    EXPECT_EQ(tot::phi(36), 12);
    EXPECT_EQ(tot::phi(1'000), 400);
    EXPECT_EQ(tot::phi(2 * 3 * 5 * 7 * 11), 480);

    constexpr uint64_t p = 1'000'003;
    constexpr uint64_t q = 1'000'033;
    EXPECT_EQ(tot::phi(p * q), (p - 1) * (q - 1));
    EXPECT_EQ(tot::phi(p * p), p * (p - 1));
}

TEST(Totient, TableMatchesKnownValues) {
    const std::vector<uint64_t> expected = {
        0, 1, 1, 2, 2, 4, 2, 6, 4, 6, 4, 10, 4, 12, 6, 8, 8, 16, 6, 18, 8,
    };
    EXPECT_EQ(tot::totients_up_to(20), expected);
}

TEST(Totient, TableMatchesSingleValue) {
    constexpr std::size_t limit = 10'000;
    const auto table = tot::totients_up_to(limit);

    ASSERT_EQ(table.size(), limit + 1);
    for (std::size_t n = 0; n <= limit; ++n)
        EXPECT_EQ(table[n], tot::phi(n)) << "n=" << n;
}

TEST(Totient, CountsCoprimeIntegers) {
    const auto table = tot::totients_up_to(500);

    for (std::size_t n = 1; n < table.size(); ++n) {
        uint64_t count = 0;
        for (std::size_t x = 1; x <= n; ++x)
            count += std::gcd(x, n) == 1;

        EXPECT_EQ(table[n], count) << "n=" << n;
    }
}

TEST(Totient, DivisorSumIdentity) {
    constexpr std::size_t limit = 5'000;
    const auto table = tot::totients_up_to(limit);

    for (std::size_t n = 1; n <= limit; ++n) {
        uint64_t sum = 0;
        for (std::size_t d = 1; d <= n / d; ++d) {
            if (n % d != 0)
                continue;

            sum += table[d];
            if (d != n / d)
                sum += table[n / d];
        }

        EXPECT_EQ(sum, n) << "n=" << n;
    }
}

TEST(Totient, MultiplicativeForCoprimeArguments) {
    std::mt19937 rng(42u);
    std::uniform_int_distribution<uint64_t> distribution(1, 5'000);

    for (int trial = 0; trial < 500; ++trial) {
        const uint64_t a = distribution(rng);
        const uint64_t b = distribution(rng);
        if (std::gcd(a, b) != 1)
            continue;

        EXPECT_EQ(tot::phi(a * b), tot::phi(a) * tot::phi(b)) << "a=" << a << " b=" << b;
    }
}
