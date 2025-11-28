/*
 * Diagnostic Manager - Event interface
 * Main logic and notifier handling lives in diagnostic-manager.
 */
#ifndef DM_EVENT_H
#define DM_EVENT_H

#include <functional>
#include <optional>
#include <system_error>
#include <cstdint>
#include <string>
#include "ara/core/result_future.h"

namespace diagnostic_manager {
namespace event {

enum class DebounceMode {
    CounterBased,
    TimeBased
};

enum class QualifiedState : std::uint8_t {
    Unqualified = 0,
    QualifiedPassed,
    QualifiedFailed
};

struct DebounceConfig {
    DebounceMode mode{DebounceMode::CounterBased};

    // Counter-based defaults
    std::int32_t failedThreshold{3};   // qualify failed after this many prefailed reports
    std::int32_t passedThreshold{3};   // qualify passed after this many prepassed reports
    std::int32_t failedStep{1};        // step per prefailed report
    std::int32_t passedStep{1};        // step per prepassed report

    // Time-based defaults (milliseconds)
    std::uint32_t timeThresholdMs{12000}; // qualify if pre state holds continuously for this duration
};

using MonitorId = std::string;
using QualifiedNotifier = std::function<void(const MonitorId &, QualifiedState)>;

// Public DMEvent API
class DMEvent {
public:
    static std::optional<bool> GetEventMemoryOverflow();
    static ara::core::Result<void> SetEventMemoryOverflowNotifier(std::function<void(bool)> notifier);

    static ara::core::Result<void> RegisterMonitor(const MonitorId &id, DebounceConfig cfg, QualifiedNotifier notifier);
    static ara::core::Result<void> UnregisterMonitor(const MonitorId &id);
    static ara::core::Result<void> ReportPreEvent(const MonitorId &id, bool preFailed);

    static std::optional<QualifiedState> GetQualifiedState(const MonitorId &id);

    static ara::core::Result<void> SetQualifiedState(const MonitorId &id, QualifiedState state);
    static ara::core::Result<void> FreezeDebouncing(const MonitorId &id);
    static ara::core::Result<void> ResetDebouncing(const MonitorId &id);
    static ara::core::Result<void> TriggerFdcThresholdReached(const MonitorId &id);
    static ara::core::Result<void> ResetTestFailed(const MonitorId &id);
};

}  // namespace event
}  // namespace diagnostic_manager

#endif // DM_EVENT_H
