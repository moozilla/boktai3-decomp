#define SelectionMenu_SetPhase          Hud_SetUpdateCallback
#define SelectionMenu_SetWidgetPosition PauseMenu_SetSpritePosition
#include "menus/menu_prompt/menu_prompt.h"
#include "engine/shared/player.h"
#include "engine/shared/input.h"
s32 Anim_Step(void *, void *);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Sound_PlaySong(u32);
s32 MessageWindow_ShowMessage(s32);
s32 MessageWindow_SetNumberArgument(s32, s32);
void PauseMenu_SetConfirmationIndicator(struct SceneHudState *, s32);
void PauseMenu_SetSpritePosition(struct SceneHudState *, s32, s32, s32);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(struct SceneHudState *));
void SelectionMenu_ApplyConfirmedReward(struct SceneHudState *);
void SceneSequence_UpdateRampDown64(struct SceneHudState *);
void SceneSequence_BeginAnimation6(struct SceneHudState *);
void SelectionMenu_UpdateConfirmInput(struct SceneHudState *p)
{
    Anim_Step(&p->accountAnimation, &p->accountResource);
    Anim_Step(&p->accountCursor, &p->accountResource);
    if (gPadState[1] & 1) {
        Sound_PlaySong(0xDD);
        Anim_LoadKeyframe(&p->accountCursor, &p->accountResource, 0x1D, 1);
        if (!p->accountSelection) {
            MessageWindow_ShowMessage(8);
            MessageWindow_SetNumberArgument(0, (p->selectedQuantity - 1) * 50);
            PauseMenu_SetConfirmationIndicator(p, 0);
            PauseMenu_SetSpritePosition(p, 7, -8, -104);
            Hud_SetUpdateCallback(p, SelectionMenu_ApplyConfirmedReward);
        } else if (p->loanDisplay) {
            Hud_SetUpdateCallback(p, SceneSequence_UpdateRampDown64);
        }
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        SceneSequence_BeginAnimation6(p);
    } else if (p->actionOwner->actionMode != 6 && (gPadState[1] & 0xC0)) {
        Sound_PlaySong(0xDC);
        p->accountSelection = 1 - p->accountSelection;
        MenuPrompt_UpdateSelectionSprites(p);
    }
}
