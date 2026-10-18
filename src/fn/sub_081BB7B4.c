#include "global.h"
void Bg_SetLayerMap(int, void *, int);
void BgGfx_LoadTilemapSimple(int, int, int, int, int);
void SpriteMenu_HideAll(void *);
void Sound_PlaySong(int);
void Anim_SetKeyframes(void *, void *, int, int, int);
void MessageWindow_ShowMessage(int);
void SpriteMenu_UpdateTimedFade(void);
void SolarBank_SetState(void *, void (*)(void));
void SolarBank_BeginClosing(u8 *p)
{
    int z;
    Bg_SetLayerMap(0, *(void **)(p + 0x6e8), 2);
    z = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, z);
    SpriteMenu_HideAll(p);
    Sound_PlaySong(0x2bf);
    *(u32 *)(p + 0x60) &= ~1;
    Anim_SetKeyframes(p + 0x58, p + 0x18, 2, 2, z);
    MessageWindow_ShowMessage(3);
    SolarBank_SetState(p, SpriteMenu_UpdateTimedFade);
}
