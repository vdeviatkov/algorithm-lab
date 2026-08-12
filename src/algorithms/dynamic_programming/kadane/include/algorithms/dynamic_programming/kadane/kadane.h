#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

namespace algorithms::dynamic_programming::kadane {

struct SubarrayResult {
    int64_t sum;
    std::size_t begin;
    std::size_t end;

    bool operator==(const SubarrayResult&) const = default;
};

// Find a non-empty contiguous subarray with maximum sum.
// The returned range is half-open: [begin, end).
//
// Ties are resolved by the smallest begin index, then the smallest end index.
// An empty input has no non-empty subarray and returns std::nullopt.
//
// Time: O(n). Auxiliary space: O(1).
std::optional<SubarrayResult> maximum_subarray(std::span<const int64_t> values);

// Convenience wrapper returning only the maximum sum.
// Returns std::nullopt for an empty input.
std::optional<int64_t> maximum_subarray_sum(std::span<const int64_t> values);

} // namespace algorithms::dynamic_programming::kadane
