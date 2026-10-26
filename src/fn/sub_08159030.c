#define Player_ComputeIndexedStatA Player_GetSpecialAttackPower
#include "global.h"
#include "player.h"
#include "game_state.h"
struct PlayerEquipmentReference {
    u8 index;
    u8 unknown1[11];
    u8 slot;
};
void PlayerAction_RefreshFlagState(u32, void *, s32);
s32 Player_CalculateActionStat(s32, struct PlayerStatModifierContext *, s32);
s32 Player_GetSpecialAttackPower(s32, s32);
void ActionRecord_CopyFromEntry(const void *, void *);
void Player_BuildActionModifiers(struct Player *, struct PlayerStatModifierContext *, const void *);
s32 Player_EvaluateItemStatA(struct PlayerActionEntry *item,
                             struct PlayerEquipmentReference *reference)
{
    s32 equipment[4];
    s16 stats[8];
    struct PlayerStatModifierContext context;
    struct PlayerActionDescriptor descriptor;
    s32 index;
    s32 *dst;
    s16 *src;
    s32 i;
    s32 value;
    s16 *statValues;
    struct PlayerStatModifierContext *modifiers;
    struct PlayerActionEntry **entry;
    u8 *override;
    if (item->kind == 0x30)
        return Player_GetSpecialAttackPower(0, 0x30);
    index = reference->index;
    statValues = stats;
    modifiers = &context;
    override = &context.statBOverrideEnabled;
    entry = &context.actionEntry;
    src = gGameState->party;
    dst = equipment;
    for (i = 3; i >= 0; i--)
        *dst++ = *src++;
    if (reference)
        equipment[reference->slot] = index;
    PlayerAction_RefreshFlagState(0, statValues, index);
    ActionRecord_CopyFromEntry(item, &descriptor);
    Player_BuildActionModifiers(0, modifiers, &descriptor);
    value = (s16)gGameState->baseStats[2] + (s16)gGameState->statBonuses[2] + statValues[5];
    {
        u8 zero = 0;
        *override = zero;
        *entry = item;
        return Player_CalculateActionStat(zero, modifiers, value);
    }
}
