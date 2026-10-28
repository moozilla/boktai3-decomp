#include "global.h"
#include "engine.h"
#include "menu_parameters.h"
#include "runtime.h"

struct Params3D0Gfx {
    u8 pad[0x6220];
};

void TimeDisplay_InitBackgrounds(void *);
void TimeDisplay_InitAnimatedObjects(void *);
void TimeDisplay_SetSelectionKeyframe(void *);
void Actor_HideAuxiliarySprites(void *);
void BgGfx_LoadTilemapAtPosition(s32, s32, s32);
void Menu_StartFadeIn(void);

s32 Params3D0_InitDisplay(u8 *p)
{
    s32 i;
    u16 *q;
    u16 v;
    *(s32 *)(p + 0x758) = -240;
    *(s32 *)(p + 0x760) = 8;
    TimeDisplay_InitBackgrounds(p);
    TimeDisplay_InitAnimatedObjects(p);
    if ((s16)gTimeDisplayParameters.silent == 1) {
        *(s32 *)(p + 0x724) = *(s16 *)((u8 *)gGameState + 0x870);
        *(u32 *)(p + 0x20) &= ~1;
        *(u32 *)(p + 0x350) &= ~1;
        *(u32 *)(p + 0x3d8) &= ~1;
        *(u32 *)(p + 0xa8) &= ~1;
        *(u32 *)(p + 0x130) &= ~1;
        *(u32 *)(p + 0x460) &= ~1;
        BgGfx_LoadTilemapAtPosition(2, 0, 0);
        *(s32 *)(p + 0x72c) = *(s32 *)(p + 0x74c) = 8;
        TimeDisplay_SetSelectionKeyframe(p);
        Actor_HideAuxiliarySprites(p);
    } else {
        *(s32 *)(p + 0x724) = 0;
        *(s32 *)(p + 0x72c) = *(s32 *)(p + 0x74c) = 0;
    }
    if (gTimeDisplayParameters.M == 1)
        *(s32 *)(p + 0x728) = 1;
    else
        *(s32 *)(p + 0x728) = gSessionState[9] ? 4 : 3;
    *(s32 *)(p + 0x740) = 0;
    *(s32 *)(p + 0x744) = 0;
    Sound_PlaySong(0xa4);
    *(s32 *)(p + 0x738) = gTimeDisplayParameters.fadeOut;
    Menu_StartFadeIn();
    CpuSet(gPaletteBankData + 0x6220, p + 0x500, 0x100);
    v = 0x7fff;
    i = 15;
    q = (u16 *)(p + 0x71e);
    for (; i >= 0; i--)
        *q-- = v;
    return 0;
}
