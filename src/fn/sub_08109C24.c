#include "global.h"
#include "sun_clock.h"
struct ClockPanelSnapshot {
    u8 unk0[0x20];
    u16 time;
    u8 unk22[0x1B];
    u8 sun;
};
struct ClockPanel {
    u8 unk0[0x84];
    u16 values[2];
    u8 unk88[0x58];
    struct ClockPanelSnapshot snapshot;
    u8 unk120[0x114];
    u16 first[2];
    u16 second[2];
};
struct ClockState {
    u8 unk0[0x18];
    s16 mode;
};
extern struct ClockState *gUnk_020004B4;
u8 SunClock_GetPhase(void);
void ClockPanel_CaptureTime(struct ClockPanel *p)
{
    struct ClockPanelSnapshot *snapshot = &p->snapshot;
    struct ClockState **state = &gUnk_020004B4;
    s32 i;
    for (i = 0; i < 2; i++) {
        p->values[i] = 0;
        p->first[i] = 0;
        p->second[i] = 0;
    }
    if (*state && (*state)->mode == 0)
        snapshot->time = gSunClock.extra6 * 15 + gSunClock.extra7;
    else
        snapshot->time = 0;
    snapshot->sun = SunClock_GetPhase();
}
