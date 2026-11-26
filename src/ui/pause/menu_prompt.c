#include "global.h"
#include "ui/pause/menu_prompt.h"
#include "system/game_state/game_state.h"
#include "player/player.h"
#include "ui/town/dark_loan_screen.h"
#include "engine/input.h"
#include "engine/video/render.h"

void Hud_SetUpdateCallback(void *, void *);
s32 MessageWindow_ShowMessage(s32);
void Hud_InitSelectionBounds(s32, s32);
s32 Anim_Step(void *, void *);
void SceneHud_FadeOutPage(void *);
void Hud_SetupBackgroundLayer(void *, u32, s32, s32, u32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);

void PauseMenu_HideAllSprites(void *);

void MenuPrompt_BeginPulseDisplay(u8 *hud)
{
    u16 zero;
    PauseMenu_HideAllSprites(hud);
    Hud_SetupBackgroundLayer(hud, *(u32 *)(hud + 0x24), 0, 5, *(u32 *)(hud + 0x34));
    *(u32 *)(hud + 0x3be8) &= ~1;
    *(u16 *)(hud + 0x3c00) = zero = 0;
    *(u16 *)(hud + 0x3c02) = zero;
    Anim_SetKeyframes(hud + 0x3be0, hud + 0xc4, 6, 2, zero);
    MessageWindow_ShowMessage(13);
    Hud_SetUpdateCallback(hud, MenuPrompt_PulseDisplay);
}

void MenuPrompt_RampDisplay(struct SceneHudState *p)
{
    if (p->pageFadeTimer <= 15)
        Hud_InitSelectionBounds(p->pageFadeTimer * 4, 0);
    else
        Hud_InitSelectionBounds(64, 0);
    Anim_Step(&p->accountAnimation, &p->accountResource);
    if (p->pageFadeTimer++ > 15)
        Hud_SetUpdateCallback(p, MenuPrompt_UpdateChoice);
}

int PauseMenu_UpdateBinaryChoice(struct SceneHudState *);

void Sound_PlaySong(u32);

void ScenePanel_BeginDetail(struct SceneHudState *);

void MenuPrompt_UpdateChoice(struct SceneHudState *p)
{
    int choice;
    Anim_Step(&p->accountAnimation, &p->accountResource);
    choice = PauseMenu_UpdateBinaryChoice(p);
    if (choice == 0) {
        Sound_PlaySong(0xDE);
        Hud_SetUpdateCallback(p, MenuPrompt_FadeChoice);
    } else if (choice == 1) {
        Sound_PlaySong(0xDD);
        ScenePanel_BeginDetail(p);
    }
}

void MenuPrompt_FadeChoice(struct SceneHudState *p)
{
    if (p->pageFadeTimer <= 15)
        Hud_InitSelectionBounds((16 - p->pageFadeTimer) * 4, 0);
    else
        Hud_InitSelectionBounds(0, 0);
    Anim_Step(&p->accountAnimation, &p->accountResource);
    if (p->pageFadeTimer++ > 15)
        ScenePanel_BeginAccountMenu(p);
}

void StatusPanel_BeginAnimatedTransition(void *);

void ScenePanel_BeginSelection(void *);

void PauseMenu_UpdateQuantityInput(void *);

void ScenePanel_UpdateDetail(struct SceneHudState *p)
{
    Anim_Step(&p->accountAnimation, &p->accountResource);
    if (gPadState[1] & 1) {
        if (p->valueRate == 0)
            Sound_PlaySong(0xDE);
        else {
            s32 *total;
            s32 value;
            p->actionOwner->currentStatB += p->valueRate;
            {
                s32 *balance = &gGameState->menuRewardBalance;
                *balance += p->valueRate * p->selectedQuantity;
            }
            total = &gGameState->unk51C;
            value = *total + p->valueRate * p->selectedQuantity;
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

int MessageWindow_SetNumberArgument(int, int);

void MenuPrompt_FinishDelayedChoice(struct SceneHudState *p)
{
    Anim_Step(&p->accountAnimation, &p->accountResource);
    if (++p->pageFadeTimer > 150) {
        if (p->valueRate) {
            MessageWindow_ShowMessage(5);
            if (p->accountCompletionScript)
                Script_Call(p->accountCompletionScript, 0);
            else
                MessageWindow_SetNumberArgument(0, 3);
            Hud_SetUpdateCallback(p, Hud_UpdateFirstPostScriptWait);
        } else {
            Hud_SetUpdateCallback(p, SceneHud_FadeOutPage);
        }
    }
}

 s32 MessageWindow_Clear(void);

u32 ScrollingDialogue_ClearScript(void);

void ScrollingDialogue_RequestHide(void);

void PauseMenu_RestoreDisplayState(u8 *);

void SceneHud_FinishPageExit(u8 *);

void MenuPrompt_UpdatePageExit(u8 *hud)
{
    u16 *timer;
    if (*(u16 *)(hud + 0xa4c) <= 15)
        Hud_InitSelectionBounds(0x40 - *(u16 *)(hud + 0xa4c) * 4, 0x1084);
    else
        Hud_InitSelectionBounds(0, 0x1084);
    timer = (u16 *)(hud + 0xa4c);
    if (*timer == 15) {
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_RequestHide();
    }
    if (++*timer > 18) {
        PauseMenu_RestoreDisplayState(hud);
        gFadeMultiplier = *(u16 *)(hud + 0xa12);
        Hud_SetUpdateCallback(hud, SceneHud_FinishPageExit);
    }
}

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void MenuPrompt_WaitForChildPhase(struct SceneHudState *p)
{
    if (ScriptPanel_IsCallbackActive(p->loanDisplay)) {
        ScreenTransition_SendCommand(2, 4, 4, 4, 4, 65535, 0);
        ScenePanel_BeginAccountMenu(p);
    }
}

void Hud_UpdateFirstPostScriptWait(u8 *s)
{
    Anim_Step(s + 0x3BE0, s + 0xC4);
    (*(u16 *)(s + 0xA4C))++;
    if (*(u16 *)(s + 0xA4C) > 0x95)
        Hud_SetUpdateCallback(s, SceneHud_FadeOutPage);
}

void MenuPrompt_PulseDisplay(struct SceneHudState *p)
{
    Anim_Step(&p->accountAnimation, &p->accountResource);
    if (p->pageFadeTimer <= 15)
        Hud_InitSelectionBounds(p->pageFadeTimer * 4, 0);
    else if (p->pageFadeTimer <= 133)
        Hud_InitSelectionBounds(64, 0);
    else
        Hud_InitSelectionBounds((150 - p->pageFadeTimer) * 4, 0);
    if (p->pageFadeTimer++ > 149)
        ScenePanel_BeginAccountMenu(p);
}

s32 Hud_GetScaledRetryCount(void)
{
    s16 v = gGameState->unk876;
    s32 r;
    if (v <= 1)
        r = 9;
    else if (v <= 9)
        r = 11 - v;
    else
        r = 2;
    return r;
}

s32 MessageWindow_Clear(void);

u32 ScrollingDialogue_ClearScript(void);

void PauseMenu_HideAllSprites(void *);

void MenuPrompt_ResumeChild(struct SceneHudState *p)
{
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    if (ScriptPanel_IsCallbackActive(p->loanDisplay)) {
        PauseMenu_HideAllSprites(p);
        DarkLoan_OpenDisplay(p->loanDisplay);
        Hud_SetUpdateCallback(p, MenuPrompt_WaitForChildPhase);
    }
}

void MenuPrompt_UpdateSelectionSprites(struct SceneHudState *p)
{
    Anim_LoadKeyframe(&p->accountChoices[0], &p->accountResource,
                      p->accountSelection + 25, 1);
    Anim_LoadKeyframe(&p->accountChoices[1], &p->accountResource,
                      (u16)(28 - p->accountSelection), 1);
    p->accountCursor.position.x = 0;
    p->accountCursor.position.y = p->accountSelection << 5;
}

void ScenePanel_UpdateAccountIntro(void);

void Hud_InitRetryPanel(u8 *s)
{
    u32 zero;
    u16 *p;
    u8 *a, *b, *initial;

    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    MessageWindow_SetBounds(1, 1, 28, 6);
    MessageWindow_SetMessageTable(*(u32 *)(s + 0x58));
    MessageWindow_SetDisplayMode(1);
    Hud_SetupBackgroundLayer(s, *(u32 *)(s + 0x18), 0, 2, *(u32 *)(s + 0x2C));
    PauseMenu_HideAllSprites(s);
    *(u32 *)(s + 0x3BE8) &= ~1;
    p = (u16 *)(s + 0x3C00);
    zero = 0;
    {
        u32 v = 0xFFA8;
        *p = v;
    }
    *(u16 *)(s + 0x3C02) = 0x20;
    initial = s + 0x3BE0;
    b = s + 0xC4;
    Anim_SetKeyframes(initial, b, 4, 2, zero);
    a = s + 0x3D00;
    Anim_LoadKeyframe(a, b, 0x1D, 1);
    Anim_SetKeyframes(a, b, 10, 1, zero);
    MessageWindow_ShowMessage(0);
    Hud_SetUpdateCallback(s, ScenePanel_UpdateAccountIntro);
}

s32 MessageWindow_SetBounds(s32, s32, s32, s32);

s32 MessageWindow_SetMessageTable(const void *);

s32 MessageWindow_SetDisplayMode(s32);

void SceneSequence_UpdateRamp64(void);

void ScenePanel_BeginAccountMenu(struct SceneHudState *p)
{
    u32 zero;
    s16 *coord;
    struct SceneInputWidget *anim;
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    MessageWindow_SetBounds(1, 1, 28, 6);
    MessageWindow_SetMessageTable(p->optionLScript);
    MessageWindow_SetDisplayMode(1);
    Hud_SetupBackgroundLayer(p, p->panelResource, 0, 2, p->panelConfig);
    PauseMenu_HideAllSprites(p);
    p->accountAnimation.flags &= ~1;
    coord = &p->accountAnimation.position.x;
    zero = 0;
    {
        u32 v = 0xFFE4;
        *coord = v;
    }
    p->accountAnimation.position.y = 32;
    Anim_SetKeyframes(&p->accountAnimation, &p->accountResource, 7, 2, zero);
    p->accountChoices[0].flags &= ~1;
    if (p->actionOwner->actionMode != 6)
        p->accountChoices[1].flags &= ~1;
    p->accountCursor.flags &= ~1;
    anim = &p->accountCursor;
    Anim_LoadKeyframe(anim, &p->accountResource, 0x1D, 1);
    Anim_SetKeyframes(anim, &p->accountResource, 10, 1, zero);
    MenuPrompt_UpdateSelectionSprites(p);
    MessageWindow_ShowMessage(7);
    Hud_SetUpdateCallback(p, SceneSequence_UpdateRamp64);
}
