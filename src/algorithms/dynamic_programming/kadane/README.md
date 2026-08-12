# Kadane's Algorithm (Maximum Subarray)

## Overview

Kadane's algorithm finds a non-empty contiguous subarray with the largest
possible sum. For example, in:

```text
[-2, 1, -3, 4, -1, 2, 1, -5, 4]
```

the maximum-sum subarray is `[4, -1, 2, 1]`, whose sum is `6`.

This module returns both the maximum sum and the half-open range `[begin, end)`
that produces it.

## Dynamic Programming Recurrence

Let `best_ending_at[i]` be the maximum sum of a non-empty subarray that ends at
index `i`. Such a subarray either:

1. starts at `i`, or
2. extends the best subarray ending at `i - 1`.

Therefore:

```text
best_ending_at[i] = max(values[i], best_ending_at[i - 1] + values[i])
```

The overall answer is the maximum of `best_ending_at[i]` across all indices.
Only the previous state is needed, so the full DP table can be compressed to a
single running sum.

## Algorithm

1. Initialize the current and global best subarrays with the first element.
2. For every next element:
   - discard the previous subarray when its sum is negative;
   - otherwise extend it with the current element.
3. Update the global best when the new candidate has a greater sum.

The implementation keeps a zero-sum prefix rather than discarding it because
ties are resolved in favor of the earliest starting index.

## API

Header: `algorithms/dynamic_programming/kadane/kadane.h`

```cpp
#include <algorithms/dynamic_programming/kadane/kadane.h>

#include <cstdint>
#include <vector>

namespace kadane = algorithms::dynamic_programming::kadane;

std::vector<int64_t> values = {-2, 1, -3, 4, -1, 2, 1, -5, 4};

auto result = kadane::maximum_subarray(values);
// result->sum == 6
// result->begin == 3
// result->end == 7

auto sum = kadane::maximum_subarray_sum(values);
// *sum == 6
```

`maximum_subarray` always selects a non-empty subarray. Empty input returns
`std::nullopt`. If several ranges have the same maximum sum, the range with the
smallest `begin` is selected; if those are also equal, the smallest `end` is
selected.

## Complexity

| Metric | Bound |
|---|---:|
| Time | `O(n)` |
| Auxiliary space | `O(1)` |

The input is read once and is not modified.

## Common Pitfalls

- Initializing the answer to zero incorrectly allows an empty subarray and
  fails for arrays containing only negative values.
- Resetting the current sum when it is zero changes which range wins a tie.
- The returned `end` index is exclusive, matching standard C++ iterator and
  range conventions.
- All possible subarray sums must fit in `int64_t`; signed integer overflow is
  outside the function's contract.
- Kadane's algorithm solves the one-dimensional contiguous problem. Circular
  arrays and maximum-sum rectangles require additional logic.

See [proof.md](proof.md) for the correctness argument.
