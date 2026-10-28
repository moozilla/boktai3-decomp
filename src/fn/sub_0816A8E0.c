#include "global.h"
#include "game_state.h"
#include "input.h"
struct PanelAccount {
    u8 unk0[0x428];
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
    u8 unk0[0x24];
    void *layerMap;
    u8 unk28[0x9C];
    u8 keyframes[0x97C];
    struct PanelAccount *account;
    u8 unkA44[0x1DC];
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

s32 Anim_Step(void *, void *);
void Sound_PlaySong(u32);
void StatusPanel_BeginAnimatedTransition(void *);
void ScenePanel_BeginSelection(void *);
void PauseMenu_UpdateQuantityInput(void *);
void ScenePanel_UpdateDetail(struct ScenePanel *p)
{
    Anim_Step(&p->mainSprite, p->keyframes);
    if (gPadState[1] & 1) {
        if (p->timer == 0)
            Sound_PlaySong(0xDE);
        else {
            s32 *total;
            s32 value;
            p->account->amount += p->timer;
            *(s32 *)((u8 *)gGameState + 0x50C) += p->timer * p->iconIndex;
            total = &gGameState->unk51C;
            value = *total + p->timer * p->iconIndex;
            if (value < 0)
                value = 0x7FFFFFFF;
            *total = value;
            Sound_PlaySong(0xDD);
        }
        StatusPanel_BeginAnimatedTransition(p);
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        ScenePanel_BeginSelection(p);
    } else
        PauseMenu_UpdateQuantityInput(p);
}
