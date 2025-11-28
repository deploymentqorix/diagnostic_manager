#ifndef ARA_CORE_RESULT_FUTURE_H_
#define ARA_CORE_RESULT_FUTURE_H_

#include <system_error>
#include <type_traits>
#include <utility>

namespace ara {
namespace core {

// Minimal ara::core::Result<T> placeholder used by this repo's components.
// Supports construction from a value (success) or std::error_code (failure).
template <typename T>
class Result {
public:
    Result() noexcept : value_{}, ec_{} {}
    explicit Result(std::error_code ec) noexcept : value_{}, ec_{ec} {}
    explicit Result(const T &v) noexcept : value_{v}, ec_{} {}
    explicit Result(T &&v) noexcept : value_{std::move(v)}, ec_{} {}

    bool HasError() const noexcept { return static_cast<bool>(ec_); }
    std::error_code Error() const noexcept { return ec_; }

    const T &Value() const noexcept { return value_; }
    T &Value() noexcept { return value_; }

private:
    T value_;
    std::error_code ec_;
};

// Specialization for void to keep usage uniform.
template <>
class Result<void> {
public:
    Result() noexcept : ec_{} {}
    explicit Result(std::error_code ec) noexcept : ec_{ec} {}

    bool HasError() const noexcept { return static_cast<bool>(ec_); }
    std::error_code Error() const noexcept { return ec_; }

private:
    std::error_code ec_;
};

// Minimal Future<T> placeholder. No functionality — present so headers compile.
template <typename T>
class Future {
public:
    Future() noexcept = default;
};

} // namespace core
} // namespace ara

#endif // ARA_CORE_RESULT_FUTURE_H_