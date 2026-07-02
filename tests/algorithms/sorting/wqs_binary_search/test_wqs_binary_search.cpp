#include <algorithms/sorting/wqs_binary_search/wqs_binary_search.h>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <gtest/gtest.h>
#include <limits>
#include <random>
#include <utility>
#include <vector>

namespace wqs = algorithms::sorting::wqs_binary_search;

// ==================== Helpers: exactly-k selection ====================
//
// "Pick exactly k of n values to maximize (or minimize) their sum" is the
// simplest WQS-shaped problem: h(k) = sum of top-k (bottom-k) values, which is
// concave (convex) in k because sorted increments are monotone.

// Oracle for maximization: maximizes sum(S) − λ·|S| by taking every value with
// v − λ ≥ 0. Including v == λ items implements the "prefer larger count"
// tie-break required by the API.
static auto make_pick_max_oracle(const std::vector<int64_t>& vals) {
    return [&vals](int64_t lambda) -> std::pair<int64_t, int64_t> {
        int64_t raw = 0, cnt = 0;
        for (int64_t v : vals) {
            if (v >= lambda) {
                raw += v;
                ++cnt;
            }
        }
        return {raw, cnt};
    };
}

// Oracle for minimization: minimizes sum(S) + λ·|S| by taking every value with
// v + λ ≤ 0 (ties included → larger count).
static auto make_pick_min_oracle(const std::vector<int64_t>& vals) {
    return [&vals](int64_t lambda) -> std::pair<int64_t, int64_t> {
        int64_t raw = 0, cnt = 0;
        for (int64_t v : vals) {
            if (v + lambda <= 0) {
                raw += v;
                ++cnt;
            }
        }
        return {raw, cnt};
    };
}

static int64_t top_k_sum(std::vector<int64_t> vals, int64_t k) {
    std::sort(vals.begin(), vals.end(), std::greater<>());
    int64_t s = 0;
    for (int64_t i = 0; i < k; ++i) s += vals[i];
    return s;
}

static int64_t bottom_k_sum(std::vector<int64_t> vals, int64_t k) {
    std::sort(vals.begin(), vals.end());
    int64_t s = 0;
    for (int64_t i = 0; i < k; ++i) s += vals[i];
    return s;
}

// ==================== Helpers: array partition DP ====================
//
// Split an array of nonnegative values into exactly k contiguous segments,
// minimizing the sum of (segment sum)². The cost satisfies the quadrangle
// (Monge) inequality, so h(k) is convex — the classic WQS use case where the
// λ-penalized DP is O(n²) instead of O(n²·k).

// Penalized oracle: min over all partitions of Σ(segment sum)² + λ·(#segments).
// Returns (raw cost, #segments), preferring more segments on ties.
static std::pair<int64_t, int64_t>
penalized_partition(const std::vector<int64_t>& a, int64_t lambda) {
    const int64_t n = static_cast<int64_t>(a.size());
    std::vector<int64_t> pref(n + 1, 0);
    for (int64_t i = 0; i < n; ++i) pref[i + 1] = pref[i] + a[i];

    constexpr int64_t INF = std::numeric_limits<int64_t>::max() / 4;
    std::vector<int64_t> dp(n + 1, INF), cnt(n + 1, 0);
    dp[0] = 0;
    for (int64_t j = 1; j <= n; ++j) {
        for (int64_t i = 0; i < j; ++i) {
            const int64_t seg = pref[j] - pref[i];
            const int64_t c = dp[i] + seg * seg + lambda;
            if (c < dp[j] || (c == dp[j] && cnt[i] + 1 > cnt[j])) {
                dp[j] = c;
                cnt[j] = cnt[i] + 1;
            }
        }
    }
    return {dp[n] - lambda * cnt[n], cnt[n]};
}

// Reference: exact O(n²·k) DP over (position, segments used).
static int64_t exact_partition(const std::vector<int64_t>& a, int64_t k) {
    const int64_t n = static_cast<int64_t>(a.size());
    std::vector<int64_t> pref(n + 1, 0);
    for (int64_t i = 0; i < n; ++i) pref[i + 1] = pref[i] + a[i];

    constexpr int64_t INF = std::numeric_limits<int64_t>::max() / 4;
    std::vector<std::vector<int64_t>> dp(k + 1, std::vector<int64_t>(n + 1, INF));
    dp[0][0] = 0;
    for (int64_t m = 1; m <= k; ++m) {
        for (int64_t j = 1; j <= n; ++j) {
            for (int64_t i = 0; i < j; ++i) {
                if (dp[m - 1][i] == INF) continue;
                const int64_t seg = pref[j] - pref[i];
                dp[m][j] = std::min(dp[m][j], dp[m - 1][i] + seg * seg);
            }
        }
    }
    return dp[k][n];
}

// ==================== maximize_integer ====================

TEST(WqsBinarySearch, MaximizeTopKDistinctValues) {
    std::vector<int64_t> vals = {5, 1, 9, 3, 7};
    for (int64_t k = 1; k <= 5; ++k) {
        auto res = wqs::maximize_integer(make_pick_max_oracle(vals), k, 0, 10);
        EXPECT_EQ(res.value, top_k_sum(vals, k)) << "k=" << k;
    }
}

TEST(WqsBinarySearch, MaximizeAllEqualValues) {
    // Duplicates make the count jump past k at a single λ; the recovery
    // formula h(k) = raw + λ(k − cnt) must still be exact.
    std::vector<int64_t> vals(6, 5);
    for (int64_t k = 1; k <= 6; ++k) {
        auto res = wqs::maximize_integer(make_pick_max_oracle(vals), k, 0, 6);
        EXPECT_EQ(res.value, 5 * k) << "k=" << k;
        EXPECT_EQ(res.count, 6);  // oracle tie-breaks to the larger count
    }
}

TEST(WqsBinarySearch, MaximizeWithNegativeValues) {
    std::vector<int64_t> vals = {-4, 6, -1, 3, -7, 2};
    for (int64_t k = 1; k <= static_cast<int64_t>(vals.size()); ++k) {
        auto res = wqs::maximize_integer(make_pick_max_oracle(vals), k, -8, 7);
        EXPECT_EQ(res.value, top_k_sum(vals, k)) << "k=" << k;
    }
}

TEST(WqsBinarySearch, MaximizeSingleElement) {
    std::vector<int64_t> vals = {42};
    auto res = wqs::maximize_integer(make_pick_max_oracle(vals), 1, 0, 43);
    EXPECT_EQ(res.value, 42);
    EXPECT_EQ(res.count, 1);
}

TEST(WqsBinarySearch, MaximizeReportsBracketedLambda) {
    std::vector<int64_t> vals = {5, 1, 9, 3, 7};
    auto res = wqs::maximize_integer(make_pick_max_oracle(vals), 2, 0, 10);
    EXPECT_GE(res.lambda, 0);
    EXPECT_LE(res.lambda, 10);
    EXPECT_GE(res.count, 2);  // tie-break convention: count ≥ k at chosen λ
}

// ==================== minimize_integer ====================

TEST(WqsBinarySearch, MinimizeBottomKDistinctValues) {
    std::vector<int64_t> vals = {5, 1, 9, 3, 7};
    for (int64_t k = 1; k <= 5; ++k) {
        auto res = wqs::minimize_integer(make_pick_min_oracle(vals), k, -10, 0);
        EXPECT_EQ(res.value, bottom_k_sum(vals, k)) << "k=" << k;
    }
}

TEST(WqsBinarySearch, MinimizeWithNegativeValues) {
    std::vector<int64_t> vals = {-4, 6, -1, 3, -7, 2};
    for (int64_t k = 1; k <= static_cast<int64_t>(vals.size()); ++k) {
        auto res = wqs::minimize_integer(make_pick_min_oracle(vals), k, -7, 8);
        EXPECT_EQ(res.value, bottom_k_sum(vals, k)) << "k=" << k;
    }
}

TEST(WqsBinarySearch, MinimizeAllEqualValues) {
    std::vector<int64_t> vals(4, -3);
    for (int64_t k = 1; k <= 4; ++k) {
        auto res = wqs::minimize_integer(make_pick_min_oracle(vals), k, 0, 4);
        EXPECT_EQ(res.value, -3 * k) << "k=" << k;
    }
}

// ==================== Partition DP (the canonical use case) ====================

TEST(WqsBinarySearch, PartitionSumOfSquaresSmall) {
    // [3, 1, 4, 1, 5] into k segments minimizing Σ(segment sum)².
    std::vector<int64_t> a = {3, 1, 4, 1, 5};
    auto oracle = [&a](int64_t lambda) { return penalized_partition(a, lambda); };
    for (int64_t k = 1; k <= static_cast<int64_t>(a.size()); ++k) {
        auto res = wqs::minimize_integer(oracle, k, -1'000'000, 1'000'000);
        EXPECT_EQ(res.value, exact_partition(a, k)) << "k=" << k;
    }
}

TEST(WqsBinarySearch, PartitionWithZeros) {
    // Zero elements create ties between partitions with different counts,
    // stressing the tie-break rule.
    std::vector<int64_t> a = {0, 2, 0, 0, 3, 0};
    auto oracle = [&a](int64_t lambda) { return penalized_partition(a, lambda); };
    for (int64_t k = 1; k <= static_cast<int64_t>(a.size()); ++k) {
        auto res = wqs::minimize_integer(oracle, k, -1'000'000, 1'000'000);
        EXPECT_EQ(res.value, exact_partition(a, k)) << "k=" << k;
    }
}

TEST(WqsBinarySearch, PartitionUniformArray) {
    // Uniform values maximize degeneracy: h(k) has long collinear stretches.
    std::vector<int64_t> a(10, 4);
    auto oracle = [&a](int64_t lambda) { return penalized_partition(a, lambda); };
    for (int64_t k = 1; k <= 10; ++k) {
        auto res = wqs::minimize_integer(oracle, k, -1'000'000, 1'000'000);
        EXPECT_EQ(res.value, exact_partition(a, k)) << "k=" << k;
    }
}

TEST(WqsBinarySearch, PartitionSingleElement) {
    std::vector<int64_t> a = {7};
    auto oracle = [&a](int64_t lambda) { return penalized_partition(a, lambda); };
    auto res = wqs::minimize_integer(oracle, 1, -1'000'000, 1'000'000);
    EXPECT_EQ(res.value, 49);
}

// ==================== Real-valued λ ====================

TEST(WqsBinarySearch, MinimizeRealSmoothConvex) {
    // Continuous model: h(c) = (c − a)² + b. The λ-penalized unconstrained
    // minimum of h(c) + λc sits at c = a − λ/2 with raw cost h(c).
    const double a = 10.0, b = 3.0;
    auto oracle = [a, b](double lambda) -> std::pair<double, double> {
        const double c = a - lambda / 2.0;
        return {(c - a) * (c - a) + b, c};
    };
    for (int64_t k = 2; k <= 8; ++k) {
        const double expected = (static_cast<double>(k) - a) * (static_cast<double>(k) - a) + b;
        EXPECT_NEAR(wqs::minimize_real(oracle, k, -100.0, 100.0), expected, 1e-6)
            << "k=" << k;
    }
}

TEST(WqsBinarySearch, MaximizeRealSmoothConcave) {
    // h(c) = −(c − a)² + b; penalized maximum of h(c) − λc sits at c = a − λ/2.
    const double a = 6.0, b = 20.0;
    auto oracle = [a, b](double lambda) -> std::pair<double, double> {
        const double c = a - lambda / 2.0;
        return {-(c - a) * (c - a) + b, c};
    };
    for (int64_t k = 1; k <= 5; ++k) {
        const double expected = -(static_cast<double>(k) - a) * (static_cast<double>(k) - a) + b;
        EXPECT_NEAR(wqs::maximize_real(oracle, k, -100.0, 100.0), expected, 1e-6)
            << "k=" << k;
    }
}

// ==================== Randomized stress tests ====================

TEST(WqsBinarySearch, RandomizedTopKSelection) {
    std::mt19937 rng(24680u);
    std::uniform_int_distribution<int64_t> val_dist(-1000, 1000);
    std::uniform_int_distribution<int> size_dist(1, 40);

    for (int trial = 0; trial < 200; ++trial) {
        const int n = size_dist(rng);
        std::vector<int64_t> vals(n);
        for (auto& v : vals) v = val_dist(rng);

        std::uniform_int_distribution<int64_t> k_dist(1, n);
        const int64_t k = k_dist(rng);

        auto max_res = wqs::maximize_integer(make_pick_max_oracle(vals), k, -1001, 1001);
        EXPECT_EQ(max_res.value, top_k_sum(vals, k)) << "trial=" << trial << " k=" << k;

        auto min_res = wqs::minimize_integer(make_pick_min_oracle(vals), k, -1001, 1001);
        EXPECT_EQ(min_res.value, bottom_k_sum(vals, k)) << "trial=" << trial << " k=" << k;
    }
}

TEST(WqsBinarySearch, RandomizedPartition) {
    std::mt19937 rng(13579u);
    std::uniform_int_distribution<int64_t> val_dist(0, 20);
    std::uniform_int_distribution<int> size_dist(1, 14);

    for (int trial = 0; trial < 60; ++trial) {
        const int n = size_dist(rng);
        std::vector<int64_t> a(n);
        for (auto& v : a) v = val_dist(rng);

        auto oracle = [&a](int64_t lambda) { return penalized_partition(a, lambda); };
        for (int64_t k = 1; k <= n; ++k) {
            auto res = wqs::minimize_integer(oracle, k, -1'000'000, 1'000'000);
            EXPECT_EQ(res.value, exact_partition(a, k))
                << "trial=" << trial << " n=" << n << " k=" << k;
        }
    }
}
