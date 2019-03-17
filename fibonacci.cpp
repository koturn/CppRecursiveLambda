#include <cstdint>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

#define ALLOW_SAME_ASM_RESULT_CODE

//! Default argument for fibonnaci function.
constexpr std::uint64_t kN = 45;
//! Default number of trials to measure average execute time.
constexpr int knTrial = 5;


/*!
 * @brief Normal fibonnaci function implemented with recursion.
 * @param [in] n  An argument for fibonacci function.
 * @return n-th Fibonacci number.
 */
static inline std::uint64_t
fib(std::uint64_t n) noexcept
{
  return n < 2 ? n : (fib(n - 1) + fib(n - 2));
}

#ifdef ALLOW_SAME_ASM_RESULT_CODE
/*!
 * @brief Create fixed function which take myself as the first argument.
 * @tparam F  Type of function.
 * @param [in] f  A function.
 * @return Fixed function, which first argument is myself.
 */
template<typename F>
inline constexpr decltype(auto)
fix(F&& f) noexcept
{
  return [f = std::forward<F>(f)](auto&&... args) {
    return f(f, std::forward<decltype(args)>(args)...);
  };
}
#endif  // ALLOW_SAME_ASM_RESULT_CODE


/*!
 * @brief Create fixed function instance.
 * @tparam F  Type of function.
 */
template<typename F>
class FixPoint : F
{
public:
  /*!
   * @brief Ctor. Initialize parent function, such as a lambda.
   * @param [in] f A function
   */
  FixPoint(F&& f) noexcept
    : F(std::forward<F>(f))
  {}

  /*!
   * @brief Call target function via parent lambda.
   * @tparam Args Argument types.
   * @param [in] args  Argument for target function.
   * @return Result of target function calling.
   */
  template<typename... Args>
  constexpr decltype(auto)
  operator()(Args&&... args) const
  {
    return F::operator()(*this, std::forward<Args>(args)...);
  }
};  // class FixPoint


/*!
 * @brief Helper function of type argument inference of FixPoint<F>.
 * @tparam F Type of function
 * @param [in] f  A function.
 * @return Instance of FixPoint<F>.
 */
template<typename F>
static inline constexpr auto
makeFixPoint(F&& f) noexcept
{
  return FixPoint<F>{std::forward<F>(f)};
}


#ifdef ALLOW_SAME_ASM_RESULT_CODE
/*!
 * @brief Fibonacci function implementation with functional object.
 * Create instance to call function at each time.
 */
class Fibonacci01
{
public:
  /*!
   * @brief Body of fibonacci function.
   * @param [in] n  An argument for fibonacci function.
   * @return N-th fibonacci number.
   */
  constexpr std::uint64_t
  operator()(std::uint64_t n) const noexcept
  {
    return n < 2 ? n : (Fibonacci01{}(n - 1) + Fibonacci01{}(n - 2));
  }
};  // struct Fibonacci01


/*!
 * @brief Fibonacci function implementation with functional object.
 * Call operator() using "this" pointer.
 */
class Fibonacci02
{
public:
  /*!
   * @brief Body of fibonacci function.
   * Call operator() via "this" pointer.
   *
   * @param [in] n  An argument for fibonacci function.
   * @return N-th fibonacci number.
   */
  constexpr std::uint64_t
  operator()(std::uint64_t n) const noexcept
  {
    return n < 2 ? n : ((*this)(n - 1) + (*this)(n - 2));
  }
};  // struct Fibonacci02
#endif  // ALLOW_SAME_ASM_RESULT_CODE


/*!
 * @brief Fibonacci function implementation with functional object.
 * A compiler may generate same code as Fibonacci02.
 *
 * Call operator() directory.
 */
class Fibonacci03
{
public:
  /*!
   * @brief Body of fibonacci function.
   * Call operator() directory.
   *
   * @param [in] n  An argument for fibonacci function.
   * @return N-th fibonacci number.
   */
  constexpr std::uint64_t
  operator()(std::uint64_t n) const noexcept
  {
    return n < 2 ? n : (operator()(n - 1) + operator()(n - 2));
  }
};  // struct Fibonacci03


#ifdef ALLOW_SAME_ASM_RESULT_CODE
/*!
 * @brief Fibonacci function implementation with functional object.
 */
class Fibonacci04
{
public:
  /*!
   * @brief Body of fibonacci function which takes myself as the first argument.
   * @param [in] f  Myself
   * @param [in] n  Argument for fibonacci function.
   * @return N-th fibonacci number.
   */
  constexpr std::uint64_t
  operator()(Fibonacci04 f, std::uint64_t n) const noexcept
  {
    return n < 2 ? n : (f(f, n - 1) + f(f, n - 2));
  }
};  // struct Fibonacci04
#endif  // ALLOW_SAME_ASM_RESULT_CODE


/*!
 * @brief Measure average execution time of given function.
 * @tparam F     Type of target function.
 * @tparam Args  Types of arguments for the function.
 * @param [in] nTrial  Number of trials.
 * @param [in] f       Target function.
 * @param [in] args    Argument for target function.
 * @return Average of execution time.
 */
template<
  typename F,
  typename... Args
>
static inline auto
measureTime(int nTrial, F&& f, Args&&... args) noexcept
{
  const auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < nTrial; i++) {
    f(std::forward<Args>(args)...);
  }
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / (1000.0 * knTrial);
}


/*!
 * @brief Measure average time of given function and show its result to stdout.
 * @tparam F  Type of target function.
 * @tparam Args  Types of arguments for the function.
 * @param [in] title   Title string.
 * @param [in] nTrial  Number of trials.
 * @param [in] f       Target function.
 * @param [in] args    Argument for target function.
 */
template<
  typename F,
  typename... Args
>
static inline void
showElapsedTime(const std::string& title, int nTrial, F&& f, Args&&... args) noexcept
{
  std::cout << title << ": " << measureTime(nTrial, std::forward<F>(f), std::forward<Args>(args)...) << " ms" << std::endl;
}




/*!
 * @brief Entry point of this program
 * @param [in] argc  Number of command-line arguments.
 * @param [in] argv  Command-line arguments.
 * @return Status-code. (0 means succeeded, otherwiese something wrong)
 */
int
main(int argc, const char* argv[])
{
  try {
    const auto n = argc > 1 ? std::stoull(argv[1]) : kN;  // Prevent compile time calculation
    const auto nTrial = argc > 2 ? std::stoi(argv[2]) : knTrial;  // Prevent compile time calculation

    showElapsedTime("Normal function", nTrial, [n] { const volatile auto result = fib(n); });

    showElapsedTime("std::function<>", nTrial, [n] {
      std::function<std::uint64_t(std::uint64_t)> fib{
        [&fib](std::uint64_t n) -> std::uint64_t {
          return n < 2 ? n : (fib(n - 1) + fib(n - 2));
        }};
      const volatile auto result = fib(n);
      static_cast<void>(result);
    });

#ifdef ALLOW_SAME_ASM_RESULT_CODE
    showElapsedTime("fix()", nTrial, [n] {
      const volatile auto result = fix([](auto f, std::uint64_t n) -> std::uint64_t {
        return n < 2 ? n : (f(f, n - 1) + f(f, n - 2));
      })(n);
    });
#endif  // ALLOW_SAME_ASM_RESULT_CODE

    showElapsedTime("FixPoint class", nTrial, [n] {
      const volatile auto result = makeFixPoint([](auto f, std::uint64_t n) -> std::uint64_t {
        return n < 2 ? n : (f(n - 1) + f(n - 2));
      })(n);
    });

    showElapsedTime("FixPoint class (auto&&)", nTrial, [n] {
      const volatile auto result = makeFixPoint([](auto&& f, std::uint64_t n) -> std::uint64_t {
        return n < 2 ? n : (f(n - 1) + f(n - 2));
      })(n);
    });

#ifdef ALLOW_SAME_ASM_RESULT_CODE
    showElapsedTime("Y-combinator", nTrial, [n] {
      const volatile auto result = [](auto f) {
        return [=](auto&&... args) {
          return f(f, std::forward<decltype(args)>(args)...);
        };
      }([](auto f, std::uint64_t n) -> std::uint64_t {
        return n < 2 ? n : f(f, n - 1) + f(f, n - 2);
      })(n);
    });

#  ifndef __clang__
    showElapsedTime("Z-combinator", nTrial, [n] {
      // Clang cannot compile following code.
      const volatile auto result = [](auto f) {
        return [=](auto g) {
          return [=](auto&&... args) {
            return f(g(g), std::forward<decltype(args)>(args)...);
          };
        }([=](auto g) {
          return [=](auto&&... args) {
            return f(g(g), std::forward<decltype(args)>(args)...);
          };
        });
      }([](auto f, std::uint64_t n) -> std::uint64_t {
        return n < 2 ? n : (f(n - 1) + f(n - 2));
      })(n);
    });
#  endif  // __clang__
#endif  // ALLOW_SAME_ASM_RESULT_CODE

#ifdef ALLOW_SAME_ASM_RESULT_CODE
    showElapsedTime("Fibonacci01 class", nTrial, [n] { const volatile auto result = Fibonacci01{}(n); });

    showElapsedTime("Fibonacci02 class", nTrial, [n] { const volatile auto result = Fibonacci02{}(n); });
#endif  // ALLOW_SAME_ASM_RESULT_CODE

    showElapsedTime("Fibonacci03 class", nTrial, [n] { const volatile auto result = Fibonacci03{}(n); });

#ifdef ALLOW_SAME_ASM_RESULT_CODE
    showElapsedTime("Fibonacci04 class", nTrial, [n] {
      Fibonacci04 f;
      const volatile auto result = f(f, n);
    });
#endif  // ALLOW_SAME_ASM_RESULT_CODE
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
