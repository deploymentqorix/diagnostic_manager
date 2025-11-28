#include "operationcycle/dm_operation_cycle.h"
#include <unordered_map>
#include <mutex>

namespace diagnostic_manager {
namespace operation_cycle {

struct OpCycleInstance {
    bool active{false};
    bool initialized{false};
    OpCycleNotifier notifier;
};

static std::unordered_map<OpCycleId, OpCycleInstance> g_opCycles;
static std::mutex g_opCyclesMutex;

ara::core::Result<void> DMOperationCycle::RegisterOperationCycle(const OpCycleId &id, OpCycleNotifier notifier) {
    std::lock_guard<std::mutex> lk(g_opCyclesMutex);
    if (g_opCycles.find(id) != g_opCycles.end()) {
        return ara::core::Result<void>{ std::make_error_code(std::errc::file_exists) };
    }
    OpCycleInstance inst;
    inst.notifier = std::move(notifier);
    inst.active = false;
    inst.initialized = true;
    g_opCycles.emplace(id, std::move(inst));
    return ara::core::Result<void>{};
}

ara::core::Result<void> DMOperationCycle::UnregisterOperationCycle(const OpCycleId &id) {
    std::lock_guard<std::mutex> lk(g_opCyclesMutex);
    auto it = g_opCycles.find(id);
    if (it == g_opCycles.end()) return ara::core::Result<void>{ std::make_error_code(std::errc::no_such_file_or_directory) };
    g_opCycles.erase(it);
    return ara::core::Result<void>{};
}

ara::core::Result<void> DMOperationCycle::SetOperationCycleState(const OpCycleId &id, bool active) {
    OpCycleNotifier notifierCopy;
    bool changed = false;
    {
        std::lock_guard<std::mutex> lk(g_opCyclesMutex);
        auto it = g_opCycles.find(id);
        if (it == g_opCycles.end()) return ara::core::Result<void>{ std::make_error_code(std::errc::no_such_file_or_directory) };
        OpCycleInstance &inst = it->second;
        if (!inst.initialized) inst.initialized = true;
        if (inst.active != active) {
            inst.active = active;
            notifierCopy = inst.notifier;
            changed = true;
        }
    }

    if (changed && notifierCopy) {
        notifierCopy(id, active);
    }
    return ara::core::Result<void>{};
}

ara::core::Result<bool> DMOperationCycle::GetOperationCycleState(const OpCycleId &id) {
    std::lock_guard<std::mutex> lk(g_opCyclesMutex);
    auto it = g_opCycles.find(id);
    if (it == g_opCycles.end()) return ara::core::Result<bool>{ std::make_error_code(std::errc::no_such_file_or_directory) };
    return ara::core::Result<bool>{ it->second.active };
}

ara::core::Result<void> DMOperationCycle::SetOpCycleNotifier(const OpCycleId &id, OpCycleNotifier notifier) {
    std::lock_guard<std::mutex> lk(g_opCyclesMutex);
    auto it = g_opCycles.find(id);
    if (it == g_opCycles.end()) return ara::core::Result<void>{ std::make_error_code(std::errc::no_such_file_or_directory) };
    it->second.notifier = std::move(notifier);
    return ara::core::Result<void>{};
}

} // namespace operation_cycle
} // namespace diagnostic_manager