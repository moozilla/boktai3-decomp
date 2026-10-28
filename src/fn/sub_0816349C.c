#include "global.h"
#include "player.h"
#include "input.h"
struct ScriptCallArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
void Script_Call(s32, struct ScriptCallArgs *);
struct TransitionPanelSprite {
    u32 flags;
    u8 unknown4[0x4C];
};
struct TransitionPanel {
    u8 task[0x18];
    struct TransitionPanelSprite sprites[8];
    u8 animationData[0x20];
    u8 animation[0x60];
    u8 unknown318[0x18];
    u8 palette[8];
    u8 unknown338[8];
    u8 closeState, state, unknown342, choice;
    u8 unknown344[2];
    u16 timer;
    u16 costs[2];
    u8 selection, pulse;
    u8 unknown34E[2];
    s32 procedure;
};
void Sound_PlaySong(u32);
void MessageWindow_Clear(void);
void Hud_SetMode(s32);
void Player_AdvanceTetherPhase(struct Player *);
void Sound_FadeOutChannel10(s32);
void Task_Kill(void *);
void Anim_LoadKeyframe(void *, void *, s32, s32);
void Palette_UpdateHighlightPulse(void *, void *);
void TransitionPanel_UpdateCostSelection(struct TransitionPanel *p)
{
    s32 decision = -1;
    if (gPadState[1] & 1) {
        u32 choice = p->choice;
        decision = 1;
        if (choice != 0)
            decision = 0;
        Sound_PlaySong(221);
    } else if (gPadState[1] & 2) {
        decision = 0;
        Sound_PlaySong(222);
    }
    if (decision == 0) {
        u32 mask;
        *(u32 *)(p->animation + 8) |= 1;
        mask = 1;
        MessageWindow_Clear();
        Hud_SetMode(7);
        Player_AdvanceTetherPhase(gPlayers[0]);
        Sound_FadeOutChannel10(2);
        p->timer = decision;
        p->choice = mask;
        p->state = 6;
        p->closeState = 4;
    } else if (decision == 1) {
        struct TransitionPanelSprite *sprite;
        s32 i;
        u32 mask;
        Sound_FadeOutChannel10(2);
        *(u32 *)(p->animation + 8) |= decision;
        mask = 1;
        sprite = p->sprites;
        for (i = 7; i >= 0; i--) {
            sprite->flags |= mask;
            sprite++;
        }
        MessageWindow_Clear();
        Hud_SetMode(7);
        if (p->procedure) {
            struct ScriptCallArgs args;
            s32 value;
            args.count = 1;
            value = p->costs[p->selection];
            args.values = &value;
            Script_Call(p->procedure, &args);
        }
        Task_Kill(p);
    } else {
        u32 changed = 0;
        if (gPadState[1] & 32)
            changed = p->choice != 0;
        else if ((gPadState[1] & 16) && !p->choice)
            changed = 1;
        if (changed) {
            Sound_PlaySong(220);
            p->choice = 1 - p->choice;
            if (p->choice)
                Anim_LoadKeyframe(p->animation, p->animationData, 76, 1);
            else
                Anim_LoadKeyframe(p->animation, p->animationData, 75, 1);
        }
        Palette_UpdateHighlightPulse(p->palette, &p->pulse);
    }
}
