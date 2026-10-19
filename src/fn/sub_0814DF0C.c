
#ifdef NONMATCHING
// fuzzy: 89.2%
// foundry draft (worker best 86.1%); what differs is mostly register allocation and instruction order
#define Player_IsStatBBelow sub_0814DF0C
#include "global.h"
#include "player.h"
#include "game_state.h"
extern struct GameState *gUnk_02000710;
s32 sub_0814DF0C(struct Player *p, s32 threshold)
{
    s32 value;
    if (p->statModifierContext.statBOverrideEnabled && p->statModifierContext.statBOverrideMode == 7)
        value = gUnk_02000710->unk77C;
    else
        value = p->currentStatB;
    if (value < threshold) return 1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0814DF0C);
#endif
