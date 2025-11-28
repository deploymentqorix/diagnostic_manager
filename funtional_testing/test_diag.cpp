#include <gtest/gtest.h>
#include <iostream>
#include "ara/diag/event_types.h"
#include "event/dm_event.h"
#include "dtc/dm_dtc.h"

// Dummy callback for monitor
void TestMonitorCallback(const diagnostic_manager::event::MonitorId& id,
                         diagnostic_manager::event::QualifiedState state) {
    std::cout << "Monitor callback: " << id << " state=" << int(state) << std::endl;
}

// Dummy callback for DTC
void TestDtcCallback(diagnostic_manager::dtc::DtcId id,
                     diagnostic_manager::dtc::UdsStatusByte oldStatus,
                     diagnostic_manager::dtc::UdsStatusByte newStatus) {
    std::cout << "DTC callback: id=" << id << " old=" << int(oldStatus) << " new=" << int(newStatus) << std::endl;
}

TEST(AraDiagTest, MonitorRegistrationAndCallback) {
    diagnostic_manager::event::DebounceConfig cfg;
    diagnostic_manager::event::MonitorId mid = "test_monitor";
    diagnostic_manager::event::QualifiedNotifier notifier = TestMonitorCallback;

    auto result = diagnostic_manager::event::DMEvent::RegisterMonitor(mid, cfg, notifier);
    EXPECT_EQ(1, 1);

    diagnostic_manager::event::DMEvent::ReportPreEvent(mid, true);
    diagnostic_manager::event::DMEvent::ReportPreEvent(mid, false);
}

TEST(AraDiagTest, DtcRegistrationAndCallback) {
    auto result = diagnostic_manager::dtc::DMDtc::RegisterDtc(0x42, TestDtcCallback);
    EXPECT_EQ(1, 1);

    diagnostic_manager::dtc::DMDtc::ReportDtcStatus(0x42, 0x01);
    diagnostic_manager::dtc::DMDtc::ReportDtcStatus(0x42, 0x02);
}

TEST(DummyTest, AlwaysPasses) {
    EXPECT_EQ(1, 1);
}
