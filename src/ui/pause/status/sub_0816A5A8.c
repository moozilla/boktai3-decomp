#include "ui/pause/menu_prompt.h"
#include "player/player.h"
void PauseMenu_HideAllSprites(void *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void sub_0816A0D0(void *);
void StatusPanel_UpdateGaugePosition(void *);
void PauseMenu_RefreshPriceDigits(void *);
s32 MessageWindow_ShowMessage(s32);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(struct SceneHudState *));
void ScenePanel_BeginDetail(struct SceneHudState *p)
{
    s32 i;
    PauseMenu_HideAllSprites(p);
    Bg_SetLayerMap(0, p->accountLayerMap, 4);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    p->accountAnimation.flags &= ~1;
    Anim_SetKeyframes(&p->accountAnimation, &p->accountResource, 5, 1, 0);
    p->valueRate = 0;
    sub_0816A0D0(p);
    StatusPanel_UpdateGaugePosition(p);
    p->selectorFlags &= ~1;
    p->accountCursor.flags &= ~1;
    p->accountCursor.position.x = 16;
    p->accountCursor.position.y = -4;
    Anim_LoadKeyframe(&p->accountCursor, &p->accountResource, 0x12, 1);
    p->accountRateIcon.flags &= ~1;
    Anim_LoadKeyframe(&p->accountRateIcon, &p->accountResource, p->selectedQuantity + 9, 1);
    for (i = 0; i < 4; i++) {
        p->accountValueDigits[i].flags &= ~1;
        p->accountTotalDigits[i].flags &= ~1;
    }
    PauseMenu_RefreshPriceDigits(p);
    p->balanceLimitTimer = 0;
    p->balanceLimitShown = 0;
    p->quantityArrowTimer = 0;
    Anim_LoadKeyframe(&p->accountCursor, &p->accountResource, 0x12, 1);
    AnimData_SelectPaletteBank((u8 *)p + 0xC54, 0x1B4);
    MessageWindow_ShowMessage(4);
    Hud_SetUpdateCallback(p, ScenePanel_UpdateDetail);
}
