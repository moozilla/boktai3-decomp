
#ifdef NONMATCHING
// fuzzy: 87.5%
// foundry draft (worker best 83.9%); what differs is mostly register allocation and instruction order
#define Player_SubtractStatB sub_0814DF58
#include "global.h"
#include "player.h"
#include "game_state.h"
extern struct GameState *gUnk_02000710;
void sub_0814DF58(struct Player *p, s32 amount)
{
    s32 value;
    if (p->statModifierContext.statBOverrideEnabled &&
        p->statModifierContext.statBOverrideMode == 7)
        value = gUnk_02000710->unk77C;
    else
        value = p->currentStatB;
    if (value < amount)
        value = 0;
    else
        value -= amount;
    if (p->statModifierContext.statBOverrideEnabled &&
        p->statModifierContext.statBOverrideMode == 7)
        gUnk_02000710->unk77C = value;
    else
        p->currentStatB = value;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0814DF58);
#endif
