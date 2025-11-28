#include <iostream>
#include "ara-diag/dev/inc/public/ara/diag/event_types.h"

// include a DM header to ensure compilation of project sources
#include "event/dm_event.h"
#include "dtc/dm_dtc.h"

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    std::cout << "diagnostic-manager binary started\n";

    diagnostic_manager::event::DebounceConfig cfg;
    diagnostic_manager::event::MonitorId mid = "dummy_monitor";
    diagnostic_manager::event::QualifiedNotifier qn = [](const diagnostic_manager::event::MonitorId &id,
                                                         diagnostic_manager::event::QualifiedState s) {
        (void)id; (void)s;
        std::cout << "Qualified notifier fired for " << id << "\n";
    };
    diagnostic_manager::event::DMEvent::RegisterMonitor(mid, cfg, qn);

    diagnostic_manager::dtc::DMDtc::RegisterDtc(0x1234, [](diagnostic_manager::dtc::DtcId id,
                                                          diagnostic_manager::dtc::UdsStatusByte /*oldS*/,
                                                          diagnostic_manager::dtc::UdsStatusByte newS) {
        std::cout << "DTC " << id << " status changed to " << int(newS) << "\n";
    });

    std::cout << __func__ << ":" << __LINE__ << std::endl;
    ara::diag::EventStatusByte status(ara::diag::EventStatusBit::FailedAndTested,
                                      ara::diag::EventStatusBit::PassedAndTested);

    status.IsFailedAndTested();
    status.IsPassedAndTested();
    status.IsSet(ara::diag::EventStatusBit::FailedAndTested);
    status.IsNotSet(ara::diag::EventStatusBit::PassedAndTested);

    std::cout << "diagnostic-manager binary ready\n";
    return 0;
}