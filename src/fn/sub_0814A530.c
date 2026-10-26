#define ActionEntry_CountUsedModifiers Inventory_CountEquipmentEffectEntries
#define gPlayerSceneFlags              gEngineFlags
#include "global.h"
#include "player.h"
#include "game_state.h"
#include "scene_inventory_actions.h"
#include "engine.h"
s32 Inventory_CountEquipmentEffectEntries(struct PlayerActionEntry *);
s32 Div(s32, s32);
static inline s32 ScaleActionStat(s32 product)
{
    s32 result;
    if (product >= 0)
        result = product >> 6;
    else
        result = -((-product) >> 6);
    return result;
}
s32 Player_CalculateActionStat(struct Player *p, struct PlayerStatModifierContext *ctx, s32 bonus)
{
    struct SceneInventoryActionDefinition *def;
    s32 count, value, i;
    if (!p) {
        def = Equipment_GetDefinition(ctx->actionEntry->kind);
        count = -1;
    } else {
        def = Equipment_GetDefinition(p->statModifierContext.actionEntry->kind);
        count = Inventory_CountEquipmentEffectEntries(p->statModifierContext.actionEntry);
    }
    if (def->kind == 0x2F)
        value = gGameState->level + 5;
    else {
        value = def->statValue;
        if (p && !p->statModifierContext.actionEntry->remainingCharge)
            value = 1;
    }
    i = 0;
    {
        u32 adjustedCount = count + 1;
        bonus *= 2;
        do {
            if (ctx->statModifierCallbacks[i])
                value += sub_08249240(ctx, (void *)value, ctx->statModifierCallbacks[i]);
            i++;
        } while (i <= 2);
        switch (adjustedCount) {
            case 0:
            case 1:
            case 2:
            default:
                break;
            case 3:
                if (!p->actionResetCounter)
                    value = ScaleActionStat(value * 58);
                break;
            case 4:
            case 5:
            case 6:
                if (!p->actionResetCounter)
                    value = ScaleActionStat(value * 51);
                else if (p->actionResetCounter == 1)
                    value = ScaleActionStat(value * 58);
                else if (p->actionResetCounter == 3)
                    value = ScaleActionStat(value * 70);
                else if (p->actionResetCounter == 4)
                    value = ScaleActionStat(value * 77);
                break;
        }
    }
    value += Div(gGameState->level + bonus, 3);
    if ((gEngineFlags & 0x1000) && def->kind == 0x24)
        value -= 15;
    return value;
}
