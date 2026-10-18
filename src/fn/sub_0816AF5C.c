#define ScenePanel_UpdateAccountIntro sub_0816AF5C
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
    u8 unkA44[8];
    u16 frame;
    u8 unkA4E[0x1D2];
    struct PanelSprite base;
    u8 unkC80[0x2F60];
    struct PanelSprite mainSprite;
    struct PanelSprite secondary[2];
    struct PanelSprite detailSprite;
    u8 unk3D60[0x120];
    struct PanelSprite rows[4];
    struct PanelSprite icon;
    struct PanelSprite rowIcons[4];
    u8 unk41E0[0x704];
    u8 menuSelection;
    u8 unk48E5;
    u8 iconIndex;
    u8 unk48E7[2];
    u8 state;
    u16 timer;
    u16 selection;
    u16 previousSelection;
};

void Hud_InitSelectionBounds(u32, u32);
void Sound_PlaySong(u32);
s32 Anim_Step(void *, void *);
s32 MessageWindow_ShowMessage(s32);
void sub_0816AC08(void *);
void sub_0816B0AC(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void sub_0816AF5C(struct ScenePanel *p)
{
    if (p->frame <= 15)
        Hud_InitSelectionBounds(p->frame * 4, 0);
    else
        Hud_InitSelectionBounds(64, 0);
    if (p->frame == 16)
        Sound_PlaySong(0x2C0);
    Anim_Step(&p->mainSprite, p->keyframes);
    if (p->frame > 104)
        p->mainSprite.x += 5;
    if (++p->frame > 116) {
        MessageWindow_ShowMessage(7);
        p->mainSprite.x = 0xFFE4;
        p->detailSprite.flags &= ~1;
        p->secondary[0].flags &= ~1;
        if (p->account->kind != 6)
            p->secondary[1].flags &= ~1;
        p->menuSelection = 0;
        sub_0816AC08(p);
        Hud_SetUpdateCallback(p, sub_0816B0AC);
    }
}
