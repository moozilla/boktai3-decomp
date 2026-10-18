// Tentative StatusPanel family and semantic names; based on static consumers.
#define StatusPanel_BeginAnimatedTransition sub_0816A700
#define Panel_SetUpdateCallback Hud_SetUpdateCallback
#include "global.h"
struct StatusPanel {
    u8 unknown0[0x24];
    void *layerMap;
    u8 unknown28[0xC4 - 0x28];
    u8 animationData[0x3BE0 - 0xC4];
    u32 animation[2];
    u32 animationFlags;
};
void PauseMenu_HideAllSprites(void *);
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 MessageWindow_ShowMessage(s32);
void Sound_PlaySong(u32);
void sub_0816A9A8(void);
void Hud_SetUpdateCallback(struct StatusPanel *, void (*)(void));
void sub_0816A700(struct StatusPanel *p)
{
    PauseMenu_HideAllSprites(p);
    Bg_SetLayerMap(0, p->layerMap, 5);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    p->animationFlags &= ~1;
    Anim_SetKeyframes(p->animation, p->animationData, 6, 2, 0);
    MessageWindow_ShowMessage(3);
    Sound_PlaySong(0x2C1);
    Hud_SetUpdateCallback(p, sub_0816A9A8);
}
