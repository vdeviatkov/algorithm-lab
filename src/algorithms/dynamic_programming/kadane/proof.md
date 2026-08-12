# Kadane's Algorithm — Proof of Correctness

## 1. Dynamic Programming State

For each index `i`, define `E(i)` as the maximum sum of a non-empty contiguous
subarray whose final element is `values[i]`.

Every such subarray is one of exactly two forms:

- it contains only `values[i]`; or
- it extends a non-empty subarray ending at `i - 1`.

Among subarrays of the second form, extending the one with maximum sum is
optimal. Hence:

```text
E(i) = max(values[i], E(i - 1) + values[i]).
```

The implementation expresses the same recurrence by starting a new range when
the previous running sum is negative, and extending it otherwise. When the
running sum is zero, both choices have equal sums; retaining the existing range
implements the documented earliest-start tie rule.

## 2. Loop Invariant

After processing index `i`:

1. `current_sum` is `E(i)`, and `[current_begin, i + 1)` is the earliest range
   attaining that sum among subarrays ending at `i`.
2. `best` is the maximum-sum non-empty subarray contained in `[0, i + 1)`, with
   ties resolved by smallest `begin` and then smallest `end`.

### Initialization

Before the loop, index `0` is the only processed element. The only non-empty
subarray ending at or contained in `[0, 1)` is `[0, 1)`. Both invariant claims
therefore hold.

### Maintenance

Assume the invariant holds after index `i - 1`.

- If `current_sum < 0`, adding it to `values[i]` would produce a smaller sum
  than starting at `i`, so the optimal ending-at-`i` range is `[i, i + 1)`.
- If `current_sum > 0`, extending the previous optimal range is better than
  starting at `i`.
- If `current_sum == 0`, both choices have equal sums. Extending preserves the
  smaller starting index required by the tie rule.

Thus the updated current range satisfies claim 1. Every non-empty subarray in
`[0, i + 1)` either ends before `i` and is represented by the previous `best`,
or ends at `i` and is represented by the updated current range. Comparing these
two candidates, including the tie rule, establishes claim 2.

### Termination

After the final index, claim 2 states that `best` is exactly the desired
maximum-sum non-empty subarray of the entire input. Therefore the algorithm is
correct.

## 3. Complexity

The algorithm performs constant work for each of `n` elements, so its running
time is `O(n)`. It stores only the current candidate and global best, requiring
`O(1)` auxiliary space.
