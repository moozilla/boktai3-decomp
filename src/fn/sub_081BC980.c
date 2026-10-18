#include "global.h"
void Bg_SetLayerMap(int, void *, int);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void DarkLoan_HideSprites(void *);
void Sound_PlaySong(int);
void Anim_SetKeyframes(void *, void *, int, int, int);
void MessageWindow_ShowMessage(int);
void sub_081BD084(void);
void DarkLoan_SetState(void *, void (*)(void));
void DarkLoan_BeginClosing(u8 *p)
{
    int z;
    Bg_SetLayerMap(0, *(void **)(p + 0x8e0), 5);
    z = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, z);
    DarkLoan_HideSprites(p);
    Sound_PlaySong(0x2c1);
    *(u32 *)(p + 0x60) &= ~1;
    Anim_SetKeyframes(p + 0x58, p + 0x18, 6, 2, z);
    MessageWindow_ShowMessage(3);
    DarkLoan_SetState(p, sub_081BD084);
}
