#ifndef GUARD_TIMED_PRESENTATION_H
#define GUARD_TIMED_PRESENTATION_H
#include "engine.h"
struct TimedPresentation {
    struct Task task;
    s32 state, timer, exitTimer, minShift, fadeShift, duration, tileLayer;
    s32 exitScript;
};
void TimedPresentation_Update(struct TimedPresentation *);
void TimedPresentation_Destroy(void);
s32 TimedPresentation_Initialize(struct TimedPresentation *);
#endif
