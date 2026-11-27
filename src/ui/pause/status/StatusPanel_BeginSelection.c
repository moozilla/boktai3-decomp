#include "ui/pause/menu_prompt.h"
#include "player/player.h"

void PauseMenu_HideAllSprites(void *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void StatusPanel_ResetSelectionAnimations(void *);
void PauseMenu_SetConfirmationIndicator(void *, s32);
void PauseMenu_SetSpritePosition(void *, s32, s32, s32);
s32 MessageWindow_ShowMessage(s32);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(struct SceneHudState *));
void StatusPanel_BeginSelection(struct SceneHudState *p)
{
    PauseMenu_HideAllSprites(p);
    Bg_SetLayerMap(0, p->accountLayerMap, 3);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    p->accountAnimation.flags &= ~1;
    p->accountAnimation.position.x = 0;
    p->accountAnimation.position.y = 0;
    Anim_SetKeyframes(&p->accountAnimation, &p->accountResource, 7, 2, 0);
    StatusPanel_ResetSelectionAnimations(p);
    PauseMenu_SetConfirmationIndicator(p, 1);
    PauseMenu_SetSpritePosition(p, 7, -8, -8);
    MessageWindow_ShowMessage(1);
    Hud_SetUpdateCallback(p, MenuPrompt_UpdateChoice);
}
