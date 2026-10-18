#define ScenePanel_SetCallback Hud_SetUpdateCallback
#include "global.h"
struct PanelAccount {
    u8 unk0[0x418];
    u8 kind;
    u8 unk419[15];
    u16 amount;
};
struct PanelSprite {
    u8 anim[8];
    u32 flags;
    u8 unkC[0x14];
    u16 x, y;
    u8 unk24[0x3C];
};
struct ScenePanel {
    u8 unk0[0x18];
    u32 resource;
    u8 unk1C[8];
    void *layerMap;
    u32 unk28;
    u32 mapResource;
    u8 unk30[0x28];
    void *text;
    u8 unk5C[0x68];
    u8 keyframes[0x97C];
    struct PanelAccount *account;
    u8 unkA44[0x1DC];
    struct PanelSprite base;
    u8 unkC80[0x2F60];
    struct PanelSprite mainSprite;
    struct PanelSprite secondary[2];
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

s32 MessageWindow_Clear(void);
u32 ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetDisplayMode(s32);
void Hud_SetupBackgroundLayer(void *, u32, int, int, u32);
void PauseMenu_HideAllSprites(void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void MenuPrompt_UpdateSelectionSprites(void *);
s32 MessageWindow_ShowMessage(s32);
void SceneSequence_UpdateRamp64(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void ScenePanel_BeginAccountMenu(struct ScenePanel *p)
{
    u32 zero;
    u16 *coord;
    struct PanelSprite *anim;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    MessageWindow_SetBounds(1, 1, 28, 6);
    MessageWindow_SetMessageTable(p->text);
    MessageWindow_SetDisplayMode(1);
    Hud_SetupBackgroundLayer(p, p->resource, 0, 2, p->mapResource);
    PauseMenu_HideAllSprites(p);
    p->mainSprite.flags &= ~1;
    coord = &p->mainSprite.x;
    zero = 0;
    {
        u32 v = 0xFFE4;
        *coord = v;
    }
    p->mainSprite.y = 32;
    Anim_SetKeyframes(&p->mainSprite, p->keyframes, 7, 2, zero);
    p->secondary[0].flags &= ~1;
    if (p->account->kind != 6)
        p->secondary[1].flags &= ~1;
    p->detailSprite.flags &= ~1;
    anim = &p->detailSprite;
    Anim_LoadKeyframe(anim, p->keyframes, 0x1D, 1);
    Anim_SetKeyframes(anim, p->keyframes, 10, 1, zero);
    MenuPrompt_UpdateSelectionSprites(p);
    MessageWindow_ShowMessage(7);
    Hud_SetUpdateCallback(p, SceneSequence_UpdateRamp64);
}
