#ifndef DM_DTC_H
#define DM_DTC_H

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include "ara/core/result_future.h"

namespace diagnostic_manager {
namespace dtc {

using UdsStatusByte = std::uint8_t;
using DtcId = std::uint32_t;
using DtcStatusNotifier = std::function<void(DtcId, UdsStatusByte, UdsStatusByte)>;

class DMDtc {
public:
    static ara::core::Result<void> RegisterDtc(DtcId dtc, DtcStatusNotifier notifier = nullptr);
    static ara::core::Result<void> UnregisterDtc(DtcId dtc);
    static ara::core::Result<void> ReportDtcStatus(DtcId dtc, UdsStatusByte udsStatus);

    static std::optional<UdsStatusByte> GetCurrentStatus(DtcId dtc);

    static ara::core::Result<void> SetDtcSuppression(DtcId dtc, bool suppressed);
    static std::optional<bool> GetDtcSuppression(DtcId dtc);

    static ara::core::Result<void> SetDtcStatusNotifier(DtcId dtc, DtcStatusNotifier notifier);
};

} // namespace dtc
} // namespace diagnostic_manager

#endif // DM_DTC_H