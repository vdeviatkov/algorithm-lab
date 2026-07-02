# WQS Binary Search — Proof of Correctness

Throughout, consider the minimization form (the maximization form is symmetric with all inequalities flipped and the penalty sign changed).

## 1. Setup

Let `S` range over a family of feasible solutions with objective `f(S)` and size `|S| ∈ {0, 1, …, n}`. Define

```
h(k) := min { f(S) : |S| = k }
```

**Assumption (convexity):** the increments `d_i := h(i+1) − h(i)` are nondecreasing in `i`:

```
d_0 ≤ d_1 ≤ … ≤ d_{n−1}
```

The oracle solves the *penalized, unconstrained* problem

```
g(λ) := min_S [ f(S) + λ·|S| ] = min_{0 ≤ i ≤ n} [ h(i) + λ·i ]
```

and returns `(raw, cnt)` where `raw = f(S*) = h(cnt)` for a penalized optimum `S*`.

---

## 2. Structure of the penalized optima

**Lemma 1 (optima form an interval).** For fixed λ, index `i` minimizes `h(i) + λi` iff

```
d_{i−1} ≤ −λ ≤ d_i
```

(with the boundary conventions `d_{−1} = −∞`, `d_n = +∞`).

*Proof.* `i` is optimal iff it beats both neighbors under convexity: moving from `i` to `i+1` changes the total by `d_i + λ ≥ 0`, and moving from `i` to `i−1` changes it by `−d_{i−1} − λ ≥ 0`. Because the `d_i` are nondecreasing, beating both neighbors implies beating every index (a walk away from `i` accumulates only nonnegative changes). ∎

Consequently the set of optimal indices at λ is a contiguous interval `[k_min(λ), k_max(λ)]` with

```
k_max(λ) = max { i : d_{i−1} ≤ −λ },     k_min(λ) = min { i : d_i ≥ −λ }.
```

**Lemma 2 (monotone counts).** `k_max(λ)` and `k_min(λ)` are nonincreasing in λ.

*Proof.* Increasing λ decreases `−λ`; since the `d_i` are nondecreasing, the set `{ i : d_{i−1} ≤ −λ }` can only shrink. Same for `k_min`. ∎

Lemma 2 is what makes **binary search on λ** valid: the count returned by an oracle that tie-breaks to the largest count is exactly `k_max(λ)`, a nonincreasing step function of λ.

---

## 3. The integer-λ binary search is exact

The implementation finds

```
λ* := max { λ ∈ [λ_lo, λ_hi] : k_max(λ) ≥ k }
```

by locating the smallest λ with `k_max(λ) < k` (standard predicate bisection over the integers; the predicate is monotone by Lemma 2) and stepping back one. The bracket precondition `k_max(λ_lo) ≥ k` and `k_max(λ_hi + 1) < k` guarantees λ* exists in range.

**Assumption (integer slopes):** `f` is integer-valued, hence every `d_i` is an integer.

**Theorem.** With the tie-break rule (oracle returns count `k_max(λ)`), `k` is an optimal index at λ*, i.e. `d_{k−1} ≤ −λ* ≤ d_k`.

*Proof.*
- **Right inequality.** By choice of λ*, `k_max(λ* + 1) < k`, so `k` is *not* in `{ i : d_{i−1} ≤ −λ* − 1 }`; that is `d_{k−1} > −λ* − 1`. With integer slopes this means `d_{k−1} ≥ −λ*`.
- **Left inequality.** `k_max(λ*) ≥ k` gives `d_{k−1} ≤ d_{k_max−1} ≤ −λ*`.

Combining: `d_{k−1} = −λ*` ≤ `d_k` (convexity), so `d_{k−1} ≤ −λ* ≤ d_k` and by Lemma 1, `k` minimizes `h(i) + λ*·i`. ∎

(If the bracket degenerates — `k_max(λ) ≥ k` for every λ ≤ λ_hi + 1 — the search returns λ* = λ_hi; correctness then requires `k_max(λ_hi) = k`, which is the documented precondition, e.g. `k = 1` with λ_hi large enough that the oracle returns a single segment.)

**Corollary (recovery formula).** Since both `k` and `cnt = k_max(λ*)` are optimal at λ*:

```
h(k) + λ*·k = g(λ*) = raw + λ*·cnt
⟹ h(k) = raw + λ*·(cnt − k)
```

which is exactly what `minimize_integer` returns. For maximization the penalty enters as `f(S) − λ|S|`, and the same argument yields `h(k) = raw + λ*·(k − cnt)`. ∎

**Why the tie-break matters.** At a corner of the convex envelope, several counts share one supporting slope. Without "prefer larger count", the oracle could report a count on the wrong side of `k` at λ* and at λ*+1, and no single λ would *appear* to support `k` — even though one does. Tie-breaking to `k_max` makes the observed count function exactly the upper envelope `k_max(λ)`, for which the theorem holds.

---

## 4. Termination and complexity

The integer bisection maintains a half-open range that shrinks by half each step, terminating in `⌈log₂(λ_hi − λ_lo + 2)⌉` iterations, each costing one oracle call:

```
O(log(λ_range) · T_oracle)
```

The real-λ variants run a fixed number of bisection steps (default 200 ≫ the 53 bits of a double mantissa), so they terminate unconditionally and converge to the supporting λ at machine precision; the recovery formula then holds in the limit by continuity of `g` (a pointwise min of affine functions is concave, hence continuous). ∎

---

## 5. What convexity buys (and what breaks without it)

If `h` is not convex, Lemma 1 fails: the penalized optimum jumps over intermediate counts, `k_max(λ)` need not pass near `k`, and the tangent-line recovery reconstructs the **lower convex envelope** of `h`, not `h` itself. WQS then silently returns `conv(h)(k) ≤ h(k)`. Verifying convexity (quadrangle inequality on segment costs, exchange arguments, or matroid intersection structure) is a prerequisite, not an optimization.

---

## References

- IOI 2016, problem "Aliens" — the task that popularized the technique.
- CP-Algorithms / Codeforces folklore: "Aliens trick", "WQS binary search" (after 王钦石 Wang Qinshi, whose Chinese OI paper introduced it).
- N. Megiddo, *Combinatorial Optimization with Rational Objective Functions* — parametric-search ancestry.
- Lagrangian relaxation: A. M. Geoffrion, *Lagrangean Relaxation for Integer Programming*, 1974.
