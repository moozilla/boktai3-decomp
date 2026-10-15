#ifndef GUARD_STAGE_ICON_DISPLAY_H
#define GUARD_STAGE_ICON_DISPLAY_H
#include "projected_display.h"
/* Tentative identity: a keyframe icon with three staged indicator animations. */
struct StageIconDisplay {
    u8 task[0x18];
    struct ProjectedDisplayResource resource;
    struct ProjectedDisplayEntry icon, indicators[3];
    u8 scriptCompletion, option;
    u16 stateFrame;
    u32 killRequested;
    s32 revealedCount;
    void (*completion)(void *, struct StageIconDisplay *);
    void *completionArgument;
    u32 completionScriptId;
    void (*update)(struct StageIconDisplay *);
};
extern struct StageIconDisplay *gUnk_0200011C;
void StageIconDisplay_SetUpdate(struct StageIconDisplay *, void (*)(struct StageIconDisplay *));
void StageIconDisplay_NotifyNative(struct StageIconDisplay *);
void StageIconDisplay_CallCompletionScript(struct StageIconDisplay *);
void StageIconDisplay_NotifyCompletion(struct StageIconDisplay *);
void StageIconDisplay_TickReveal(struct StageIconDisplay *);
void StageIconDisplay_TickShake(struct StageIconDisplay *);
void StageIconDisplay_TickDelay(struct StageIconDisplay *);
s32 StageIconDisplay_Tick(struct StageIconDisplay *);
s32 StageIconDisplay_Destroy(struct StageIconDisplay *);
void StageIconDisplay_InitAnimations(struct StageIconDisplay *, s32);
void StageIconDisplay_SelectUpdate(struct StageIconDisplay *, s32);
s32 StageIconDisplay_Initialize(struct StageIconDisplay *, s32, u32,
                                void (*)(void *, struct StageIconDisplay *), void *);
s32 StageIconDisplay_InitFromScript(struct StageIconDisplay *);
void sub_0821983C(void *, void *, u32, u32, u32, u32, s32, const void *);
#endif
