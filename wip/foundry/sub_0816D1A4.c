#define SceneHud_SelectUpgradeableStat sub_0816D1A4
#define gRandomTableIndex gUnk_03005308
#define gRandomTable gUnk_0203B400
#define gGameState gUnk_02000710
#include "global.h"
#include "game_state.h"
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern struct GameState *gUnk_02000710;
s32 Mod(s32, s32);
static inline s32 RandomValue(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400);
}
s32 sub_0816D1A4(s32 amount)
{
    s32 remaining = amount;
    u32 index;
    struct GameState *state;
    s32 i;
    s32 result;
    struct GameState *entry;
    s16 *stats;
    s32 base, bonus;
    index = Mod(RandomValue() >> 3, 3);
    state = gUnk_02000710;
    entry = (struct GameState *)(index * 2 + (u32)state);
    base = (s16)entry->baseStats[0];
    bonus = (s16)entry->statBonuses[0];
    if (base + bonus + remaining > 100)
        remaining = 100 - base - bonus;
    if (remaining > 0) {
        result = index;
        goto done;
    }
    stats = (s16 *)state->baseStats;
    for (i = 0; i < 3; i++) {
        remaining = amount;
        base = stats[0];
        bonus = stats[4];
        if (base + bonus + remaining > 100)
            remaining = 100 - base - bonus;
        if (remaining > 0) {
            result = i;
            goto done;
        }
        stats++;
    }
    result = 255;
done:
    return result;
}
