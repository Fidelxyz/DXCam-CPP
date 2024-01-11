#ifndef DXCAM_CPP_HIGHRESTIMER_H
#define DXCAM_CPP_HIGHRESTIMER_H

#include <windows.h>

class HighResTimer {
public:
    explicit HighResTimer(int period_ms);
    void wait() const;
    void cancel() const;

private:
    HANDLE handle;
};


#endif  // DXCAM_CPP_HIGHRESTIMER_H
