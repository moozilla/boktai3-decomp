#ifndef GUARD_SHAKE_ICON_DISPLAY_H
#define GUARD_SHAKE_ICON_DISPLAY_H
#include "projected_display.h"
struct ShakeIconDisplay {
    u8 task[0x18];
    struct ProjectedDisplayResource resource;
    struct ProjectedDisplayEntry animation;
    u16 unknown98, stateFrame, lifetime, unknown9E;
    struct ContactPosition position;
    u32 completionScriptId;
    void (*update)(struct ShakeIconDisplay *);
};
void ShakeIconDisplay_TickShake(struct ShakeIconDisplay *);
s32 ShakeIconDisplay_Tick(struct ShakeIconDisplay *);
u32 ShakeIconDisplay_Destroy(struct ShakeIconDisplay *);
s32 ShakeIconDisplay_Initialize(struct ShakeIconDisplay *);
void sub_0821983C(void *, void *, u32, u32, u32, u32, s32, const void *);
#endif
