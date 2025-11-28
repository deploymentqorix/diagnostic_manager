#include "ara/diag/dtc_information.h"
#include "ara/core/result_future.h"
#include "ara/core/instance_specifier.h"
#include <system_error>


namespace ara {
namespace diag {

// Standalone implementation: no diagnostic-manager backend available.
// Return "operation not supported" to callers so they can detect
// the absence of a backend. Replace with real behavior when integrated.

DTCInformation::DTCInformation(const ara::core::InstanceSpecifier &specifier)
    : specifierPtr_(&specifier) {}

ara::core::Result<bool> DTCInformation::GetEventMemoryOverflow() {
    return ara::core::Result<bool>{ std::make_error_code(std::errc::operation_not_supported) };
}

ara::core::Result<void> DTCInformation::SetEventMemoryOverflowNotifier(std::function<void(bool)> /*notifier*/) {
    return ara::core::Result<void>{ std::make_error_code(std::errc::operation_not_supported) };
}

} // namespace diag
} // namespace ara
