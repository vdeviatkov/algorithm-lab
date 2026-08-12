#include "algorithms/dynamic_programming/kadane/kadane.h"

namespace algorithms::dynamic_programming::kadane {

std::optional<SubarrayResult> maximum_subarray(std::span<const int64_t> values) {
    if (values.empty())
        return std::nullopt;

    SubarrayResult best{values.front(), 0, 1};
    int64_t current_sum = values.front();
    std::size_t current_begin = 0;

    for (std::size_t i = 1; i < values.size(); ++i) {
        // A negative prefix can only make a subarray starting at i worse.
        // Keep a zero prefix because ties prefer the earlier begin index.
        if (current_sum < 0) {
            current_sum = values[i];
            current_begin = i;
        } else {
            current_sum += values[i];
        }

        const std::size_t current_end = i + 1;
        const bool is_better_sum = current_sum > best.sum;
        const bool is_earlier_tie =
            current_sum == best.sum &&
            (current_begin < best.begin || (current_begin == best.begin && current_end < best.end));

        if (is_better_sum || is_earlier_tie)
            best = {current_sum, current_begin, current_end};
    }

    return best;
}

std::optional<int64_t> maximum_subarray_sum(std::span<const int64_t> values) {
    const auto result = maximum_subarray(values);
    if (!result)
        return std::nullopt;
    return result->sum;
}

} // namespace algorithms::dynamic_programming::kadane
