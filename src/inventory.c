#include "global.h"
#include "ram.h"
#include "scene_input_grid.h"
#include "player.h"
#include "scene_hud_display.h"
#include "game_state.h"
#include "engine.h"

s32 Script_SeekToKeyword(s32);

s32 Script_GetValue(void);

void Inventory_SetUsePermissionFlagsFromScript(void)
{
    s32 i;
    gInventoryUseFlags = 0;
    if (Script_SeekToKeyword(0x66)) {
        for (i = 0; i <= 3; i++) {
            if (Script_GetValue())
                gInventoryUseFlags |= 1 << i;
        }
    }
}

u32 Inventory_GetUsePermissionFlag(u32 bit)
{
    return gInventoryUseFlags & (1 << bit);
}

void Inventory_InitializeSelectionGrid(struct SceneInputGrid *grid, s32 identity, u32 kind)
{
    s32 i, j;
    grid->kind = kind;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 9; j++)
            grid->cells[j][i] |= 0xFFu;
    if (identity) {
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                grid->cells[j][i] = i * 4 + j;
    }
    if (kind & 1)
        grid->cells[8][0] = 0x20;
    else
        grid->cells[8][0] = 0xFF;
    if (kind & 0x20)
        grid->cells[8][1] = 0x26;
    else
        grid->cells[8][1] = 0xFF;
    if (kind & 0x40)
        grid->cells[8][2] = 0x27;
    else
        grid->cells[8][2] = 0xFF;
    if (kind & 0x80)
        grid->cells[8][3] = 0x28;
    else
        grid->cells[8][3] = 0xFF;
}

/* Inventory_GetItemCategory: Extracts the low category nibble from an item definition; category meaning tentative */


extern const u16 gUnk_08E61280[];

u32 Inventory_GetItemCategory(s32 i)
{
    return gUnk_08E61280[i * 8] & 15;
}

u32 Inventory_GetItemUseCondition(u32 index)
{
    return (gUnk_08E61280[index * 8] & 0xF0) >> 4;
}

u32 Inventory_GetKeyItemId(u32);

u32 Inventory_GetItemId(u32);

u32 Inventory_GetQuantityByStorage(u32 condition, u32 value)
{
    if (condition == 0)
        return Inventory_GetItemId(value);
    return Inventory_GetKeyItemId(value);
}

s32 Div(s32, s32);

void Hud_DrawDefaultHealthMeter(void *, s32, s32, s32);

struct QCECC {
    u8 f[0x424];
    u16 done;
    u16 total;
};

struct SCECC {
    u8 f[0xa40];
    struct QCECC *q;
    u8 f2[0x4866 - 0xa44];
    s16 value;
};

void Inventory_RestoreLifePercent(struct SCECC *s, s32 n)
{
    struct QCECC *q = s->q;
    s32 v;
    s32 sum;
    if (q == 0)
        return;
    v = Div(n * q->total, 100);
    s->value = v;
    sum = q->done + s->value;
    if (sum > q->total) {
        s->value = q->total - q->done;
        q->done = q->total;
    } else {
        q->done = sum;
    }
    Hud_DrawDefaultHealthMeter(s, 1, 2, 12);
}

void sub_0813D834(void *, s32, s32);

struct QCF2C {
    u8 f[0x424];
    u16 done;
    u16 total;
};

struct SCF2C {
    u8 f[0xa40];
    struct QCF2C *q;
    u8 f2[0x4866 - 0xa44];
    s16 value;
};

void Inventory_RestoreLifePercentTimed(struct SCF2C *s, s32 n, s32 extra)
{
    struct QCF2C *q = s->q;
    s32 v;
    s32 sum;
    if (q == 0)
        return;
    v = Div(n * q->total, 100);
    s->value = v;
    sum = q->done + s->value;
    if (sum > q->total) {
        s->value = q->total - q->done;
        q->done = q->total;
    } else {
        q->done = sum;
    }
    Hud_DrawDefaultHealthMeter(s, 1, 2, 12);
    if (extra > 0)
        sub_0813D834(q, 0, extra * 60);
}

void Hud_DrawDefaultEnergyMeter(struct SceneHudState *, s32, s32, s32, s32, s32);

void SceneHud_ApplyStatBPercent(struct SceneHudState *s, s32 percent, s32 duration)
{
    struct Player *p = s->actionOwner;
    s32 value;
    if (p) {
        s->appliedStatB = Div(p->maximumStatB * percent, 100);
        value = p->currentStatB + Div(p->maximumStatB * percent, 100);
        if (value > p->maximumStatB) {
            s->appliedStatB = p->maximumStatB - p->currentStatB;
            p->currentStatB = p->maximumStatB;
        } else if (value < 0) {
            p->currentStatB = 0;
        } else {
            p->currentStatB = value;
        }
        if (s->appliedStatB < 0)
            s->appliedStatB = 0;
        Hud_DrawDefaultEnergyMeter(s, 1, 2, 14, s->actionOwner->currentStatB,
                                   s->actionOwner->maximumStatB);
        if (duration > 0) {
            if (Inventory_GetQuantityByStorage(s->actionCategory, s->actionSelection) == 14)
                sub_0813D834(p, 1, duration * 60);
            else
                sub_0813D834(p, 0, duration * 60);
        }
    }
}

#define Player_ClearScriptStatus Player_ClearStateTimer

struct PlayerStatusPanel {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[0x3E22];
    u16 resetState;
};

void Player_ClearScriptStatus(struct Player *, s32);

void PlayerStatusPanel_ResetScriptCounters(struct PlayerStatusPanel *panel, s32 mask)
{
    struct Player *player = panel->player;
    s32 i;
    if (player) {
        panel->resetState = 0;
        for (i = 0; i <= 1; i++) {
            if ((mask >> i) & 1)
                Player_ClearScriptStatus(player, i);
        }
        if (mask & 8) {
            u32 index = player->timedEventSlot;
            struct GameState *state = gGameState;
            u32 address = index * 4 + (u32)state;
            *(u32 *)(address + 0x628) = 0;
        }
    }
}

#define SceneHud_GetSelectedAction Inventory_GetQuantityByStorage

void SceneHud_ApplyTimedSceneMode(struct SceneHudState *s, s32 mode, s32 duration)
{
    struct Player *p = s->actionOwner;
    if (p) {
        p->unk536 = Inventory_GetQuantityByStorage(s->actionCategory, s->actionSelection);
        p->sceneMode = mode;
        p->active = duration * 60;
        p->actionEighteenParameter = 366;
        p->actionEighteenDuration = 48;
        s->appliedStatB = 0;
        s->actionRefreshRequested = 1;
        s->statusRefreshRequested = 1;
    }
}

struct StatBlock {
    u8 pad[0x18];
    s16 base[4];
    s16 bonus[4];
};

s32 Mod(s32, s32);

static inline int nextRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}

s32 SceneHud_SelectUpgradeableStat(s32 amount)
{
    s32 start, i, a;
    struct StatBlock *g;
    a = amount;
    start = Mod(nextRandom() >> 3, 3);
    g = gGameState;
    {
        s16 *e = (s16 *)(start * 2 + (u32)g);
        if (e[12] + e[16] + a > 100)
            a = 100 - e[12] - e[16];
    }
    if (a > 0)
        return start;
    for (i = 0; i <= 2; i++) {
        a = amount;
        {
            s16 *e = (s16 *)(i * 2 + (u8 *)g);
            if (e[12] + e[16] + a > 100)
                a = 100 - e[12] - e[16];
        }
        if (a > 0)
            return i;
    }
    return 0xff;
}

#define Player_RecalculateStats Player_RefreshStatsAndVitals

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

struct QD308 {
    u8 f[0x418];
    u8 mode;
};

struct SD308 {
    u8 f0[0xa40];
    struct QD308 *q;
    u8 f1[0x42a0 - 0xa44];
    u8 state2;
    u8 f2[0x4870 - 0x42a1];
    u8 state;
    u8 x;
    u16 y;
};

void Inventory_BeginItemEffect(struct SD308 *s, u32 x, u32 y)
{
    struct QD308 *q = s->q;
    if (q == 0)
        return;
    s->x = x;
    if (q->mode == 6) {
        if ((u8)x == 0)
            s->x = 2;
        else if ((u8)x == 2)
            s->x = 0;
    }
    s->y = y;
    s->state = 1;
    s->state2 = 1;
}
