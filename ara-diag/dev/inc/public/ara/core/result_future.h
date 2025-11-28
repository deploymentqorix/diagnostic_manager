#ifndef ARA_CORE_RESULT_FUTURE_H_
#define ARA_CORE_RESULT_FUTURE_H_

#include <system_error>
#include <utility>

namespace ara {
namespace core {

// Minimal Result<T> and Result<void> stubs (header-only).
template <typename T>
class Result {
public:
    Result() noexcept : ok_(true) {}
    explicit Result(const std::error_code &ec) noexcept : ok_(false), error_(ec) {}
    explicit Result(T value) noexcept : ok_(true), value_(std::move(value)) {}

    bool IsOk() const noexcept { return ok_; }
    const std::error_code &Error() const noexcept { return error_; }
    const T &Value() const & { return value_; }
    T &&Value() && { return std::move(value_); }

private:
    bool ok_{false};
    T value_{};
    std::error_code error_;
};

template <>
class Result<void> {
public:
    Result() noexcept : ok_(true) {}
    explicit Result(const std::error_code &ec) noexcept : ok_(false), error_(ec) {}

    bool IsOk() const noexcept { return ok_; }
    const std::error_code &Error() const noexcept { return error_; }

private:
    bool ok_{false};
    std::error_code error_;
};

// Minimal Future<T> stub.
template <typename T>
class Future {
public:
    Future() noexcept = default;
    // Add more functionality as needed by real implementation.
};

} // namespace core
} // namespace ara

#endif // ARA_CORE_RESULT_FUTURE_H_