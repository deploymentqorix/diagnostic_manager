#ifndef ARA_DIAG_MONITOR_H_
#define ARA_DIAG_MONITOR_H_

#include "monitor_types.h"
#include <functional>
#include <cstdint>

namespace ara {
namespace core {
class InstanceSpecifier; // forward declaration
template <typename T> class Result;
} // namespace core

namespace diag {

class Monitor final {
public:
    // Monitor constructors (monitor-internal, counter-based, time-based)
    Monitor(const ara::core::InstanceSpecifier &specifier,
            std::function<void(InitMonitorReason)> initMonitor,
            std::function<std::int8_t()> getFaultDetectionCounter);

    Monitor(const ara::core::InstanceSpecifier &specifier,
            std::function<void(InitMonitorReason)> initMonitor,
            CounterBased defaultValues);

    Monitor(const ara::core::InstanceSpecifier &specifier,
            std::function<void(InitMonitorReason)> initMonitor,
            TimeBased defaultValues);

    // Single non-copyable / non-movable instance
    Monitor(Monitor &&) noexcept = delete;
    Monitor(Monitor &) = delete;
    Monitor &operator=(Monitor &&) = delete;
    Monitor &operator=(Monitor &) = delete;

    ~Monitor() noexcept = default;

    // Report monitor action
    void ReportMonitorAction(MonitorAction action);

    // Offer / StopOffer
    ara::core::Result<void> Offer();
    void StopOffer();

private:
    const ara::core::InstanceSpecifier *specifierPtr_;
    std::function<void(InitMonitorReason)> initMonitor_;
    std::function<std::int8_t()> getFDC_;
    CounterBased counterDefaults_{};
    TimeBased timeDefaults_{};
    bool offered_{false};
};

} // namespace diag
} // namespace ara

#endif // ARA_DIAG_MONITOR_H_