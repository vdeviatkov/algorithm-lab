# Euler's Totient Function (φ)

## Overview

Euler's totient function `φ(n)` counts the integers in `[1, n]` that are
coprime to `n`:

```text
φ(n) = |{x : 1 <= x <= n and gcd(x, n) = 1}|
```

This module provides:

- `phi(n)` — compute one value by factoring `n`.
- `totients_up_to(n)` — compute every value from `φ(0)` through `φ(n)` with a
  linear sieve.

The module uses the conventions `φ(0) = 0` and `φ(1) = 1`.

## Formula

If the distinct prime factors of `n` are `p₁, p₂, ..., pₖ`, then:

```text
φ(n) = n * (1 - 1/p₁) * (1 - 1/p₂) * ... * (1 - 1/pₖ)
```

The implementation applies each factor using exact integer arithmetic:

```text
result -= result / p
```

Only distinct prime factors matter. For example:

```text
36 = 2² * 3²
φ(36) = 36 * (1 - 1/2) * (1 - 1/3) = 12
```

## Algorithm

### Single value

To compute `phi(n)`:

1. Set `result = n`.
2. Trial-divide `n` by every possible prime factor `p`.
3. When `p` divides `n`, remove every copy of `p` and update
   `result -= result / p`.
4. If a prime factor greater than the square root remains, apply it once.

### Table of values

`totients_up_to(n)` uses a linear sieve. For a prime `p`:

```text
φ(p) = p - 1
```

For a previously processed `i`:

```text
φ(p * i) = φ(i) * p       if p divides i
φ(p * i) = φ(i) * (p - 1) otherwise
```

The linear-sieve ordering generates every composite exactly once.

## Complexity

| Operation | Time | Memory |
|---|---:|---:|
| `phi(n)` | `O(sqrt(n))` | `O(1)` |
| `totients_up_to(n)` | `O(n)` | `O(n)` |

The single-value bound is worst-case and occurs for prime `n`. Removing a small
prime factor can shrink the remaining value much earlier.

## API

Header: `algebra/totient/totient.h`

```cpp
#include "algebra/totient/totient.h"

using algebra::totient::phi;
using algebra::totient::totients_up_to;

auto value = phi(36);
// value == 12

auto table = totients_up_to(10);
// table == {0, 1, 1, 2, 2, 4, 2, 6, 4, 6, 4}
```

## Useful Identities

Euler's totient is multiplicative:

```text
φ(a * b) = φ(a) * φ(b), when gcd(a, b) = 1
```

Its divisor sum satisfies:

```text
sum of φ(d) over all divisors d of n = n
```

Euler's theorem connects the function to modular arithmetic:

```text
a^φ(n) ≡ 1 (mod n), when gcd(a, n) = 1
```

## Notes / Pitfalls

- `φ(p) = p - 1` for a prime `p`.
- `φ(p^k) = p^k - p^(k-1)` for a prime power.
- Multiplicativity requires coprime arguments; it is not generally true that
  `φ(a * b) = φ(a) * φ(b)`.
- The count includes `1`. With the standard definition, `φ(1) = 1`.
- For many queries sharing the same upper bound, build the table once instead
  of repeatedly factoring each integer.

See [proof.md](proof.md) for correctness arguments.
