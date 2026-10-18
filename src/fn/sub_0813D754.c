#define SceneActor_GetEffectElapsedTicks Player_GetEffectTickDelta
#define SceneActor_SelectEffectDisplayMode sub_0813D754
#define SceneActor_StartRandomizedEffect sub_0813D3B0
#define gSceneActorEffectThresholds gUnk_08610918
#include "global.h"
struct SceneActorEffectDisplay {
    u8 unknown0[0x1C];
    u32 flags;
    u8 unknown20[0x4F4-0x20];
    u8 tick, savedTick, overrideMode;
    u8 unknown4F7[0x5B9-0x4F7];
    u8 effectMode, unknown5BA, effectActive;
    u8 unknown5BC[0xACA-0x5BC];
    u16 savedMode;
    s16 flashTimer;
};
extern const u8 gUnk_08610918[];
void sub_0813D3B0(void *);
s32 Player_GetEffectTickDelta(void *);
s32 sub_0813D754(struct SceneActorEffectDisplay *p, s32 mode)
{
    s32 original = mode;
    if (p->effectActive) {
        if (p->flags & 1) {
            if (Player_GetEffectTickDelta(p) >= gUnk_08610918[p->effectMode - 1]) {
                sub_0813D3B0(p);
                p->savedTick = p->tick;
            }
        }
        if (p->effectActive) {
            if (p->effectMode == 1) mode = 3;
            else if (p->effectMode == 2) mode = 4;
            else if (p->effectMode == 3) mode = 5;
        }
    }
    if (p->overrideMode) mode = 7;
    if (mode == original) {
        if (p->flashTimer) {
            if (((u16)p->flashTimer >> 2) & 1) mode = p->savedMode;
            p->flashTimer--;
        }
    } else {
        p->savedMode = mode;
        p->flashTimer = 64;
    }
    return mode;
}
