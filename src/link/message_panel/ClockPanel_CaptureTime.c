#include "global.h"
#include "system/clock/sun_clock.h"
#include "scene/areas/area_effect_link.h"
/* The record exchanged by the selection panel, in the session data. */
struct ClockPanelSnapshot {
    u8 unknown0[0x20];
    u16 time;
    u8 unknown22[0x1B];
    u8 sun;
};
/* Transfer bookkeeping of the same session. */
struct ClockPanel {
    u8 unknown0[0x84];
    u16 values[2];
    u8 unknown88[0x58];
    struct ClockPanelSnapshot snapshot;
    u8 unknown120[0x114];
    u16 first[2];
    u16 second[2];
};
u8 SunClock_GetPhase(void);
void ClockPanel_CaptureTime(struct ClockPanel *p)
{
    struct ClockPanelSnapshot *snapshot = &p->snapshot;
    s32 i;

    for (i = 0; i < 2; i++) {
        p->values[i] = 0;
        p->first[i] = 0;
        p->second[i] = 0;
    }
    if (gLinkSession && gLinkSession->localPlayer == 0)
        snapshot->time = gSunClock.extra6 * 15 + gSunClock.extra7;
    else
        snapshot->time = 0;
    snapshot->sun = SunClock_GetPhase();
}
