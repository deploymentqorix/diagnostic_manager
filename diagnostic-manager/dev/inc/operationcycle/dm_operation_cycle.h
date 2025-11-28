#ifndef DM_OPERATION_CYCLE_H
#define DM_OPERATION_CYCLE_H

#include <functional>
#include <optional>
#include <string>
#include "ara/core/result_future.h"

namespace diagnostic_manager {
namespace operation_cycle {

// Monitor identifier (use InstanceSpecifier::GetName() from ara-diag)
using OpCycleId = std::string;

// Notifier called when op-cycle state changes: (id, active)
using OpCycleNotifier = std::function<void(const OpCycleId &, bool)>;

class DMOperationCycle {
public:
    // Register an operation cycle instance with optional notifier.
    static ara::core::Result<void> RegisterOperationCycle(const OpCycleId &id, OpCycleNotifier notifier = nullptr);

    // Unregister previously registered operation cycle instance.
    static ara::core::Result<void> UnregisterOperationCycle(const OpCycleId &id);

    // Set the operation cycle state (active = true/false). Calls notifier if state changed.
    static ara::core::Result<void> SetOperationCycleState(const OpCycleId &id, bool active);

    // Get current operation cycle state.
    static ara::core::Result<bool> GetOperationCycleState(const OpCycleId &id);

    // Set or replace notifier for an already registered operation cycle.
    static ara::core::Result<void> SetOpCycleNotifier(const OpCycleId &id, OpCycleNotifier notifier);
};

} // namespace operation_cycle
} // namespace diagnostic_manager

#endif // DM_OPERATION_CYCLE_H