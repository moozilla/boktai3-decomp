#include "global.h"
#include "player.h"
#include "game_state.h"

// MATCHING_CONCESSION: register-pinning
#define Player_IsStatBBelow sub_0814DF0C
extern struct GameState *gUnk_02000710;
s32 Player_IsStatBBelow(struct Player *p, s32 threshold)
{
    s32 value;
    if (!p->statModifierContext.statBOverrideEnabled)
        goto current;
    asm("" ::: "r3");
    if (p->statModifierContext.statBOverrideMode == 7)
        value = gUnk_02000710->unk77C;
    else {
    current:
        value = p->currentStatB;
    }
    if (value < threshold)
        return 1;
    return 0;
}
