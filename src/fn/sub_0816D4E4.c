// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "game_state.h"
#include "scene_item_values.h"

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
static inline s32 InventoryPresent(s16 *p)
{
    if (*p >= 0)
        return 1;
    return 0;
}
static inline struct SceneValueEquipment *GetEquipment(s16 *p)
{
    asm("" ::: "memory");
    return Inventory_GetEquipmentEntry(*p);
}
u32 SceneHud_CanApplySelectedAction(u32 action)
{
    struct Player *p = gPlayers[0];
    s32 i;
    struct GameState *entry;
    s16 *inventory;
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
                if (p->scriptStatus[i] && ((s32)gUnk_08E61280[action].unknown6 >> i & 1))
                    return 1;
            }
            if (GetTimedCounter(p->timedEventSlot) > 0 && (gUnk_08E61280[action].unknown6 & 8))
                return 1;
            break;
        case 5:
            entry = (struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState);
            inventory = (s16 *)entry->inventory;
            if (InventoryPresent(inventory))
                GetEquipment(inventory);
            break;
    }
    return 0;
}
