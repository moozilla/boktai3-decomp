#include "global.h"
#define MenuPrompt_SetPhase Hud_SetUpdateCallback
#define MenuPrompt_SetDisplayLevels Hud_InitSelectionBounds
struct MenuPrompt {
    u8 unk0[0xC4];
    u8 animData[0xA4C - 0xC4];
    u16 timer;
    u8 unkA4E[0x3BE0 - 0xA4E];
    u8 anim[4];
};
void Hud_InitSelectionBounds(u32, u32);
s32 Anim_Step(void *, void *);
void MenuPrompt_UpdateChoice(struct MenuPrompt *);
void Hud_SetUpdateCallback(struct MenuPrompt *, void (*)(struct MenuPrompt *));
void MenuPrompt_RampDisplay(struct MenuPrompt *p)
{
    if (p->timer <= 15)
        Hud_InitSelectionBounds(p->timer * 4, 0);
    else
        Hud_InitSelectionBounds(64, 0);
    Anim_Step(p->anim, p->animData);
    if (p->timer++ > 15)
        Hud_SetUpdateCallback(p, MenuPrompt_UpdateChoice);
}
