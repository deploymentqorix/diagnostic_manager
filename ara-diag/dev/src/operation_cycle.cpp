#include "ara/diag/operation_cycle.h"
#include "ara/core/result_future.h"
#include "ara/core/instance_specifier.h"
#include <system_error>
#include <utility>

namespace ara {
namespace diag {

OperationCycle::OperationCycle(const ara::core::InstanceSpecifier &specifier)
    : specifierPtr_(&specifier) {}

ara::core::Result<void> OperationCycle::SetNotifier(std::function<void()> notifier) {
    std::lock_guard<std::mutex> lk(mutex_);
    notifier_ = std::move(notifier);
    return ara::core::Result<void>{};
}

ara::core::Result<void> OperationCycle::SetOperationCycleState(bool active) {
    std::function<void()> cb;
    {
        std::lock_guard<std::mutex> lk(mutex_);
        // if state unchanged, nothing to do
        if (active_ == active) return ara::core::Result<void>{};
        active_ = active;
        cb = notifier_;
    }
    // call notifier outside lock
    if (cb) cb();
    return ara::core::Result<void>{};
}

ara::core::Result<bool> OperationCycle::GetOperationCycleState() const {
    std::lock_guard<std::mutex> lk(mutex_);
    return ara::core::Result<bool>{ active_ };
}

} // namespace diag
} // namespace ara