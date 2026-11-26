#ifndef GUARD_ENEMY_DEFEAT_STATISTICS_H
#define GUARD_ENEMY_DEFEAT_STATISTICS_H

#include "global.h"

/* Signed counter view of the game-state block. Category names describe the
 * switch cases that increment each field; their gameplay meanings are tentative.
 * The five reaction counters correspond to collider reaction bits 7 through 11. */
struct EnemyDefeatStatistics {
    u8 unknown0[0x538];
    s16 total;
    s16 category4, category1, category2, category3, category5To7;
    s16 category9, category10, unknown548, category12And13, category14;
    s16 category17, category18, category19, category21, category22;
    s16 category20, category23, category24, unknown55E, category25;
    u8 unknown562[0x28];
    s16 reactionCounts[5];
    u8 unknown594[0x2E8];
    s16 lifetimeTotal;
};

typedef char EnemyDefeatStatistics_CheckCategory[
    (u32)&((struct EnemyDefeatStatistics *)0)->category4 == 0x53A ? 1 : -1];
typedef char EnemyDefeatStatistics_CheckReactions[
    (u32)&((struct EnemyDefeatStatistics *)0)->reactionCounts == 0x58A ? 1 : -1];
typedef char EnemyDefeatStatistics_CheckLifetime[
    (u32)&((struct EnemyDefeatStatistics *)0)->lifetimeTotal == 0x87C ? 1 : -1];

#endif
