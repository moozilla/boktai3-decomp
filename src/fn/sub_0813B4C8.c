#include "global.h"
#include "player.h"
#include "game_state.h"

// MATCHING_CONCESSION: register-pinning
#define Player_RecalculateDerivedStats sub_0813B4C8

s32 Div(s32, s32);
void Player_RecalculateDerivedStats(struct Player *p)
{
    register s32 i asm("r3");
    register u32 offset asm("r1") = 0x418;
    if (*((u8 *)p + offset) != 6) {
        struct GameState *state = gUnk_02000710;
        register u16 *dest asm("r2");
        register u16 *stats asm("r1");
        register u16 cap asm("r4");
        offset += 4;
        dest = (u16 *)((u8 *)p + offset);
        stats = state->baseStats;
        cap = 120;
        i = 2;
        do {
            s32 sum = stats[4] + stats[0];
            *dest = sum;
            if ((u16)sum > 120)
                *dest = cap;
            dest++;
            stats++;
        } while (--i >= 0);
    } else {
        s32 level = gUnk_02000710->level + 10;
        s32 remainder, quotient, value;
        u16 *dest;
        register u16 *stats asm("r2");
        u16 cap;
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
