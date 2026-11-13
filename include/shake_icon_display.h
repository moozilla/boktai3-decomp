#ifndef GUARD_SHAKE_ICON_DISPLAY_H
#define GUARD_SHAKE_ICON_DISPLAY_H
#include "projected_display.h"
struct ShakeIconDisplay {
    struct Task task;
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
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
struct ShakeIconDisplay *ShakeIconDisplay_CreateFromScript(void);
#endif
