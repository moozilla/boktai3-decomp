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
void sub_08055B90(struct StageIconDisplay *, void (*)(struct StageIconDisplay *));
void sub_08055BA8(struct StageIconDisplay *);
void sub_08055BC8(struct StageIconDisplay *);
void sub_08055BE0(struct StageIconDisplay *);
void sub_08055C04(struct StageIconDisplay *);
void sub_08055CA4(struct StageIconDisplay *);
void sub_08055D38(struct StageIconDisplay *);
s32 sub_08055D74(struct StageIconDisplay *);
s32 sub_08055DA0(struct StageIconDisplay *);
void sub_08055DD4(struct StageIconDisplay *, s32);
void sub_08055EB8(struct StageIconDisplay *, s32);
s32 sub_08055EEC(struct StageIconDisplay *, s32, u32, void (*)(void *, struct StageIconDisplay *),
                 void *);
s32 sub_08055FA4(struct StageIconDisplay *);
void sub_0821983C(void *, void *, u32, u32, u32, u32, s32, const void *);
#endif
