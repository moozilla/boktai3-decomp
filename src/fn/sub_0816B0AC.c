#define SelectionMenu_SetPhase          Hud_SetUpdateCallback
#define SelectionMenu_SetWidgetPosition PauseMenu_SetSpritePosition
#include "global.h"
#include "input.h"
struct SelectionMenuOwner {
    u8 unknown0[0x418];
    u8 mode;
};
struct SelectionMenu {
    u8 unknown0[0xC4];
    u8 animationData[0x97C];
    struct SelectionMenuOwner *owner;
    u8 unknownA44[0x319C];
    u8 primaryAnimation[0x120];
    u8 selectionAnimation[0xBE4];
    u8 selection;
    u8 unknown48E5;
    u8 count;
    u8 unknown48E7[0xD];
    u32 pending;
};
s32 Anim_Step(void *, void *);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Sound_PlaySong(u32);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_SetNumberArgument(s32, s32);
void PauseMenu_SetConfirmationIndicator(struct SelectionMenu *, s32);
void PauseMenu_SetSpritePosition(struct SelectionMenu *, s32, s32, s32);
void Hud_SetUpdateCallback(struct SelectionMenu *, void (*)(struct SelectionMenu *));
void SelectionMenu_ApplyConfirmedReward(struct SelectionMenu *);
void SceneSequence_UpdateRampDown64(struct SelectionMenu *);
void SceneSequence_BeginAnimation6(struct SelectionMenu *);
void MenuPrompt_UpdateSelectionSprites(struct SelectionMenu *);
void SelectionMenu_UpdateConfirmInput(struct SelectionMenu *p)
{
    Anim_Step(p->primaryAnimation, p->animationData);
    Anim_Step(p->selectionAnimation, p->animationData);
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        Anim_LoadKeyframe(p->selectionAnimation, p->animationData, 0x1D, 1);
        if (!p->selection) {
            MessageWindow_ShowMessage(8);
            MessageWindow_SetNumberArgument(0, (p->count - 1) * 50);
            PauseMenu_SetConfirmationIndicator(p, 0);
            PauseMenu_SetSpritePosition(p, 7, -8, -104);
            Hud_SetUpdateCallback(p, SelectionMenu_ApplyConfirmedReward);
        } else if (p->pending) {
            Hud_SetUpdateCallback(p, SceneSequence_UpdateRampDown64);
        }
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        SceneSequence_BeginAnimation6(p);
    } else if (p->owner->mode != 6 && (gPadState[1] & 0xC0)) {
        Sound_PlaySong(0xDC);
        p->selection = 1 - p->selection;
        MenuPrompt_UpdateSelectionSprites(p);
    }
}
