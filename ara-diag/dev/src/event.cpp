#include "ara/diag/event.h"
#include "ara/core/result_future.h"
#include "ara/core/instance_specifier.h"
#include <system_error>


namespace ara {
namespace diag {

// Minimal standalone stubs for the API so this library can build without a full diagnostic-manager backend.
// Integrations can replace these with richer implementations that use the real ara::core::Future/Result APIs.

Event::Event(const ara::core::InstanceSpecifier &specifier)
    : specifierPtr_(&specifier) {
    // minimal standalone implementation: store pointer to specifier
    // Diagnostic-manager integration can enrich behavior in the real backend.
}

ara::core::Future<EventStatusByte> Event::GetEventStatus() {
    return ara::core::Future<EventStatusByte>{};
}

ara::core::Result<void> Event::SetEventStatusChangedNotifier(std::function<void(EventStatusByte)> /*notifier*/) {
    return ara::core::Result<void>{ std::make_error_code(std::errc::operation_not_supported) };
}

} // namespace diag
} // namespace ara