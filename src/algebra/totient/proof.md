# Euler's Totient Function — Proof of Correctness

## 1. Product Formula

Let the distinct prime factors of `n` be `p₁, p₂, ..., pₖ`. An integer in
`[1, n]` is not coprime to `n` exactly when it is divisible by at least one of
these primes.

For each `pᵢ`, exactly `n / pᵢ` integers in the range are divisible by `pᵢ`.
Applying the Principle of Inclusion-Exclusion gives:

```text
φ(n) = n
       - sum(n / pᵢ)
       + sum(n / (pᵢ pⱼ))
       - ...
     = n * product over p | n of (1 - 1/p).
```

Every prime divisor appears once in the product, regardless of its exponent in
the factorization of `n`.

## 2. Correctness of `phi(n)`

The algorithm starts with `result = n`. When it discovers a distinct prime
factor `p`, it performs:

```text
result -= result / p
```

All previously applied factors have denominators made of primes distinct from
`p`, so the current result is divisible by `p`. The update is therefore exact
integer arithmetic and multiplies the result by `(1 - 1/p)`.

After applying the update, the algorithm divides every copy of `p` out of the
working value of `n`. Consequently, no prime factor is applied twice.

The loop continues while `p <= n / p`, an overflow-safe form of `p² <= n`.
When it ends, the remaining `n` is either `1` or prime. If it were composite,
it would have a factor no greater than its square root and the loop would not
have ended. The final update applies this possible remaining prime exactly
once.

Thus the algorithm applies precisely the factors in the product formula and
returns `φ(n)`.

The explicit conventions `φ(0) = 0` and `φ(1) = 1` handle the boundary values.

## 3. Linear-Sieve Recurrences

For every prime `p`:

```text
φ(p) = p - 1,
```

because every integer from `1` through `p - 1` is coprime to `p`.

Now consider a processed integer `i` and a prime `p`.

### Case 1: `p` divides `i`

Multiplying `i` by `p` increases only the exponent of a prime that is already
present. From the product formula:

```text
φ(p * i) = p * φ(i).
```

### Case 2: `p` does not divide `i`

The new factor `p` is coprime to `i`. By multiplicativity:

```text
φ(p * i) = φ(p) * φ(i) = (p - 1) * φ(i).
```

The linear sieve generates each composite once as `p * i`, with `p` its
smallest prime factor. It stops the inner loop after the first prime dividing
`i`; this is the standard linear-sieve invariant. Since `i < p * i`, `φ(i)` is
already correct when either recurrence is applied.

By induction over increasing values of `i`, every table entry equals its
Euler totient.

## 4. Multiplicativity

If `gcd(a, b) = 1`, their sets of prime factors are disjoint. Applying the
product formula:

```text
φ(a * b)
  = a * b * product over p | ab of (1 - 1/p)
  = [a * product over p | a of (1 - 1/p)]
    * [b * product over p | b of (1 - 1/p)]
  = φ(a) * φ(b).
```

This property justifies the second linear-sieve recurrence.

## 5. Divisor-Sum Identity

Partition the integers `1, 2, ..., n` by the value of `gcd(x, n)`. For each
divisor `d` of `n`, the integers with `gcd(x, n) = d` correspond after division
by `d` to integers coprime to `n / d`. There are `φ(n / d)` of them.

These disjoint classes contain all `n` integers, so:

```text
sum over d | n of φ(n / d) = n.
```

Since `n / d` also ranges over all divisors of `n`:

```text
sum over d | n of φ(d) = n.
```

This identity is independently checked by the test suite.
