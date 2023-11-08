//
// Created by Fidel on 2023/11/7.
//
#include "HighResTimer.h"

#include <cassert>

HighResTimer::HighResTimer(const int period_ms)
    : handle(CreateWaitableTimerExW(nullptr, nullptr,
                                    CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                    TIMER_ALL_ACCESS)) {
    LARGE_INTEGER due_time = {};  // init to zero
    bool res = SetWaitableTimer(this->handle, &due_time, period_ms, nullptr,
                                nullptr, false);
    assert(res);
}

void HighResTimer::wait() const {
    auto res = WaitForSingleObject(this->handle, INFINITE);
    assert(res == 0);
}

void HighResTimer::cancel() const {
    bool res = CancelWaitableTimer(this->handle);
    assert(res);
}
