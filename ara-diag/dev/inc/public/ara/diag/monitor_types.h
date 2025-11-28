#ifndef ARA_DIAG_MONITOR_TYPES_H_
#define ARA_DIAG_MONITOR_TYPES_H_

#include <cstdint>
#include <optional> // <-- added

namespace ara {
namespace diag {

// Counter-based debouncing parameters
struct CounterBased {
    std::int16_t failedThreshold{0};    // Threshold until qualified failed
    std::int16_t passedThreshold{0};    // Threshold until qualified passed
    std::uint16_t failedStepsize{0};    // Stepsize per pre-failed report
    std::uint16_t passedStepsize{0};    // Stepsize per pre-passed report
    std::int16_t failedJumpValue{0};    // failed to jump value
    std::int16_t passedJumpValue{0};    // passed to jump value
    bool useJumpToFailed{false};        // is jump to failed supported
    bool useJumpToPassed{false};        // is jump to passed supported
};

// Time-based debouncing parameters
struct TimeBased {
    std::uint32_t failedMs{0}; // time until failed in ms
    std::uint32_t passedMs{0}; // time until passed in ms
};

// Reasons for monitor initialization
enum class InitMonitorReason : std::uint32_t {
    kClear      = 0x00, // Event was cleared and all internal values and states are reset
    kRestart    = 0x01, // Operation cycle of the event was (re-)started
    kReenabled  = 0x02, // Enable conditions are fulfilled and control DTC setting is set to on
    kDisabled   = 0x03  // Enable conditions no longer fulfilled, or Control DTC setting is set to off
};

// Monitor actions reported by AAs
enum class MonitorAction : std::uint32_t {
    kPassed               = 0x00, // Monitor reports qualified test result passed
    kFailed               = 0x01, // Monitor reports qualified test result failed
    kPrepassed            = 0x02, // Monitor reports unqualified test result pre-passed
    kPrefailed            = 0x03, // Monitor reports unqualified test result pre-failed
    kFdcThresholdReached  = 0x04, // Monitor triggers storage of ExtendedDataRecords / Freeze Frames
    kResetTestFailed      = 0x05, // Reset TestFailed Bit without other side effects
    kFreezeDebouncing     = 0x06, // Freeze the internal debounce counter/timer
    kResetDebouncing      = 0x07  // Reset the internal debounce counter/timer
};

// New: mapping of AUTOSAR DiagEventDebounceCounterBased metaclass.
// Fields are optional because the meta-attributes are 0..1 in the model.
struct DiagEventDebounceCounterBased {
    // Threshold to allocate an event memory entry and to capture the Freeze Frame.
    std::optional<std::int16_t> fdcThreshold;

    // Steps to decrement the internal debounce counter (counter decrement step).
    std::optional<std::uint16_t> decrementStep;

    // Event-specific limit that indicates the "failed" counter status.
    std::optional<std::int16_t> counterFailedThreshold;

    // Steps to increment the internal debounce counter (counter increment step).
    std::optional<std::uint16_t> incrementStep;

    // Activates/deactivates counter jump-down behaviour.
    std::optional<bool> counterJumpDown;

    // Initial value of the internal debounce counter when jumping down.
    std::optional<std::int16_t> counterJumpDownValue;

    // Activates/deactivates counter jump-up behaviour.
    std::optional<bool> counterJumpUp;

    // Initial value of the internal debounce counter when jumping up.
    std::optional<std::int16_t> counterJumpUpValue;

    // Event-specific limit that indicates the "passed" counter status.
    std::optional<std::int16_t> counterPassedThreshold;
};

// New: mapping of AUTOSAR DiagEventDebounceTimeBased metaclass.
// Time values are represented as milliseconds (std::uint32_t). Attributes are optional (0..1).
struct DiagEventDebounceTimeBased {
    // Threshold to allocate an event memory entry and to capture the Freeze Frame.
    std::optional<std::uint32_t> timeBasedFdcThresholdMs;

    // Event-specific delay indicating the "failed" status (ms).
    std::optional<std::uint32_t> timeFailedThresholdMs;

    // Event-specific delay indicating the "passed" status (ms).
    std::optional<std::uint32_t> timePassedThresholdMs;
};

// New: abstract diagnostic data identifier base (attributes optional per model).
struct DiagnosticAbstractDataIdentifier {
    // Numerical identifier used to identify the DID in the diagnostic workflow.
    std::optional<std::uint32_t> id;

    virtual ~DiagnosticAbstractDataIdentifier() = default;
};

} // namespace diag
} // namespace ara

#endif // ARA_DIAG_MONITOR_TYPES_H_