/*
 * DTC information API declarations
 * SWS_DM_00919, SWS_DM_00918 (draft)
 */
#ifndef ARA_DIAG_DTC_INFORMATION_H_
#define ARA_DIAG_DTC_INFORMATION_H_

#include <cstdint>
#include <functional>
#include <system_error>

namespace ara {
namespace core {
class InstanceSpecifier;
template <typename T> class Future;
template <typename T> class Result;
} // namespace core

namespace diag {

class DTCInformation {
public:
    enum class ControlDtcStatusType : std::uint8_t {
        kDTCSettingOn = 0x00,
        kDTCSettingOff = 0x01
    };

    enum class UdsDtcStatusBitType : std::uint8_t {
        kTestFailed                       = 0x01,
        kTestFailedThisOperationCycle     = 0x02,
        kPendingDTC                       = 0x04,
        kConfirmedDTC                     = 0x08,
        kTestNotCompletedSinceLastClear   = 0x10,
        kTestFailedSinceLastClear         = 0x20,
        kTestNotCompletedThisOperationCycle = 0x40,
        kWarningIndicatorRequested        = 0x80
    };

    struct UdsDtcStatusByteType {
        std::uint8_t value{0};

        bool IsSet(UdsDtcStatusBitType bit) const noexcept {
            return (value & static_cast<std::uint8_t>(bit)) != 0;
        }
    };

    struct SnapshotDataIdentifierType {
        std::uint32_t id{0};
    };

    struct SnapshotDataRecordType {
        // Minimal placeholder for standalone build
        std::uint32_t length{0};
        const std::uint8_t *data{nullptr};
    };

    struct SnapshotRecordUpdatedType {
        SnapshotDataIdentifierType identifier;
        SnapshotDataRecordType record;
    };

    enum class DtcSuppressionType : std::uint8_t {
        kDTCSuppressionOff = 0x00,
        kDTCSuppressionOn  = 0x01
    };

    explicit DTCInformation(const ara::core::InstanceSpecifier &specifier);

    DTCInformation(DTCInformation &&) noexcept = delete;
    DTCInformation(DTCInformation &) = delete;
    DTCInformation &operator=(DTCInformation &&) = delete;
    DTCInformation &operator=(DTCInformation &) = delete;

    ~DTCInformation() noexcept = default;

    // Returns the current UDS DTC status byte of the given DTC identifier.
    ara::core::Future<UdsDtcStatusByteType> GetCurrentStatus(std::uint32_t dtc);

    // Contains the current status of DTC Suppression.
    ara::core::Result<DtcSuppressionType> GetDtcSuppression(std::uint32_t dtc);

    // Register a notifier function which is called if a DTC status has changed.
    ara::core::Result<void> SetDTCStatusChangedNotifier(
        std::function<void(std::uint32_t dtc,
                           UdsDtcStatusByteType udsStatusByteOld,
                           UdsDtcStatusByteType udsStatusByteNew)> notifier);

    // Set the status of DTC Suppression.
    ara::core::Result<void> SetDtcSuppression(std::uint32_t dtc, DtcSuppressionType suppressionStatus);

    // Register a notifier for snapshot record updates.
    ara::core::Result<void> SetSnapshotRecordUpdatedNotifier(
        std::function<void(SnapshotRecordUpdatedType)> notifier);

    // Event memory overflow APIs (per AUTOSAR SWS)
    ara::core::Result<bool> GetEventMemoryOverflow();
    ara::core::Result<void> SetEventMemoryOverflowNotifier(std::function<void(bool)> notifier);

private:
    const ara::core::InstanceSpecifier *specifierPtr_{nullptr};
};

} // namespace diag
} // namespace ara

#endif // ARA_DIAG_DTC_INFORMATION_H_
