# Stars and Bars

## 1. Mathematical Concept

Stars and Bars is a counting technique for distributing **identical objects**
among **distinct groups**. Equivalently, it counts integer solutions to equations
of the form:

$$
x_1 + x_2 + \dots + x_k = n
$$

Each object is represented by a star, and the separators between groups are
represented by bars.

For example, the arrangement

```text
***|*||**
```

represents the solution:

$$
(x_1, x_2, x_3, x_4) = (3, 1, 0, 2)
$$

There are $n$ stars and $k - 1$ bars. Choosing the positions of the bars
completely determines the distribution.

---

## 2. Core Formulas

### Non-negative Solutions

If every variable may be zero:

$$
x_1 + x_2 + \dots + x_k = n, \qquad x_i \ge 0
$$

then the number of solutions is:

$$
\boxed{\binom{n + k - 1}{k - 1}}
$$

Among the $n + k - 1$ positions, choose the $k - 1$ positions occupied by
bars. Equivalently, choose the $n$ positions occupied by stars.

### Positive Solutions

If every variable must be positive:

$$
x_1 + x_2 + \dots + x_k = n, \qquad x_i \ge 1
$$

give one object to every group first. Let $y_i = x_i - 1$. The transformed
equation is:

$$
y_1 + y_2 + \dots + y_k = n - k, \qquad y_i \ge 0
$$

Therefore, when $n \ge k$, the number of solutions is:

$$
\boxed{\binom{n - 1}{k - 1}}
$$

If $n < k$, no valid distribution exists.

### Arbitrary Lower Bounds

Suppose every variable has a lower bound:

$$
x_i \ge L_i
$$

Set $y_i = x_i - L_i$. The remaining number of objects is:

$$
n' = n - \sum_{i=1}^{k} L_i
$$

If $n' < 0$, the answer is zero. Otherwise:

$$
\boxed{\binom{n' + k - 1}{k - 1}}
$$

This shift is the standard way to recognize Stars and Bars problems whose
variables do not initially start at zero.

---

## 3. Application: Distributing Identical Items

### The Problem

How many ways can $12$ identical candies be distributed among $4$ children if
every child must receive at least $2$ candies?

### The Transformation

Let $x_i$ be the number of candies received by child $i$:

$$
x_1 + x_2 + x_3 + x_4 = 12, \qquad x_i \ge 2
$$

Reserve two candies for each child and define $y_i = x_i - 2$. This uses
$4 \cdot 2 = 8$ candies and leaves $4$:

$$
y_1 + y_2 + y_3 + y_4 = 4, \qquad y_i \ge 0
$$

The number of distributions is:

$$
\binom{4 + 4 - 1}{4 - 1}
= \binom{7}{3}
= 35
$$

The important step is not memorizing another formula, but transforming the
constraints into a sum of non-negative variables.

---

## 4. Solution Implementation

Stars and Bars reduces the problem to a binomial coefficient. In competitive
programming, answers are often required modulo a prime, so factorials and
inverse factorials can be precomputed once.

```cpp
#include <cstdint>
#include <vector>

class CombinationsMod {
public:
    CombinationsMod(std::int32_t max_n, std::int64_t mod)
        : mod_(mod), fact_(max_n + 1), inv_fact_(max_n + 1) {
        fact_[0] = 1;
        for (std::int32_t i = 1; i <= max_n; ++i) {
            fact_[i] = fact_[i - 1] * i % mod_;
        }

        inv_fact_[max_n] = power(fact_[max_n], mod_ - 2);
        for (std::int32_t i = max_n; i > 0; --i) {
            inv_fact_[i - 1] = inv_fact_[i] * i % mod_;
        }
    }

    std::int64_t choose(std::int32_t n, std::int32_t r) const {
        if (r < 0 || r > n) {
            return 0;
        }

        return fact_[n] * inv_fact_[r] % mod_
             * inv_fact_[n - r] % mod_;
    }

private:
    std::int64_t power(std::int64_t base, std::int64_t exponent) const {
        std::int64_t result = 1;

        while (exponent > 0) {
            if (exponent & 1) {
                result = result * base % mod_;
            }
            base = base * base % mod_;
            exponent >>= 1;
        }

        return result;
    }

    std::int64_t mod_;
    std::vector<std::int64_t> fact_;
    std::vector<std::int64_t> inv_fact_;
};

std::int64_t count_non_negative_distributions(
    std::int32_t objects,
    std::int32_t groups,
    const CombinationsMod& combinations
) {
    if (objects < 0 || groups <= 0) {
        return 0;
    }

    return combinations.choose(objects + groups - 1, groups - 1);
}

std::int64_t count_positive_distributions(
    std::int32_t objects,
    std::int32_t groups,
    const CombinationsMod& combinations
) {
    if (objects < groups || groups <= 0) {
        return 0;
    }

    return combinations.choose(objects - 1, groups - 1);
}
```

### Complexity

For a maximum required value $N$:

- Precomputation: $O(N)$ time and $O(N)$ memory.
- Each binomial coefficient query: $O(1)$ time.
- Each Stars and Bars query after transformation: $O(1)$ time.

The implementation uses Fermat's little theorem for modular inverses, so the
modulus must be prime and `max_n` must be smaller than the modulus. The
factorial table must also be large enough for the largest upper argument passed
to `choose`.

---

## 5. Upper Bounds and Inclusion-Exclusion

Stars and Bars handles lower bounds directly, but a constraint such as
$x_i \le U_i$ cannot be removed by shifting alone.

For the uniform constraint

$$
x_1 + x_2 + \dots + x_k = n,
\qquad 0 \le x_i \le U
$$

use the Principle of Inclusion-Exclusion to remove distributions in which one
or more variables exceed $U$:

$$
\boxed{
\sum_{j=0}^{k}
(-1)^j
\binom{k}{j}
\binom{n - j(U + 1) + k - 1}{k - 1}
}
$$

A binomial coefficient whose upper argument is smaller than its lower argument
is treated as zero.

This is a common pattern: use Stars and Bars for the unrestricted count, then
use [Inclusion-Exclusion](../pie/README.md) to enforce upper bounds.

---

## 6. Why the Formula Works

Consider all strings containing exactly:

- $n$ stars, and
- $k - 1$ bars.

Every such string defines one non-negative solution. The number of stars before
the first bar is $x_1$, the number between the first and second bars is $x_2$,
and so on. Consecutive bars represent an empty group.

The mapping also works in reverse: every non-negative solution determines
exactly one string of stars and bars. It is therefore a bijection.

The string has $n + k - 1$ positions. Choosing which $k - 1$ positions contain
bars gives:

$$
\binom{n + k - 1}{k - 1}
$$

This proves the formula for non-negative solutions. The positive and
lower-bound variants follow by shifting the variables before applying the same
bijection.

---

## 7. Common Pitfalls

### Identical Objects, Distinct Groups

Stars and Bars applies when the objects are indistinguishable and the groups
are labeled. If the objects are distinct, assigning each of $n$ objects to one
of $k$ groups gives $k^n$ possibilities instead.

If the groups are also indistinguishable, the problem involves integer
partitions rather than Stars and Bars.

### Zero Is Allowed Only in the Non-negative Form

Use:

$$
\binom{n + k - 1}{k - 1}
$$

when empty groups are allowed, and:

$$
\binom{n - 1}{k - 1}
$$

when every group must be non-empty.

### Lower and Upper Bounds Behave Differently

Lower bounds can be removed by subtracting the required minimum from each
variable. Upper bounds usually require Inclusion-Exclusion or dynamic
programming.

### Order Changes the Model

The vector $(x_1, x_2, \dots, x_k)$ records how many objects each labeled group
receives. It does not count different orders of identical objects inside a
group.

---

## 8. Quick Reference

| Constraints | Number of solutions |
|---|---:|
| $x_1 + \dots + x_k = n$, $x_i \ge 0$ | $\binom{n + k - 1}{k - 1}$ |
| $x_1 + \dots + x_k = n$, $x_i \ge 1$ | $\binom{n - 1}{k - 1}$ |
| $x_1 + \dots + x_k = n$, $x_i \ge L_i$ | $\binom{n - \sum L_i + k - 1}{k - 1}$ |

For the last two rows, the answer is zero when there are not enough objects to
satisfy the lower bounds.
