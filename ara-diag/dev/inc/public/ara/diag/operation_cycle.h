#ifndef ARA_DIAG_OPERATION_CYCLE_H_
#define ARA_DIAG_OPERATION_CYCLE_H_

#include <functional>
#include <mutex>

namespace ara {
namespace core {
class InstanceSpecifier;
template <typename T> class Result;
} // namespace core

namespace diag {

class OperationCycle {
public:
    explicit OperationCycle(const ara::core::InstanceSpecifier &specifier);

    // single non-copyable / non-movable instance
    OperationCycle(OperationCycle &&) noexcept = delete;
    OperationCycle(OperationCycle &) = delete;
    OperationCycle &operator=(OperationCycle &&) = delete;
    OperationCycle &operator=(OperationCycle &) = delete;

    ~OperationCycle() noexcept = default;

    // Register a notifier called when operation cycle changes.
    ara::core::Result<void> SetNotifier(std::function<void()> notifier);

    // Set the operation cycle state (e.g. called by diagnostic-manager)
    ara::core::Result<void> SetOperationCycleState(bool active);

    // Get the current operation cycle state
    ara::core::Result<bool> GetOperationCycleState() const;

private:
    const ara::core::InstanceSpecifier *specifierPtr_{nullptr};
    std::function<void()> notifier_;
    bool active_{false};
    mutable std::mutex mutex_;
};

} // namespace diag
} // namespace ara

#endif // ARA_DIAG_OPERATION_CYCLE_H_