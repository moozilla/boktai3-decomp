#include "global.h"
#include "engine.h"
#include "render.h"
struct M {
    u8 pad[0x2B8];
    void *res;
};
void sub_082157EC(void);
void Hud_SetMode(s32);
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, s32 *);
u16 *Video_GetWrappedBackgroundMapEntry(s32, s32, s32);
void ChallengeMenu_EnableBackground(struct M *);
void ChallengeMenu_InitBackgrounds(struct M *p)
{
    s32 value;
    s32 i;
    void *res;
    u16 *g = &gDispCntShadow;
    s32 mask = 0xFFFFF0FF;
    *g = mask & *g;
    sub_082157EC();
    Hud_SetMode(7);
    res = ScriptProc_Start(0xC091, 0x3536);
    p->res = res;
    value = 0x11;
    Bg_SetupLayers(3, 0, res, 0, 0, 1, &value);
    Bg_SetupLayers(2, 0, res, 0, 0, 1, &value);
    ChallengeMenu_EnableBackground(p);
    i = 1;
    do {
        u16 *e = Video_GetWrappedBackgroundMapEntry(3, 1, i);
        s32 next = i + 1;
        s32 n = 4;
        do {
            *e++ = 0x2002;
        } while (--n >= 0);
        i = next;
    } while (i <= 4);
}
