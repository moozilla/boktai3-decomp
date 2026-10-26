#include "global.h"
#include "render.h"
#define Hud_SetPhase        Hud_SetUpdateCallback
#define gDisplayUpdateFlags gEnginePauseFlags
struct HudManager {
    u8 pad0[0xA12];
    u16 savedControl;
    u8 padA14[0x43E0 - 0xA14];
    u16 savedBlend[4];
};
s32 SceneHud_CanOpenFromStart(struct HudManager *);
void Hud_SetUpdateCallback(struct HudManager *, void (*)(void));
void SceneDialog_UpdateEntryFade(void);
static inline void SetFlags(u32 mask)
{
    gEnginePauseFlags |= mask;
}
s32 Hud_BeginSavedBlend(struct HudManager *p)
{
    s32 result;
    if (!SceneHud_CanOpenFromStart(p)) {
        result = 0;
    } else {
        SetFlags(4);
        p->savedControl = gFadeMultiplier;
        p->savedBlend[0] = gBgFadeColor;
        p->savedBlend[1] = gBgFadeBankMask;
        p->savedBlend[2] = gObjFadeColor;
        p->savedBlend[3] = gObjFadeFlags;
        Hud_SetUpdateCallback(p, SceneDialog_UpdateEntryFade);
        result = 1;
    }
    return result;
}
