#include "global.h"
#define Hud_SetPhase        Hud_SetUpdateCallback
#define gDisplayUpdateFlags gUnk_0300523C
struct HudManager {
    u8 pad0[0xA12];
    u16 savedControl;
    u8 padA14[0x43E0 - 0xA14];
    u16 savedBlend[4];
};
extern u32 gUnk_0300523C, gUnk_030051D8;
extern u16 gUnk_03005214, gUnk_030051C4, gUnk_030051DC, gUnk_03005210;
s32 sub_08176E20(struct HudManager *);
void Hud_SetUpdateCallback(struct HudManager *, void (*)(void));
void SceneDialog_UpdateEntryFade(void);
static inline void SetFlags(u32 mask)
{
    gUnk_0300523C |= mask;
}
s32 Hud_BeginSavedBlend(struct HudManager *p)
{
    s32 result;
    if (!sub_08176E20(p)) {
        result = 0;
    } else {
        SetFlags(4);
        p->savedControl = gUnk_030051D8;
        p->savedBlend[0] = gUnk_03005214;
        p->savedBlend[1] = gUnk_030051C4;
        p->savedBlend[2] = gUnk_030051DC;
        p->savedBlend[3] = gUnk_03005210;
        Hud_SetUpdateCallback(p, SceneDialog_UpdateEntryFade);
        result = 1;
    }
    return result;
}
