#include <algorithms/dynamic_programming/kadane/kadane.h>
#include <cstdint>
#include <gtest/gtest.h>
#include <optional>
#include <random>
#include <span>
#include <vector>

namespace kadane = algorithms::dynamic_programming::kadane;

namespace {

std::optional<kadane::SubarrayResult> brute_force(std::span<const int64_t> values) {
    if (values.empty())
        return std::nullopt;

    kadane::SubarrayResult best{values.front(), 0, 1};

    for (std::size_t begin = 0; begin < values.size(); ++begin) {
        int64_t sum = 0;
        for (std::size_t end = begin; end < values.size(); ++end) {
            sum += values[end];
            const kadane::SubarrayResult candidate{sum, begin, end + 1};
            if (candidate.sum > best.sum ||
                (candidate.sum == best.sum &&
                 (candidate.begin < best.begin ||
                  (candidate.begin == best.begin && candidate.end < best.end)))) {
                best = candidate;
            }
        }
    }

    return best;
}

} // namespace

TEST(Kadane, EmptyInput) {
    const std::vector<int64_t> values;
    EXPECT_EQ(kadane::maximum_subarray(values), std::nullopt);
    EXPECT_EQ(kadane::maximum_subarray_sum(values), std::nullopt);
}

TEST(Kadane, SingleElement) {
    const std::vector<int64_t> values = {-7};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{-7, 0, 1}));
    EXPECT_EQ(kadane::maximum_subarray_sum(values), -7);
}

TEST(Kadane, ClassicExample) {
    const std::vector<int64_t> values = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{6, 3, 7}));
}

TEST(Kadane, AllNegativeSelectsLargestElement) {
    const std::vector<int64_t> values = {-8, -3, -6, -2, -5, -4};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{-2, 3, 4}));
}

TEST(Kadane, AllPositiveSelectsWholeArray) {
    const std::vector<int64_t> values = {2, 3, 1, 5};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{11, 0, 4}));
}

TEST(Kadane, ZeroValuesUseEarliestShortestRange) {
    const std::vector<int64_t> values = {0, 0, 0};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{0, 0, 1}));
}

TEST(Kadane, EqualSumsUseEarliestBegin) {
    const std::vector<int64_t> values = {1, -1, 1};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{1, 0, 1}));
}

TEST(Kadane, ZeroPrefixIsKeptForTieBreaking) {
    const std::vector<int64_t> values = {0, 5};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{5, 0, 2}));
}

TEST(Kadane, MaximumCanAppearAfterNegativePrefix) {
    const std::vector<int64_t> values = {-100, 4, -1, 2, 1};
    EXPECT_EQ(kadane::maximum_subarray(values), (kadane::SubarrayResult{6, 1, 5}));
}

TEST(Kadane, RandomizedMatchesBruteForce) {
    std::mt19937 rng(12345u);
    std::uniform_int_distribution<int64_t> value_distribution(-20, 20);
    std::uniform_int_distribution<int> size_distribution(0, 40);

    for (int trial = 0; trial < 1'000; ++trial) {
        std::vector<int64_t> values(static_cast<std::size_t>(size_distribution(rng)));
        for (auto& value : values)
            value = value_distribution(rng);

        EXPECT_EQ(kadane::maximum_subarray(values), brute_force(values)) << "trial=" << trial;
    }
}
