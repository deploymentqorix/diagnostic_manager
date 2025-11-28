#ifndef ARA_DIAG_EVENT_H_
#define ARA_DIAG_EVENT_H_

#include "event_types.h"
#include <functional>

namespace ara {
namespace core {
class InstanceSpecifier; // forward declaration; real definition is expected in the project
template <typename T> class Future;    // forward declare core types used by the API
template <typename T> class Result;
} // namespace core

namespace diag {

class Event {
public:
    explicit Event(const ara::core::InstanceSpecifier &specifier);

    // Event shall be a single non-copyable/non-movable instance
    Event(Event &&) noexcept = delete;
    Event(Event &) = delete;
    Event &operator=(Event &&) = delete;
    Event &operator=(Event &) = delete;

    ~Event() noexcept = default;

    // Returns the current diagnostic event status.
    ara::core::Future<EventStatusByte> GetEventStatus();

    // Register a notifier function which is called if a diagnostic event is changed.
    ara::core::Result<void> SetEventStatusChangedNotifier(std::function<void(EventStatusByte)> notifier);

private:
    const ara::core::InstanceSpecifier *specifierPtr_;
};

} // namespace diag
} // namespace ara

#endif // ARA_DIAG_EVENT_H_