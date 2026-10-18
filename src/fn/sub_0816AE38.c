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

void PauseMenu_HideAllSprites(void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 MessageWindow_ShowMessage(s32);
void SceneSequence_CallCompletionScript(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void Sound_PlaySong(u32);
void ScenePanel_BeginAccountExit(struct ScenePanel *p)
{
    u32 zero;
    u16 *coord;
    PauseMenu_HideAllSprites(p);
    p->mainSprite.flags &= ~1;
    coord = &p->mainSprite.x;
    zero = 0;
    {
        u32 v = 0xFFE4;
        *coord = v;
    }
    p->mainSprite.y = 32;
    Anim_SetKeyframes(&p->mainSprite, p->keyframes, 6, 2, zero);
    p->secondary[0].flags &= ~1;
    if (p->account->kind != 6)
        p->secondary[1].flags &= ~1;
    p->detailSprite.flags &= ~1;
    Sound_PlaySong(0x2C1);
    MessageWindow_ShowMessage(3);
    Hud_SetUpdateCallback(p, SceneSequence_CallCompletionScript);
}
