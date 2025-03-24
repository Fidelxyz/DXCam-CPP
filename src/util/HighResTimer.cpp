#include "HighResTimer.h"

#include <cassert>

HighResTimer::HighResTimer(const int period_ms)
    : handle_(CreateWaitableTimerExW(nullptr, nullptr,
                                     CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                     TIMER_ALL_ACCESS)) {
    constexpr LARGE_INTEGER due_time{0, 0};  // init to zero
    const bool res = SetWaitableTimer(handle_, &due_time, period_ms, nullptr,
                                      nullptr, false);
    assert(res);
}

void HighResTimer::wait() const {
    const auto res = WaitForSingleObject(handle_, INFINITE);
    assert(res == 0);
}

void HighResTimer::cancel() const {
    const bool res = CancelWaitableTimer(handle_);
    assert(res);
}
