#include "ara/diag/monitor.h"
#include "ara/core/result_future.h"
#include "ara/core/instance_specifier.h"
#include <system_error>

namespace ara {
namespace diag {

Monitor::Monitor(const ara::core::InstanceSpecifier &specifier,
                 std::function<void(InitMonitorReason)> initMonitor,
                 std::function<std::int8_t()> getFaultDetectionCounter)
    : specifierPtr_(&specifier),
      initMonitor_(std::move(initMonitor)),
      getFDC_(std::move(getFaultDetectionCounter)),
      offered_(false) {
    // standalone stub: no backend integration
}

Monitor::Monitor(const ara::core::InstanceSpecifier &specifier,
                 std::function<void(InitMonitorReason)> initMonitor,
                 CounterBased defaultValues)
    : specifierPtr_(&specifier),
      initMonitor_(std::move(initMonitor)),
      counterDefaults_(defaultValues),
      offered_(false) {
    // standalone stub
}

Monitor::Monitor(const ara::core::InstanceSpecifier &specifier,
                 std::function<void(InitMonitorReason)> initMonitor,
                 TimeBased defaultValues)
    : specifierPtr_(&specifier),
      initMonitor_(std::move(initMonitor)),
      timeDefaults_(defaultValues),
      offered_(false) {
    // standalone stub
}

void Monitor::ReportMonitorAction(MonitorAction /*action*/) {
    // No backend available in standalone build: ignore
}

ara::core::Result<void> Monitor::Offer() {
    // Standalone library: indicate operation not supported to callers.
    // Real diagnostic-manager would set offered_ and return success or
    // Diag-specific errors (e.g. OfferErrc::kAlreadyOffered).
    return ara::core::Result<void>{ std::make_error_code(std::errc::operation_not_supported) };
}

void Monitor::StopOffer() {
    // Standalone stub: nothing to stop
    offered_ = false;
}

} // namespace diag
} // namespace ara