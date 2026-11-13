#include "global.h"
#include "player.h"
#include "game_state.h"
#include "scene_item_values.h"
#include "scene_hud_display.h"

void PauseMenu_RequestEffectMode(struct SceneHudState *p, u8 mode)
{
    p->primaryEffectRequested = 1;
    p->primaryEffectMode = mode;
    p->statusRefreshRequested = 1;
}

void PauseMenu_ApplyCharacterSelection(struct SceneHudState *state)
{
    u8 status = state->actionTransferBonus;
    if (status == 0) {
        *(u16 *)((u8 *)gGameState + 0x610) = 1;
        *(u16 *)((u8 *)gGameState + 0x612) = status;
    } else {
        *(u16 *)((u8 *)gGameState + 0x610) = 0;
        *(u16 *)((u8 *)gGameState + 0x612) = 1;
    }
}


/* PauseMenu_RequestRecoveryRefresh: Clears the accumulated recovery value and sets pending refresh flags; recovery interpretation is tentative */


void PauseMenu_RequestRecoveryRefresh(struct SceneHudState *state)
{
    state->appliedStatB = 0;
    state->pageReturnToParent = 1;
    state->statusRefreshRequested = 1;
}

s32 Inventory_HasItem(s32);

void PlayerStatusPanel_RestoreStats(struct SceneHudState *panel)
{
    panel->actionOwner->currentStatA = panel->savedStatA;
    panel->actionOwner->currentStatB = panel->savedStatB;
    if (panel->actionOwner->currentStatA == 0) {
        panel->statusRefreshRequested = 1;
        if (Inventory_HasItem(0x26))
            panel->effectActive = 0;
    } else {
        panel->effectActive = 0;
    }
}

void PauseMenu_RequestSecondaryEffectMode(struct SceneHudState *state, u32 mode)
{
    state->secondaryEffectRequested = 1;
    state->secondaryEffectMode = mode;
    state->statusRefreshRequested = 1;
}

void Inventory_SetItemStateValue(u32, u32);

void PauseMenu_ClearItemSlotValues(void)
{
    s32 i;
    for (i = 0; i <= 15; i++)
        Inventory_SetItemStateValue(i, 0);
}

void PauseMenu_ClearEquippedItemState(void)
{
    s16 i = (s16)gGameState->unk50;
    s16 v = *(s16 *)((u8 *)gGameState + 0x68 + i * 2);
    struct SceneValueEquipment *equipment = Inventory_GetEquipmentEntry(v);
    equipment->condition = 0;
}

#define SceneHud_TestActionFlag      Inventory_GetUsePermissionFlag

#define SceneHud_GetActionEffectKind Inventory_GetItemUseCondition

#define gActionDefinitions           gUnk_08E61280

extern const struct SceneValueItemDefinition gUnk_08E61280[];

u32 Inventory_GetUsePermissionFlag(u32);

u32 Inventory_GetItemUseCondition(u32);

static inline s32 GetTimedCounter(u32 slot)
{
    struct GameState *state = gGameState;
    struct GameState *entry = (struct GameState *)(slot * 4 + (u32)state);
    return entry->counters628[0];
}

u32 SceneHud_CanApplySelectedAction(u32 action)
{
    struct Player *p = gPlayers[0];
    s32 i;
    if (!p)
        return 0;
    if (action == 26) {
        if (Inventory_GetUsePermissionFlag(0))
            goto success;
        return 0;
    }
    if (action - 27 <= 2) {
        if (Inventory_GetUsePermissionFlag(2))
            goto success;
        return 0;
    }
    if (action == 39) {
        if (Inventory_GetUsePermissionFlag(3))
            goto success;
        return 0;
    }
    if (action == 41)
        return 0;
    if (action == 13 && p->actionMode == 6)
        return 1;
    switch (Inventory_GetItemUseCondition(action)) {
        case 2:
            if (p->currentStatA < p->maximumStatA)
                return 1;
            break;
        case 3:
            if (p->currentStatB < p->maximumStatB)
                return 1;
            break;
        case 1:
        success:
            return 1;
        case 4:
            for (i = 0; i < 2; i++) {
                if (p->scriptStatus[i] && ((s32)gUnk_08E61280[action].effectValueA >> i & 1))
                    return 1;
            }
            if (GetTimedCounter(p->timedEventSlot) > 0 &&
                (gUnk_08E61280[action].effectValueA & 8))
                return 1;
            break;
        case 5: {
            s16 *selected = ((struct GameState *)((s16)gGameState->unk50 * 2 +
                                                  (u32)gGameState))
                                ->inventory;
            if (*selected < 0)
                return 0;
            Inventory_GetEquipmentEntry(*selected);
        } break;
    }
    return 0;
}

void Inventory_ClearItemSlot(u32);

void PauseMenu_ClearItemSlot(struct SceneHudState *state, u32 value)
{
    if (state->actionCategory == 0)
        Inventory_ClearItemSlot(value);
}

// confidence=low: applies the selected storage item effects and clears its menu slot.


extern void *const gUnk_08611BF0[];

s32 Inventory_GetQuantityByStorage(u32, u32);

void sub_0813D834(struct Player *, s32, u32);

void sub_08249244(void *, s32, s32, void *);

void Inventory_ClearItemOverrideFlag(u32);

static inline u32 Item_HasBonus(u32 *flags, u32 mask)
{
    return *flags & mask;
}

void PauseMenu_ApplyItemSlot(struct SceneHudState *p, u32 slot)
{
    s32 item = Inventory_GetQuantityByStorage(p->actionCategory, slot);
    s32 a, b;
    u8 mode = p->actionOwner->actionMode;

    if (mode == 1 || mode == 3 || mode == 4 || mode == 5) {
        if (item == 1 || item == 13 || item == 22 || item == 19 || item == 20 || item == 18 ||
            item == 25 || item == 2 || item == 23 || item == 24) {
            s16 *applied;
            s32 duration = 0x708;
            if (p->actionOwner && Item_HasBonus(&p->actionOwner->statBonusFlags, 0x20000))
                duration <<= 1;
            sub_0813D834(p->actionOwner, 0, duration);
            applied = &p->appliedStatB;
            *applied = 0;
            goto clear;
        }
        if (item == 14) {
            s32 duration = 0x384;
            if (p->actionOwner && Item_HasBonus(&p->actionOwner->statBonusFlags, 0x20000))
                duration <<= 1;
            sub_0813D834(p->actionOwner, 1, duration);
            p->appliedStatB = 0;
            goto clear;
        }
    }
    a = gUnk_08E61280[item].effectValueA;
    b = gUnk_08E61280[item].effectValueB;
    if (p->actionOwner->actionMode == 6 && item == 13)
        a = -a;
    if (p->actionOwner && Item_HasBonus(&p->actionOwner->statBonusFlags, 0x20000)) {
        if (item == 1 || item == 13 || item == 2 || item == 14) {
            a <<= 1;
            b <<= 1;
        } else if (item == 22 || item == 19 || item == 20 || item == 18 || item == 25 ||
                   item == 23 || item == 24)
            b <<= 1;
    }
    sub_08249244(p, a, b, gUnk_08611BF0[gUnk_08E61280[item].actionKind]);
    if (item == 9) {
        Inventory_ClearItemOverrideFlag(slot);
        if (Inventory_GetQuantityByStorage(0, slot) != 9)
            return;
        goto clear;
    }
    if ((u32)(item - 10) <= 1) {
    clear:
        PauseMenu_ClearItemSlot(p, slot);
    } else {
        PauseMenu_ClearItemSlot(p, slot);
    }
}

void sub_0822D080(u32, u32);

void Inventory_SwapValuableSlots(u32, u32);

void PauseMenu_SwapItemSlots(struct SceneHudState *state, u32 a, u32 b)
{
    if (state->actionCategory == 0)
        sub_0822D080(a, b);
    else
        Inventory_SwapValuableSlots(a, b);
}
