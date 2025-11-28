#include "ara/diag/condition.h"
#include "ara/core/result_future.h"
#include "ara/core/instance_specifier.h"
#include <system_error>

namespace ara {
namespace diag {

Condition::Condition(const ara::core::InstanceSpecifier &specifier)
    : specifierPtr_(&specifier) {}

// Standalone: no diagnostic-manager backend available. Return not-supported
// for operations that would require backend interaction.
ara::core::Future<ConditionType> Condition::GetCondition() {
    return ara::core::Future<ConditionType>{};
}

ara::core::Result<void> Condition::SetCondition(ConditionType /*condition*/) {
    return ara::core::Result<void>{ std::make_error_code(std::errc::operation_not_supported) };
}

} // namespace diag
} // namespace ara