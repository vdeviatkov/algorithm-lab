#pragma once

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace algorithms::sorting::wqs_binary_search {

// WQS Binary Search — also known as the "Aliens Trick" (from IOI 2016 "Aliens")
// or Lagrangian relaxation over the count constraint.
//
// Problem shape:
//   Given a family of feasible solutions S with an objective f(S) and an integer
//   "count" |S| (e.g. number of picked items / groups / operations), compute
//       h(k) := opt_{S : |S| = k} f(S)
//   for a specific target k.
//
// Assumption:
//   h(k) is convex in k for minimization (or concave in k for maximization).
//
// Reduction:
//   Attach a Lagrange multiplier λ to the count constraint and drop it:
//       g(λ) = opt_S [ f(S) ± λ|S| ]     (+ for min, − for max)
//   Then g is the Legendre / Fenchel conjugate of h, and h(k) is recovered from
//   the tangent line of slope k. As λ varies monotonically, so does the count
//   returned by the unconstrained oracle — enabling binary search on λ.
//
// Complexity:
//   O(log(range) · T_oracle), where T_oracle is the cost of one unconstrained
//   (penalized) solve. This shaves a factor of k off many O(n·k)-style DPs.
//
// See proof.md in this module for the full derivation.

/// Result of a WQS binary search.
template <typename Cost>
struct wqs_result {
    Cost value;      ///< recovered h(k)
    int64_t lambda;  ///< integer Lagrange multiplier chosen by the search
    int64_t count;   ///< count returned by the oracle at that lambda
};

namespace detail {

// Extract the first element (raw cost) type from an oracle return of the form
// std::pair<Cost, int64_t> (or std::tuple with matching layout).
template <typename Oracle>
using oracle_return_t =
    std::invoke_result_t<Oracle&, int64_t>;

template <typename Oracle>
using cost_t = std::remove_cv_t<std::remove_reference_t<
    decltype(std::get<0>(std::declval<oracle_return_t<Oracle>>()))>>;

template <typename Oracle>
using oracle_return_real_t =
    std::invoke_result_t<Oracle&, double>;

} // namespace detail

// ========================= Integer λ: minimize =========================
//
// Solve  min_{S : |S| = k} f(S)  where h is convex in k.
//
// `oracle(λ)` (with `λ` a 64-bit integer) must minimize
//     f(S) + λ · |S|
// over all unconstrained feasible S and return a pair-like (raw_cost, count).
// `raw_cost` is the *unpenalized* f(S); do not fold the λ|S| term into it.
//
// Tie-breaking: when several optima have the same penalized total, the oracle
// should prefer the one with the *larger* count. This is the standard "prefer
// more items" convention and makes the recovery formula exact even at corners
// of the piecewise-linear envelope.
//
// The bracket [lambda_lo, lambda_hi] must satisfy
//     oracle(lambda_lo).count ≥ k    and    oracle(lambda_hi + 1).count < k,
// i.e. it must contain the transition where the optimal count drops below k.
//
// Returns h(k) via h(k) = raw_cost + λ · (count − k).
template <typename Oracle>
auto minimize_integer(Oracle&& oracle,
                      int64_t k,
                      int64_t lambda_lo,
                      int64_t lambda_hi)
    -> wqs_result<detail::cost_t<Oracle>>
{
    using Cost = detail::cost_t<Oracle>;
    // Find the smallest λ in [lambda_lo, lambda_hi + 1] such that count < k.
    // Then the largest λ with count ≥ k is (that − 1).
    int64_t lo = lambda_lo, hi = lambda_hi + 1;
    while (lo < hi) {
        int64_t mid = lo + (hi - lo) / 2;
        auto [cost, cnt] = oracle(mid);
        (void)cost;
        if (cnt >= k) lo = mid + 1;
        else          hi = mid;
    }
    int64_t lambda = lo - 1;
    auto [cost, cnt] = oracle(lambda);
    Cost value = cost + static_cast<Cost>(lambda) * static_cast<Cost>(cnt - k);
    return {value, lambda, cnt};
}

// ========================= Integer λ: maximize =========================
//
// Solve  max_{S : |S| = k} f(S)  where h is concave in k.
//
// `oracle(λ)` must maximize
//     f(S) − λ · |S|
// and return (raw_value, count) with tie-breaking toward the *larger* count.
// Bracket condition: `oracle(lambda_lo).count ≥ k` and
// `oracle(lambda_hi + 1).count < k`.
//
// Returns h(k) via h(k) = raw_value + λ · (k − count).
template <typename Oracle>
auto maximize_integer(Oracle&& oracle,
                      int64_t k,
                      int64_t lambda_lo,
                      int64_t lambda_hi)
    -> wqs_result<detail::cost_t<Oracle>>
{
    using Cost = detail::cost_t<Oracle>;
    int64_t lo = lambda_lo, hi = lambda_hi + 1;
    while (lo < hi) {
        int64_t mid = lo + (hi - lo) / 2;
        auto [val, cnt] = oracle(mid);
        (void)val;
        if (cnt >= k) lo = mid + 1;
        else          hi = mid;
    }
    int64_t lambda = lo - 1;
    auto [val, cnt] = oracle(lambda);
    Cost value = val + static_cast<Cost>(lambda) * static_cast<Cost>(k - cnt);
    return {value, lambda, cnt};
}

// =========================== Real-valued λ =============================
//
// When the count grid is fine enough that integer-λ binary search would not
// terminate meaningfully (or when a real-valued penalty is natural), search λ
// as a `double`. The oracle takes and returns doubles: `oracle(λ) -> (value, count)`.
//
// The search runs for a fixed number of bisection steps; ~60 iterations already
// gets to the limit of double precision.

/// Minimize variant with real λ. Assumes h(k) convex; oracle minimizes
/// f(S) + λ|S| and returns (raw_cost, count).
template <typename Oracle>
double minimize_real(Oracle&& oracle,
                     int64_t k,
                     double lambda_lo,
                     double lambda_hi,
                     int max_iters = 200)
{
    double lo = lambda_lo, hi = lambda_hi;
    for (int i = 0; i < max_iters; ++i) {
        double mid = lo + (hi - lo) / 2.0;
        auto [cost, cnt] = oracle(mid);
        (void)cost;
        if (cnt >= k) lo = mid;
        else          hi = mid;
    }
    auto [cost, cnt] = oracle(lo);
    return cost + lo * static_cast<double>(cnt - k);
}

/// Maximize variant with real λ. Assumes h(k) concave; oracle maximizes
/// f(S) − λ|S| and returns (raw_value, count).
template <typename Oracle>
double maximize_real(Oracle&& oracle,
                     int64_t k,
                     double lambda_lo,
                     double lambda_hi,
                     int max_iters = 200)
{
    double lo = lambda_lo, hi = lambda_hi;
    for (int i = 0; i < max_iters; ++i) {
        double mid = lo + (hi - lo) / 2.0;
        auto [val, cnt] = oracle(mid);
        (void)val;
        if (cnt >= k) lo = mid;
        else          hi = mid;
    }
    auto [val, cnt] = oracle(lo);
    return val + lo * static_cast<double>(k - cnt);
}

} // namespace algorithms::sorting::wqs_binary_search
