# Möbius Function — Proof of Correctness

## 1. The fundamental identity

**Lemma.**
```
Σ_{d | n} μ(d) = [n = 1]     (1 if n = 1, else 0)
```

*Proof.* For `n = 1` the sum is `μ(1) = 1`. For `n > 1`, write `n = p₁^{a₁} … p_k^{a_k}` with `k ≥ 1`. Divisors with a squared prime factor contribute 0, so only the `2^k` squarefree divisors (products of subsets of `{p₁, …, p_k}`) matter:

```
Σ_{d | n} μ(d) = Σ_{j=0}^{k} C(k, j) · (−1)^j = (1 − 1)^k = 0.   ∎
```

This is exactly inclusion–exclusion: μ is the Möbius function of the divisor lattice.

## 2. Möbius inversion theorem

**Theorem.** For arithmetic functions `f, F`:
```
F(n) = Σ_{d | n} f(d)  for all n   ⟺   f(n) = Σ_{d | n} μ(n/d) F(d)  for all n
```

*Proof (⇒).* Substitute and swap summation order (every pair `e | d | n` appears once):

```
Σ_{d | n} μ(n/d) F(d) = Σ_{d | n} μ(n/d) Σ_{e | d} f(e)
                      = Σ_{e | n} f(e) · Σ_{m | (n/e)} μ((n/e)/m)
                      = Σ_{e | n} f(e) · [n/e = 1]        (by the Lemma)
                      = f(n).   ∎
```

The converse direction is the same computation run backwards.

## 3. Correctness of `mu(n)` (trial division)

The loop tries every `p` with `p² ≤ n` in increasing order against the *shrinking* value of `n`. When `p` divides, `p` is prime: any composite divisor would have a smaller prime factor already divided out. After dividing once, a second division by `p` certifies a squared factor → return 0; otherwise the sign flips once per distinct prime. If the loop ends with `n > 1`, the remainder is a single prime `> √(original n)` (two such factors would exceed the original value), flipping the sign once more. The result is `(−1)^{#distinct primes}` for squarefree `n` and `0` otherwise. ∎

## 4. Correctness of `mobius_up_to(n)` (linear sieve)

**Invariant of the linear sieve.** Every composite `m` is assigned `composite[m] = 1` exactly once, as `m = p · i` where `p` is the *smallest* prime factor of `m` and `i = m / p`. (Standard argument: the inner loop breaks at the first `p | i`, so `i`'s smallest prime factor bounds the primes that pair with `i` from above.)

Given that decomposition, the recurrences hold:

- **`p ∤ i`:** then `p` appears in `m = p·i` with exponent 1, and `m` squarefree ⟺ `i` squarefree with one fewer prime, so `μ(m) = −μ(i)` (both are 0 when `i` is not squarefree).
- **`p | i`:** then `p² | m`, so `μ(m) = 0`.
- **`m` prime:** `μ(m) = −1` directly.

Every entry `2..n` is written through exactly one of these three cases, and case inputs `μ(i)` with `i < m` are already final (the loop processes `i` in increasing order). By induction the table equals μ. ∎

**Complexity:** each composite is crossed out once → O(n) total inner-loop steps.

## 5. Correctness of the in-place transforms

Both loops touch `a[m]` only from `a[d]` with `d | m`, `d < m`.

**Forward (`divisor_sum_transform`), d descending.** Claim: when the outer loop reaches `d`, `a[d]` still holds the original `f(d)`. Position `d` is only modified by outer iterations `d' > d` with `d' | d` — impossible since a proper divisor is smaller. So each `a[m]` accumulates `f(d)` for every proper divisor `d`, on top of its own `f(m)`: `a[m] = Σ_{d|m} f(d) = F(m)`. ∎

**Inverse (`mobius_inversion`), d ascending.** Claim (induction on `d`): when the outer loop reaches `d`, `a[d] = f(d)`. Base: `a[1] = F(1) = f(1)`. Step: all proper divisors `d' < d` of `d` have already run their inner loops, each having subtracted `f(d')` from `a[d]`; hence `a[d] = F(d) − Σ_{d'|d, d'<d} f(d') = f(d)`. The subtraction pattern then delivers exactly `f(n) = Σ_{d|n} μ(n/d) F(d)` by the inversion theorem (§2). ∎

**Complexity:** the double loop performs `Σ_{d=1}^{N} ⌊N/d⌋ − 1 = O(N log N)` operations (harmonic series).

## 6. Multiplicativity (used by applications)

`μ(mn) = μ(m)·μ(n)` for `gcd(m, n) = 1`: the prime factorizations are disjoint, so squarefree-ness and prime counts add up. This justifies identities such as

```
[gcd(a, b) = 1] = Σ_{d | gcd(a,b)} μ(d)
```

(a direct restatement of the Lemma with `n = gcd(a,b)`), which underlies the coprime-pair and squarefree counting formulas tested in this module. ∎

## References

- Hardy & Wright, *An Introduction to the Theory of Numbers*, §16.3–16.5.
- G.-C. Rota, *On the Foundations of Combinatorial Theory I: Theory of Möbius Functions*, 1964 — the lattice-theoretic generalization.
- CP-Algorithms: "Linear sieve" — the O(n) sieve invariant.
