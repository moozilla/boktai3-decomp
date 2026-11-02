#include "global.h"
#include "player.h"
#include "game_state.h"

#define STAT_BONUS_OFFSET ((u32) & ((struct GameState *)0)->statBonuses)

s32 Div(s32, s32);

static inline void RecalculateBaseStats(struct Player *p)
{
    struct GameState *state = gGameState;
    s32 idx;
    for (idx = 0; idx < 3; idx++) {
        u16 *out = &p->derivedStats[idx];
        *out = state->baseStats[idx] + state->statBonuses[idx];
        if (*out > 120)
            *out = 120;
    }
}

void Player_RecalculateDerivedStats(struct Player *p)
{
    s32 i;
    if (p->actionMode != 6) {
        RecalculateBaseStats(p);
    } else {
        s32 level = gGameState->level + 10;
        s32 remainder, quotient, value;
        u16 *dest;
        const u8 *statCursor;
        u16 cap;
        if (level > 99)
            level = 99;
        remainder = level - 1;
        remainder *= 2;
        quotient = Div(remainder, 3);
        remainder -= quotient * 3;
        value = quotient + 10;
        dest = p->derivedStats;
        statCursor = (const u8 *)gGameState;
        cap = 120;
        for (i = 0; i < 3; i++) {
            *dest = *(const u16 *)(statCursor + STAT_BONUS_OFFSET) + value;
            if (remainder > 0) {
                (*dest)++;
                remainder--;
            }
            if (*dest > 120)
                *dest = cap;
            dest++;
            statCursor += sizeof(u16);
        }
    }
}
