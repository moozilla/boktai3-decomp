#include "global.h"
void DarkLoan_SetState(void *, void (*)(void));
void DarkLoan_HideSprites(void *);
void Bg_SetLayerMap(s32, s32, s32);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
s32 MessageWindow_ShowMessage(s32);
void StatGauge_WaitBeforeFade(void);
struct G {
    u8 pad[0x534];
    s16 level;
};
extern struct G *gGameState;
void StatGauge_ShowCompletionMessage(u8 *p)
{
    Bg_SetLayerMap(0, *(s32 *)(p + 0x8e0), 5);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    DarkLoan_HideSprites(p);
    *(u32 *)(p + 0x60) &= ~1;
    if (gGameState->level <= 3)
        MessageWindow_ShowMessage(9);
    else if (gGameState->level <= 5)
        MessageWindow_ShowMessage(0xa);
    else if (gGameState->level <= 7)
        MessageWindow_ShowMessage(0xb);
    else
        MessageWindow_ShowMessage(0xc);
    DarkLoan_SetState(p, StatGauge_WaitBeforeFade);
}
