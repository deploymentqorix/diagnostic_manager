#include "dtc/dm_dtc.h"

#include "ara/core/result_future.h"
#include <unordered_map>
#include <mutex>
#include <system_error>
#include <utility>

namespace diagnostic_manager {
namespace dtc {

struct DtcInstance {
    UdsStatusByte status{0};
    bool hasStatus{false};
    bool suppression{false};
    DtcStatusNotifier notifier;
};

static std::unordered_map<DtcId, DtcInstance> g_dtcs;
static std::mutex g_dtcsMutex;

ara::core::Result<void> DMDtc::RegisterDtc(DtcId dtc, DtcStatusNotifier notifier) {
    std::lock_guard<std::mutex> lk(g_dtcsMutex);
    if (g_dtcs.find(dtc) != g_dtcs.end()) {
        return ara::core::Result<void>{ std::make_error_code(std::errc::file_exists) };
    }
    DtcInstance inst;
    inst.notifier = std::move(notifier);
    g_dtcs.emplace(dtc, std::move(inst));
    return ara::core::Result<void>{};
}

ara::core::Result<void> DMDtc::UnregisterDtc(DtcId dtc) {
    std::lock_guard<std::mutex> lk(g_dtcsMutex);
    auto it = g_dtcs.find(dtc);
    if (it == g_dtcs.end()) return ara::core::Result<void>{ std::make_error_code(std::errc::no_such_file_or_directory) };
    g_dtcs.erase(it);
    return ara::core::Result<void>{};
}

ara::core::Result<void> DMDtc::ReportDtcStatus(DtcId dtc, UdsStatusByte udsStatus) {
    DtcStatusNotifier notifierCopy;
    UdsStatusByte oldStatus = 0;
    bool shouldNotify = false;
    bool suppressed = false;

    {
        std::lock_guard<std::mutex> lk(g_dtcsMutex);
        auto it = g_dtcs.find(dtc);
        if (it == g_dtcs.end()) return ara::core::Result<void>{ std::make_error_code(std::errc::no_such_file_or_directory) };

        DtcInstance &inst = it->second;
        oldStatus = inst.hasStatus ? inst.status : 0;

        // Update stored status
        if (!inst.hasStatus || inst.status != udsStatus) {
            inst.status = udsStatus;
            inst.hasStatus = true;
            suppressed = inst.suppression;
            notifierCopy = inst.notifier;
            shouldNotify = !suppressed && static_cast<bool>(notifierCopy);
        } else {
            // no change -> nothing to do
            return ara::core::Result<void>{};
        }
    }

    // Notify outside lock if not suppressed
    if (shouldNotify && notifierCopy) {
        notifierCopy(dtc, oldStatus, udsStatus);
    }

    return ara::core::Result<void>{};
}

std::optional<UdsStatusByte> DMDtc::GetCurrentStatus(DtcId dtc) {
    std::lock_guard<std::mutex> lk(g_dtcsMutex);
    auto it = g_dtcs.find(dtc);
    if (it == g_dtcs.end()) return std::nullopt;
    if (!it->second.hasStatus) return std::nullopt;
    return it->second.status;
}

ara::core::Result<void> DMDtc::SetDtcSuppression(DtcId dtc, bool suppressed) {
    std::lock_guard<std::mutex> lk(g_dtcsMutex);
    auto it = g_dtcs.find(dtc);
    if (it == g_dtcs.end()) return ara::core::Result<void>{ std::make_error_code(std::errc::no_such_file_or_directory) };
    it->second.suppression = suppressed;
    return ara::core::Result<void>{};
}

std::optional<bool> DMDtc::GetDtcSuppression(DtcId dtc) {
    std::lock_guard<std::mutex> lk(g_dtcsMutex);
    auto it = g_dtcs.find(dtc);
    if (it == g_dtcs.end()) return std::nullopt;
    return it->second.suppression;
}

ara::core::Result<void> DMDtc::SetDtcStatusNotifier(DtcId dtc, DtcStatusNotifier notifier) {
    std::lock_guard<std::mutex> lk(g_dtcsMutex);
    auto it = g_dtcs.find(dtc);
    if (it == g_dtcs.end()) return ara::core::Result<void>{ std::make_error_code(std::errc::no_such_file_or_directory) };
    it->second.notifier = std::move(notifier);
    return ara::core::Result<void>{};
}

} // namespace dtc
} // namespace diagnostic_manager