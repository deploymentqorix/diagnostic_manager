#ifndef DM_EVENT_H
#define DM_EVENT_H

#include <functional>
#include <optional>
#include <system_error>
#include <cstdint>
#include <string>
#include "event/dm_event.h"
#include "ara/core/result_future.h"
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>

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

// Monitor identifier type (use InstanceSpecifier::GetName() on ara-diag side)
using MonitorId = std::string;

// Notifier type called when a monitor becomes qualified (or changes qualified state).
// first arg: monitor id, second arg: new qualified state
using QualifiedNotifier = std::function<void(const MonitorId &, QualifiedState)>;

// Public DMEvent API
class DMEvent {
public:
    // Existing APIs (event memory overflow)
    static std::optional<bool> GetEventMemoryOverflow();
    static std::error_code SetEventMemoryOverflowNotifier(std::function<void(bool)> notifier);

    // Register a monitor with debounce configuration and a notifier.
    // Returns error if monitor id already registered.
    static std::error_code RegisterMonitor(const MonitorId &id, DebounceConfig cfg, QualifiedNotifier notifier);

    // Unregister previously registered monitor.
    static std::error_code UnregisterMonitor(const MonitorId &id);

    // Called by ara-diag when it reports an unqualified test result (prepassed / prefailed)
    // preFailed == true => pre-failed reported; preFailed == false => pre-passed reported
    // Returns error if monitor id unknown.
    static std::error_code ReportPreEvent(const MonitorId &id, bool preFailed);

    // Query current qualified state (if registered)
    static std::optional<QualifiedState> GetQualifiedState(const MonitorId &id);

    // Additional control operations driven by MonitorAction commands
    static std::error_code SetQualifiedState(const MonitorId &id, QualifiedState state);
    static std::error_code FreezeDebouncing(const MonitorId &id);
    static std::error_code ResetDebouncing(const MonitorId &id);
    static std::error_code TriggerFdcThresholdReached(const MonitorId &id);
    static std::error_code ResetTestFailed(const MonitorId &id);
};

using namespace std::chrono;

struct MonitorInstance {
    DebounceConfig cfg;
    QualifiedNotifier notifier;
    int counter{0};
    QualifiedState qualified{QualifiedState::Unqualified};
    std::optional<steady_clock::time_point> preStartTime;
    std::optional<bool> lastPreFailed;
    bool armedForTimeQualification{false};
    bool frozen{false};
};

static std::map<MonitorId, MonitorInstance> g_monitors;
static std::mutex g_mutex;
static std::thread g_worker;
static std::condition_variable g_cv;
static bool g_stopWorker{false};
static bool g_workerStarted{false};

static void worker_loop() {
    std::unique_lock<std::mutex> lk(g_mutex);
    while (!g_stopWorker) {
        // wake periodically (200 ms) or when signalled
        g_cv.wait_for(lk, std::chrono::milliseconds(200));
        if (g_stopWorker) break;

        const auto now = steady_clock::now();
        for (auto &p : g_monitors) {
            MonitorInstance &mi = p.second;
            if (mi.cfg.mode != DebounceMode::TimeBased) continue;
            if (mi.frozen) continue;
            if (!mi.preStartTime.has_value() || !mi.lastPreFailed.has_value()) continue;
            if (mi.qualified != QualifiedState::Unqualified) continue; // already qualified

            auto elapsed = duration_cast<milliseconds>(now - mi.preStartTime.value()).count();
            if (elapsed >= static_cast<int64_t>(mi.cfg.timeThresholdMs)) {
                // qualify according to lastPreFailed
                if (mi.lastPreFailed.value()) mi.qualified = QualifiedState::QualifiedFailed;
                else mi.qualified = QualifiedState::QualifiedPassed;

                // call notifier outside lock: copy notifier and id
                QualifiedNotifier notifier = mi.notifier;
                MonitorId id = p.first;
                lk.unlock();
                if (notifier) notifier(id, mi.qualified);
                lk.lock();
                // prevent repeated notifications until a reset occurs (e.g. opposite pre event)
                mi.preStartTime.reset();
                mi.armedForTimeQualification = false;
            }
        }
    }
}

// helper to ensure worker is running
static void start_worker_if_needed() {
    if (g_workerStarted) return;
    g_stopWorker = false;
    g_worker = std::thread(worker_loop);
    g_workerStarted = true;
}

// helper to stop worker (not strictly necessary in process lifetime)
static void stop_worker() {
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_stopWorker = true;
        g_cv.notify_all();
    }
    if (g_worker.joinable()) g_worker.join();
    g_workerStarted = false;
}

// --- Public API implementations ---

std::optional<bool> DMEvent::GetEventMemoryOverflow() {
    // standalone implementation: not supported / unknown
    return std::nullopt;
}

std::error_code DMEvent::SetEventMemoryOverflowNotifier(std::function<void(bool)> /*notifier*/) {
    // Not implemented in this simple manager
    return make_error_code(std::errc::operation_not_supported);
}

std::error_code DMEvent::RegisterMonitor(const MonitorId &id, DebounceConfig cfg, QualifiedNotifier notifier) {
    std::lock_guard<std::mutex> lk(g_mutex);
    if (g_monitors.find(id) != g_monitors.end()) {
        return make_error_code(std::errc::file_exists); // already registered
    }
    MonitorInstance mi;
    mi.cfg = cfg;
    mi.notifier = notifier;
    mi.counter = 0;
    mi.qualified = QualifiedState::Unqualified;
    mi.preStartTime.reset();
    mi.lastPreFailed.reset();
    mi.armedForTimeQualification = false;
    mi.frozen = false;
    g_monitors.emplace(id, std::move(mi));

    if (cfg.mode == DebounceMode::TimeBased) {
        start_worker_if_needed();
        g_cv.notify_all();
    }
    return std::error_code{}; // success
}

std::error_code DMEvent::UnregisterMonitor(const MonitorId &id) {
    std::lock_guard<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return make_error_code(std::errc::no_such_file_or_directory);
    g_monitors.erase(it);
    return std::error_code{};
}

std::error_code DMEvent::ReportPreEvent(const MonitorId &id, bool preFailed) {
    std::unique_lock<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return make_error_code(std::errc::no_such_file_or_directory);

    MonitorInstance &mi = it->second;

    if (mi.frozen) {
        // Ignore pre-events while frozen
        return std::error_code{};
    }

    // Counter-based debouncing
    if (mi.cfg.mode == DebounceMode::CounterBased) {
        // counter: increment towards failed on preFailed, decrement towards passed on prePassed.
        if (preFailed) {
            mi.counter += mi.cfg.failedStep;
            if (mi.counter > mi.cfg.failedThreshold) mi.counter = mi.cfg.failedThreshold;
        } else {
            mi.counter -= mi.cfg.passedStep;
            if (mi.counter < -static_cast<int>(mi.cfg.passedThreshold)) mi.counter = -static_cast<int>(mi.cfg.passedThreshold);
        }

        // evaluate qualification
        QualifiedState newState = QualifiedState::Unqualified;
        if (mi.counter >= mi.cfg.failedThreshold) newState = QualifiedState::QualifiedFailed;
        else if (mi.counter <= -static_cast<int>(mi.cfg.passedThreshold)) newState = QualifiedState::QualifiedPassed;

        if (newState != mi.qualified && newState != QualifiedState::Unqualified) {
            mi.qualified = newState;
            QualifiedNotifier notifier = mi.notifier;
            MonitorId mid = it->first;
            lk.unlock();
            if (notifier) notifier(mid, mi.qualified);
            lk.lock();
            // counter remains at threshold to prevent re-notify until reset by opposite events
        } else {
            // if the opposite pre event reduces counter below threshold, reset qualified state
            if (mi.qualified != QualifiedState::Unqualified && newState == QualifiedState::Unqualified) {
                mi.qualified = QualifiedState::Unqualified;
                QualifiedNotifier notifier = mi.notifier;
                MonitorId mid = it->first;
                lk.unlock();
                if (notifier) notifier(mid, mi.qualified);
                lk.lock();
            }
        }
    } else { // TimeBased
        auto now = steady_clock::now();
        // same pre state continues -> start or continue timer
        if (!mi.lastPreFailed.has_value() || mi.lastPreFailed.value() != preFailed) {
            // state changed: reset timer
            mi.lastPreFailed = preFailed;
            mi.preStartTime = now;
            mi.armedForTimeQualification = true;
            // If previously qualified and opposite preSeen, reset qualification
            if (mi.qualified != QualifiedState::Unqualified) {
                mi.qualified = QualifiedState::Unqualified;
                QualifiedNotifier notifier = mi.notifier;
                MonitorId mid = it->first;
                lk.unlock();
                if (notifier) notifier(mid, mi.qualified);
                lk.lock();
            }
        } else {
            // continuing; worker thread will promote to qualified when threshold elapsed
            // but we can do an immediate check here too for responsiveness
            if (mi.preStartTime.has_value()) {
                auto elapsed = duration_cast<milliseconds>(now - mi.preStartTime.value()).count();
                if (elapsed >= static_cast<int64_t>(mi.cfg.timeThresholdMs) && mi.qualified == QualifiedState::Unqualified) {
                    mi.qualified = (preFailed ? QualifiedState::QualifiedFailed : QualifiedState::QualifiedPassed);
                    QualifiedNotifier notifier = mi.notifier;
                    MonitorId mid = it->first;
                    lk.unlock();
                    if (notifier) notifier(mid, mi.qualified);
                    lk.lock();
                    // disarm timer until next pre event change
                    mi.preStartTime.reset();
                    mi.armedForTimeQualification = false;
                }
            }
        }
    }

    return std::error_code{};
}

std::optional<QualifiedState> DMEvent::GetQualifiedState(const MonitorId &id) {
    std::lock_guard<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return std::nullopt;
    return it->second.qualified;
}

std::error_code DMEvent::SetQualifiedState(const MonitorId &id, QualifiedState state) {
    std::unique_lock<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return make_error_code(std::errc::no_such_file_or_directory);
    MonitorInstance &mi = it->second;
    mi.qualified = state;
    QualifiedNotifier notifier = mi.notifier;
    MonitorId mid = it->first;
    lk.unlock();
    if (notifier) notifier(mid, state);
    return std::error_code{};
}

std::error_code DMEvent::FreezeDebouncing(const MonitorId &id) {
    std::lock_guard<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return make_error_code(std::errc::no_such_file_or_directory);
    it->second.frozen = true;
    return std::error_code{};
}

std::error_code DMEvent::ResetDebouncing(const MonitorId &id) {
    std::unique_lock<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return make_error_code(std::errc::no_such_file_or_directory);
    MonitorInstance &mi = it->second;
    mi.counter = 0;
    mi.preStartTime.reset();
    mi.lastPreFailed.reset();
    mi.armedForTimeQualification = false;
    mi.frozen = false;
    mi.qualified = QualifiedState::Unqualified;
    QualifiedNotifier notifier = mi.notifier;
    MonitorId mid = it->first;
    // notify de-qualification
    lk.unlock();
    if (notifier) notifier(mid, mi.qualified);
    return std::error_code{};
}

std::error_code DMEvent::TriggerFdcThresholdReached(const MonitorId &id) {
    std::unique_lock<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return make_error_code(std::errc::no_such_file_or_directory);
    // signal consumer that FDC threshold reached; here we call notifier with current qualified state
    QualifiedNotifier notifier = it->second.notifier;
    QualifiedState state = it->second.qualified;
    MonitorId mid = it->first;
    lk.unlock();
    if (notifier) notifier(mid, state);
    return std::error_code{};
}

std::error_code DMEvent::ResetTestFailed(const MonitorId &id) {
    std::unique_lock<std::mutex> lk(g_mutex);
    auto it = g_monitors.find(id);
    if (it == g_monitors.end()) return make_error_code(std::errc::no_such_file_or_directory);
    // reset only the TestFailed status: we interpret as de-qualify (Unqualified) but keep counters
    MonitorInstance &mi = it->second;
    mi.qualified = QualifiedState::Unqualified;
    QualifiedNotifier notifier = mi.notifier;
    MonitorId mid = it->first;
    lk.unlock();
    if (notifier) notifier(mid, mi.qualified);
    return std::error_code{};
}

// Ensure worker stopped at unload (best effort)
struct WorkerStopper {
    ~WorkerStopper() { stop_worker(); }
};
static WorkerStopper g_workerStopper;

}  // namespace event
}  // namespace diagnostic_manager

#endif // DM_EVENT_H
