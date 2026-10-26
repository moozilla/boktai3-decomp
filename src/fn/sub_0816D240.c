#define Player_RecalculateStats Player_RefreshStatsAndVitals
#include "global.h"
#include "player.h"
#include "game_state.h"
#include "scene_hud_display.h"
s32 SceneHud_SelectUpgradeableStat(s32);
void Player_RefreshStatsAndVitals(struct Player *);
void Hud_DrawStatusMetersAndCounts(struct SceneHudState *, s32, s32);
void sub_08166534(void *, s32, s32);
void InventoryPanel_DrawStatComparison(void *, s32, s32, s32, s32, s32);
static inline s32 ClampIncrease(s32 index, s32 amount)
{
    struct GameState *state = gGameState;
    struct GameState *entry = (struct GameState *)(index * 2 + (u32)state);
    s32 base = (s16)entry->baseStats[0];
    s32 bonus = (s16)entry->statBonuses[0];
    if (base + bonus + amount > 100)
        amount = 100 - base - bonus;
    return amount;
}
void SceneHud_IncreaseStatBonus(struct SceneHudState *s, s32 index, s32 amount)
{
    struct Player *p = s->actionOwner;
    struct GameState *entry;
    s32 increase;
    if (p) {
        if (index <= 2)
            s->selectedStat = index;
        else {
            s->selectedStat = SceneHud_SelectUpgradeableStat(amount);
            if (s->selectedStat == 255)
                s->selectedStat = 0;
        }
        s->appliedStatIncrease = amount;
        increase = s->appliedStatIncrease;
        increase = ClampIncrease(s->selectedStat, increase);
        s->appliedStatIncrease = increase;
        entry = (struct GameState *)(s->selectedStat * 2 + (u32)gGameState);
        entry->statBonuses[0] += increase;
        Player_RefreshStatsAndVitals(p);
        Hud_DrawStatusMetersAndCounts(s, 2, 12);
        sub_08166534(s, 10, 6);
        InventoryPanel_DrawStatComparison(s, 0, 0, 0, 1, 0);
    }
}
