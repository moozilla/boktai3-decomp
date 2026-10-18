#define ScenePanel_SetCallback Hud_SetUpdateCallback
#include "global.h"
struct PanelSprite {
    u8 anim[8];
    u32 flags;
    u8 unkC[0x14];
    u16 x, y;
    u8 unk24[0x3C];
};
struct ScenePanel {
    u8 unk0[0x24];
    void *layerMap;
    u8 unk28[0x9C];
    u8 keyframes[0xB5C];
    struct PanelSprite base;
    u8 unkC80[0x2F60];
    struct PanelSprite mainSprite;
    u8 unk3C40[0xC0];
    struct PanelSprite detailSprite;
    u8 unk3D60[0x120];
    struct PanelSprite rows[4];
    struct PanelSprite icon;
    struct PanelSprite rowIcons[4];
    u8 unk41E0[0x706];
    u8 iconIndex;
    u8 unk48E7[2];
    u8 state;
    u16 timer;
    u16 selection;
    u16 previousSelection;
};
void PauseMenu_HideAllSprites(void *);
void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);
void sub_0816A0D0(void *);
void StatusPanel_UpdateGaugePosition(void *);
void sub_08169F44(void *);
s32 MessageWindow_ShowMessage(s32);
void ScenePanel_UpdateDetail(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void ScenePanel_BeginDetail(struct ScenePanel *p)
{
    u32 *a, *b;
    s32 i;
    u16 zero;
    PauseMenu_HideAllSprites(p);
    Bg_SetLayerMap(0, p->layerMap, 4);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    p->mainSprite.flags &= ~1;
    Anim_SetKeyframes(&p->mainSprite, p->keyframes, 5, 1, 0);
    p->timer = 0;
    sub_0816A0D0(p);
    StatusPanel_UpdateGaugePosition(p);
    p->base.flags &= ~1;
    p->detailSprite.flags &= ~1;
    p->detailSprite.x = 16;
    p->detailSprite.y = -4;
    Anim_LoadKeyframe(&p->detailSprite, p->keyframes, 0x12, 1);
    p->icon.flags &= ~1;
    Anim_LoadKeyframe(&p->icon, p->keyframes, p->iconIndex + 9, 1);
    b = &p->rowIcons[0].flags;
    a = &p->rows[0].flags;
    for (i = 3; i >= 0; i--) {
        *a &= ~1;
        *b &= ~1;
        b += 24;
        a += 24;
    }
    zero = 0;
    sub_08169F44(p);
    p->selection = zero;
    p->previousSelection = zero;
    p->state = 0;
    Anim_LoadKeyframe(&p->detailSprite, p->keyframes, 0x12, 1);
    AnimData_SelectPaletteBank((u8 *)p + 0xC54, 0x1B4);
    MessageWindow_ShowMessage(4);
    Hud_SetUpdateCallback(p, ScenePanel_UpdateDetail);
}
