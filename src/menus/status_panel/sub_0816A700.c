// Tentative StatusPanel family and semantic names; based on static consumers.
#define Panel_SetUpdateCallback Hud_SetUpdateCallback
#include "menus/menu_prompt/menu_prompt.h"
#include "engine/shared/player.h"
void PauseMenu_HideAllSprites(void *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
s32 MessageWindow_ShowMessage(s32);
void Sound_PlaySong(u32);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(struct SceneHudState *));
void StatusPanel_BeginAnimatedTransition(struct SceneHudState *p)
{
    PauseMenu_HideAllSprites(p);
    Bg_SetLayerMap(0, p->accountLayerMap, 5);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    p->accountAnimation.flags &= ~1;
    Anim_SetKeyframes(&p->accountAnimation, &p->accountResource, 6, 2, 0);
    MessageWindow_ShowMessage(3);
    Sound_PlaySong(0x2C1);
    Hud_SetUpdateCallback(p, MenuPrompt_FinishDelayedChoice);
}
