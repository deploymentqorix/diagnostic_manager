#ifndef ARA_DIAG_CONDITION_H_
#define ARA_DIAG_CONDITION_H_

#include <functional>
#include <cstdint> // <-- add this to make std::uint8_t available

namespace ara {
namespace core {
class InstanceSpecifier;
template <typename T> class Future;
template <typename T> class Result;
} // namespace core

namespace diag {

enum class ConditionType : std::uint8_t {
    kConditionFalse = 0x00,
    kConditionTrue  = 0x01
};

class Condition {
public:
    explicit Condition(const ara::core::InstanceSpecifier &specifier);

    Condition(Condition &&) noexcept = delete;
    Condition(Condition &) = delete;
    Condition &operator=(Condition &&) = delete;
    Condition &operator=(Condition &) = delete;

    ~Condition() noexcept = default;

    // Returns the current condition
    ara::core::Future<ConditionType> GetCondition();

    // Set condition (standalone stub may return not supported)
    ara::core::Result<void> SetCondition(ConditionType condition);

private:
    const ara::core::InstanceSpecifier *specifierPtr_{nullptr};
    std::function<void()> notifier_;
};

} // namespace diag
} // namespace ara

#endif // ARA_DIAG_CONDITION_H_