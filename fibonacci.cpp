#include <cstdint>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>

#define ALLOW_SAME_ASM_RESULT_CODE

constexpr std::uint64_t kN = 45;
constexpr int knTrial = 1;


static inline std::uint64_t
fib(std::uint64_t n) noexcept
{
  return n < 2 ? n : (fib(n - 1) + fib(n - 2));
}

#ifdef ALLOW_SAME_ASM_RESULT_CODE
template<typename F>
inline constexpr decltype(auto)
fix(F&& f) noexcept
{
  return [f = std::forward<F>(f)](auto&&... args) {
    return f(f, std::forward<decltype(args)>(args)...);
  };
}
#endif  // ALLOW_SAME_ASM_RESULT_CODE


template<typename F>
class FixPoint : F
{
public:
  FixPoint(F&& f) noexcept
    : F(std::forward<F>(f))
  {}

  template<typename... Args>
  constexpr decltype(auto)
  operator()(Args&&... args) const
  {
    return F::operator()(*this, std::forward<Args>(args)...);
  }
};  // class FixPoint

template<typename F>
static inline constexpr auto
makeFixPoint(F&& f) noexcept
{
  return FixPoint<F>{std::forward<F>(f)};
}


#ifdef ALLOW_SAME_ASM_RESULT_CODE
class Fibonacci01
{
public:
  constexpr std::uint64_t
  operator()(std::uint64_t n) const noexcept
  {
    return n < 2 ? n : (Fibonacci01{}(n - 1) + Fibonacci01{}(n - 2));
  }
};  // struct Fibonacci01


class Fibonacci02
{
public:
  constexpr std::uint64_t
  operator()(std::uint64_t n) const noexcept
  {
    return n < 2 ? n : ((*this)(n - 1) + (*this)(n - 2));
  }
};  // struct Fibonacci02
#endif  // ALLOW_SAME_ASM_RESULT_CODE


class Fibonacci03
{
public:
  constexpr std::uint64_t
  operator()(std::uint64_t n) const noexcept
  {
    return n < 2 ? n : (operator()(n - 1) + operator()(n - 2));
  }
};  // struct Fibonacci03


#ifdef ALLOW_SAME_ASM_RESULT_CODE
class Fibonacci04
{
public:
  constexpr std::uint64_t
  operator()(Fibonacci04 f, std::uint64_t n) const noexcept
  {
    return n < 2 ? n : (f(f, n - 1) + f(f, n - 2));
  }
};  // struct Fibonacci04
#endif  // ALLOW_SAME_ASM_RESULT_CODE




template<
  typename F,
  typename... Args
>
static inline auto
measureTime(F&& f, Args&&... args) noexcept
{
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < knTrial; i++) {
    f(std::forward<Args>(args)...);
  }
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / (1000.0 * knTrial);
}


template<
  typename F,
  typename... Args
>
static inline auto
showElapsedTime(const std::string& title, F&& f, Args&&... args) noexcept
{
  std::cout << title << ": " << measureTime(std::forward<F>(f), std::forward<Args>(args)...) << " ms" << std::endl;
}




int
main(int argc, const char* argv[])
{
  auto n = argc > 1 ? std::stoull(argv[1]) : kN;  // Prevent compile time calculation

  showElapsedTime("Normal function", [n] { const volatile auto result = fib(n); });

  showElapsedTime("std::function<>", [n] {
    std::function<std::uint64_t(std::uint64_t)> fib{
      [&fib](std::uint64_t n) -> std::uint64_t {
        return n < 2 ? n : (fib(n - 1) + fib(n - 2));
      }};
    const volatile auto result = fib(n);
    static_cast<void>(result);
  });

#ifdef ALLOW_SAME_ASM_RESULT_CODE
  showElapsedTime("fix()", [n] {
    const volatile auto result = fix([](auto f, std::uint64_t n) -> std::uint64_t {
      return n < 2 ? n : (f(f, n - 1) + f(f, n - 2));
    })(n);
  });
#endif  // ALLOW_SAME_ASM_RESULT_CODE

  showElapsedTime("FixPoint class", [n] {
    const volatile auto result = makeFixPoint([](auto f, std::uint64_t n) -> std::uint64_t {
      return n < 2 ? n : (f(n - 1) + f(n - 2));
    })(n);
  });

  showElapsedTime("FixPoint class (auto&&)", [n] {
    const volatile auto result = makeFixPoint([](auto&& f, std::uint64_t n) -> std::uint64_t {
      return n < 2 ? n : (f(n - 1) + f(n - 2));
    })(n);
  });

#ifdef ALLOW_SAME_ASM_RESULT_CODE
  showElapsedTime("Y-combinator", [n] {
    const volatile auto result = [](auto f) {
      return [=](auto&&... args) {
        return f(f, std::forward<decltype(args)>(args)...);
      };
    }([](auto f, std::uint64_t n) -> std::uint64_t {
      return n < 2 ? n : f(f, n - 1) + f(f, n - 2);
    })(n);
  });

#  ifndef __clang__
  showElapsedTime("Z-combinator", [n] {
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
  showElapsedTime("Fibonacci01 class", [n] { const volatile auto result = Fibonacci01{}(n); });

  showElapsedTime("Fibonacci02 class", [n] { const volatile auto result = Fibonacci02{}(n); });
#endif  // ALLOW_SAME_ASM_RESULT_CODE

  showElapsedTime("Fibonacci03 class", [n] { const volatile auto result = Fibonacci03{}(n); });

#ifdef ALLOW_SAME_ASM_RESULT_CODE
  showElapsedTime("Fibonacci04 class", [n] {
    Fibonacci04 f;
    const volatile auto result = f(f, n);
  });
#endif  // ALLOW_SAME_ASM_RESULT_CODE
}
