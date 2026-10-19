#include "global.h"
#include "player.h"
#include "game_state.h"

#ifdef NONMATCHING
// fuzzy: 83.3%
// foundry draft (worker best 82.4%); what differs is mostly register allocation and instruction order
#define Player_RecalculateDerivedStats sub_0813B4C8
s32 Div(s32, s32);
void sub_0813B4C8(struct Player *p)
{
    s32 i;
    u16 *stats;
    u16 *dest;
    u16 cap;
    if (p->actionMode != 6) {
        struct GameState *state = gUnk_02000710;
        dest = p->derivedStats;
        stats = state->baseStats;
        cap = 120;
        i = 2;
        do {
            *dest = stats[0] + stats[4];
            if (*dest > 120)
                *dest = cap;
            dest++;
            stats++;
        } while (--i >= 0);
    } else {
        s32 level = gUnk_02000710->level + 10;
        s32 remainder, quotient, value;
        if (level > 99)
            level = 99;
        remainder = level - 1;
        remainder *= 2;
        quotient = Div(remainder, 3);
        remainder -= quotient * 3;
        value = quotient + 10;
        dest = p->derivedStats;
        stats = (u16 *)gUnk_02000710;
        cap = 120;
        i = 2;
        do {
            *dest = stats[16] + value;
            if (remainder > 0) {
                (*dest)++;
                remainder--;
            }
            if (*dest > 120)
                *dest = cap;
            dest++;
            stats++;
        } while (--i >= 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0813B4C8);
#endif
