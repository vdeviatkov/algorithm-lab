# Möbius Function & Möbius Inversion

## Overview
The **Möbius function** `μ(n)` is the multiplicative function

```
μ(1) = 1
μ(n) = 0        if n has a squared prime factor
μ(n) = (-1)^k   if n = p₁·p₂·…·p_k  (distinct primes)
```

It is the inclusion–exclusion coefficient over divisor lattices: **Möbius inversion** converts between a function `f` and its divisor sums `F(n) = Σ_{d|n} f(d)`.

This module provides:
- `mu(n)` — a single value via trial division
- `mobius_up_to(n)` — the table `μ(0..n)` via a linear sieve
- `divisor_sum_transform(a)` — in-place `a[n] ← Σ_{d|n} a[d]`
- `mobius_inversion(a)` — the in-place inverse, `a[n] ← Σ_{d|n} μ(n/d)·a[d]`

## Problem statement
1. Given `n`, compute `μ(n)`; or compute the table `μ(1..N)`.
2. Given `F(n) = Σ_{d|n} f(d)` for all `n ≤ N`, recover `f` (and vice versa).

## Algorithm
- **Single value:** trial-divide by `p = 2, 3, …, √n`; a repeated factor gives 0, otherwise flip the sign per distinct prime.
- **Table:** linear sieve — every composite is visited exactly once via its smallest prime factor `p`, with the recurrences `μ(p·i) = −μ(i)` if `p ∤ i` and `μ(p·i) = 0` if `p | i`.
- **Transforms:** the harmonic double loop `for d { for m = 2d, 3d, … }`; direction of iteration makes each in-place pass exact (see header comments and [`proof.md`](./proof.md)).

## Complexity

| Function | Time | Memory |
|---|---|---|
| `mu(n)` | O(√n) | O(1) |
| `mobius_up_to(n)` | O(n) | O(n) |
| `divisor_sum_transform` / `mobius_inversion` | O(N log N) | O(1) extra |

## API
Header: `algebra/mobius/mobius.h`

```cpp
#include "algebra/mobius/mobius.h"

namespace mob = algebra::mobius;

mob::mu(1);   // 1
mob::mu(6);   // 1   (2·3, two distinct primes)
mob::mu(12);  // 0   (2² divides it)

auto table = mob::mobius_up_to(100);  // table[n] == μ(n), table[0] == 0

// Recover Euler's totient from n = Σ_{d|n} φ(d):
std::vector<long long> a(N + 1);
for (std::size_t i = 0; i <= N; ++i) a[i] = i;
mob::mobius_inversion(a);             // now a[n] == φ(n)
```

## Typical applications
- **Coprime counting:** `#{(a,b) : a,b ≤ n, gcd(a,b)=1} = Σ_{d≤n} μ(d)·⌊n/d⌋²`
- **Squarefree counting:** `Q(n) = Σ_{d≤√n} μ(d)·⌊n/d²⌋`
- **GCD-sum convolutions:** replacing `[gcd = 1]` with `Σ_{d|gcd} μ(d)` in double sums
- **Recovering multiplicative functions** (e.g. φ) from divisor-sum identities

## Notes / pitfalls
- `μ(0)` is not mathematically defined; this module uses the convention `μ(0) = 0`.
- The table uses `int8_t` (values are only −1, 0, 1) to keep the sieve cache-friendly.
- The in-place transforms iterate in opposite directions — swapping them silently computes the wrong thing.

## References
- A. F. Möbius, *Über eine besondere Art von Umkehrung der Reihen*, 1832.
- G. H. Hardy, E. M. Wright, *An Introduction to the Theory of Numbers* — §16.3–16.5.
- CP-Algorithms: "Möbius function" / "Linear sieve".
