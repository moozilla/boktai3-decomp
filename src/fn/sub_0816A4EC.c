#define ScenePanel_SetCallback Hud_SetUpdateCallback
#include "global.h"

struct ScenePanel {
    u8 unk00[0x24];
    void *layerMap;
    u8 unk28[0x9C];
    u8 keyframes[0x3BE0 - 0xC4];
    u8 animation[8];
    u32 flags;
    u8 unk3BEC[0x14];
    u16 x;
    u16 y;
};
void PauseMenu_HideAllSprites(void *);
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void StatusPanel_ResetSelectionAnimations(void *);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void PauseMenu_SetSpritePosition(void *, s32, s32, s32);
s32 MessageWindow_ShowMessage(s32);
void MenuPrompt_UpdateChoice(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void ScenePanel_BeginSelection(struct ScenePanel *p)
{
    PauseMenu_HideAllSprites(p);
    Bg_SetLayerMap(0, p->layerMap, 3);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    p->flags &= ~1;
    p->x = 0;
    p->y = 0;
    Anim_SetKeyframes(p->animation, p->keyframes, 7, 2, 0);
    StatusPanel_ResetSelectionAnimations(p);
    PauseMenu_SetConfirmationIndicator(p, 1);
    PauseMenu_SetSpritePosition(p, 7, -8, -8);
    MessageWindow_ShowMessage(1);
    Hud_SetUpdateCallback(p, MenuPrompt_UpdateChoice);
}
