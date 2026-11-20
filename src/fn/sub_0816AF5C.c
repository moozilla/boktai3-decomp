#include "menu_prompt.h"
#include "player.h"
void Hud_InitSelectionBounds(u32, u32);
void Sound_PlaySong(u32);
s32 Anim_Step(void *, void *);
s32 MessageWindow_ShowMessage(s32);
void SelectionMenu_UpdateConfirmInput(struct SceneHudState *);
void Hud_SetUpdateCallback(struct SceneHudState *, void (*)(struct SceneHudState *));
void ScenePanel_UpdateAccountIntro(struct SceneHudState *p)
{
    if (p->pageFadeTimer <= 15)
        Hud_InitSelectionBounds(p->pageFadeTimer * 4, 0);
    else
        Hud_InitSelectionBounds(64, 0);
    if (p->pageFadeTimer == 16)
        Sound_PlaySong(0x2C0);
    Anim_Step(&p->accountAnimation, &p->accountResource);
    if (p->pageFadeTimer > 104)
        p->accountAnimation.position.x += 5;
    if (++p->pageFadeTimer > 116) {
        MessageWindow_ShowMessage(7);
        p->accountAnimation.position.x = 0xFFE4;
        p->accountCursor.flags &= ~1;
        p->accountChoices[0].flags &= ~1;
        if (p->actionOwner->actionMode != 6)
            p->accountChoices[1].flags &= ~1;
        p->accountSelection = 0;
        MenuPrompt_UpdateSelectionSprites(p);
        Hud_SetUpdateCallback(p, SelectionMenu_UpdateConfirmInput);
    }
}
