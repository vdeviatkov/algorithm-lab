# WQS Binary Search (Aliens Trick)

This module implements **WQS Binary Search** — also known as the **Aliens Trick** (after IOI 2016 "Aliens"), **Lagrangian relaxation on a count constraint**, or **parametric search on a penalty**. It comes in four flavors:

1. **`minimize_integer`** — recover `min_{|S| = k} f(S)` with an integer penalty λ
2. **`maximize_integer`** — recover `max_{|S| = k} f(S)` with an integer penalty λ
3. **`minimize_real`** / **`maximize_real`** — real-valued λ via fixed-iteration bisection

Implementation highlights:
- **Header-only**, templated on the oracle callable
- Oracle contract: solve the *unconstrained* penalized problem and return `(raw_value, count)`
- Exact answer recovery via the tangent-line formula `h(k) = raw ± λ·(count − k)`
- `wqs_result` also reports the chosen λ and the oracle's count for debugging

---

## When to use

WQS binary search removes an "exactly k" constraint from an optimization problem, typically shaving a factor of `k` off a DP:

- **DP with a count dimension** — `dp[i][j]` = best over the first `i` items using exactly `j` groups/picks/operations. If the answer is convex in `j`, replace the `j` dimension with a λ-penalty and binary search λ: `O(n·k)` → `O(n log C)`.
- **Classic instances:** IOI 2016 "Aliens", "split an array into k segments minimizing Σ(segment sum)²", "pick k non-adjacent items", k-link shortest path, k-clustering on a line.

**Requirement:** `h(k) = opt_{|S|=k} f(S)` must be **convex** in `k` for minimization (concave for maximization). Typical sufficient conditions: the segment cost satisfies the quadrangle (Monge) inequality, or an exchange argument shows monotone marginal gains.

---

## API

Header: `include/algorithms/sorting/wqs_binary_search/wqs_binary_search.h`

### Integer λ, minimization

```cpp
#include <algorithms/sorting/wqs_binary_search/wqs_binary_search.h>

namespace wqs = algorithms::sorting::wqs_binary_search;

// oracle(λ) minimizes f(S) + λ·|S| over unconstrained S and returns
// {raw_cost, count} — raw_cost is the UNPENALIZED f(S).
// On ties in the penalized total, prefer the larger count.
auto oracle = [&](int64_t lambda) -> std::pair<int64_t, int64_t> {
    // e.g. an O(n) or O(n log n) DP with cost + lambda per segment
    ...
};

auto res = wqs::minimize_integer(oracle, k, lambda_lo, lambda_hi);
// res.value  == min_{|S| = k} f(S)
// res.lambda == chosen multiplier, res.count == oracle count at that λ
```

### Integer λ, maximization

```cpp
// oracle(λ) maximizes f(S) − λ·|S|, same return/tie-break conventions
auto res = wqs::maximize_integer(oracle, k, lambda_lo, lambda_hi);
```

### Real-valued λ

```cpp
// For non-integer costs or very fine count grids
double best = wqs::minimize_real(oracle, k, -1e9, 1e9);   // oracle(double λ)
double best2 = wqs::maximize_real(oracle, k, -1e9, 1e9);
```

### Oracle contract (important)

| Rule | Why |
|------|-----|
| Return the **unpenalized** `f(S)` as `raw` | The λ·count term is re-added/removed by the recovery formula |
| **Tie-break toward larger count** | Makes the recovery exact at corners of the convex envelope |
| Bracket must satisfy `count(λ_lo) ≥ k` and `count(λ_hi + 1) < k` | Guarantees the binary search lands on the supporting slope |

---

## Complexity

| Variant | Time | Space |
|---------|------|-------|
| `minimize_integer` / `maximize_integer` | O(log(λ_hi − λ_lo) · T_oracle) | O(1) extra |
| `minimize_real` / `maximize_real` | O(iters · T_oracle), default 200 iters | O(1) extra |

Compared to a direct DP with a count dimension (`O(n·k)`), the WQS reduction costs `O(n log C)` where `C` is the λ range — usually the maximum cost difference of adding one more item.

---

## Comparison with related techniques

| Technique | Removes | Requires |
|-----------|---------|----------|
| WQS / Aliens trick | "exactly k" constraint | h(k) convex/concave |
| Divide & Conquer opt | inner DP loop | opt(i, j) monotone |
| Knuth / SMAWK | inner DP loop | Monge (quadrangle) costs |
| Slope trick | DP over values | piecewise-linear convex DP states |

WQS composes with the others: the penalized oracle itself is often a D&C-opt or SMAWK-accelerated DP.

---

## Proof / correctness

See [`proof.md`](./proof.md) — covers convexity → monotone counts, exactness of the integer-λ recovery with the tie-break rule, and the bracket condition.
