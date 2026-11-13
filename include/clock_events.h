#ifndef GUARD_CLOCK_EVENTS_H
#define GUARD_CLOCK_EVENTS_H

#include "engine.h"
#include "sun_clock.h"

struct ClockEvents {
    struct Task task;         /* 0x00 */
    s16 hour, minute, second; /* 0x18 */
    s16 frame;                /* 0x1E: wraps after 60 updates */
    s32 seconds;              /* 0x20: cached time of day */
    s32 window;               /* 0x24: solar-event tolerance, in seconds */
    u32 procedure;            /* 0x28: script option p */
    u8 armed[4];              /* 0x2C: noon, solar time A, solar time B, reserved */
};

struct ClockEventArguments {
    u32 count : 16;
    s32 *values;
};

void ClockEvents_RunScript(struct ClockEvents *clock, s32 event);
s32 ClockEvents_Update(struct ClockEvents *clock);
u32 ClockEvents_Destroy(void);
s32 ClockEvents_Initialize(struct ClockEvents *clock);
struct ClockEvents *ClockEvents_Create(void);

#endif
