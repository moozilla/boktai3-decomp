#define Player_EvaluateItemStatA    sub_08159030
#define Player_ComputeItemStatA     sub_0814A530
#define Player_ComputeIndexedStatA Player_GetSpecialAttackPower
#define Player_BuildActionModifiers sub_0813E608
#define gGameState                  gUnk_02000710
#include "global.h"
#include "player.h"
#include "game_state.h"
struct PlayerEquipmentReference {
    u8 index;
    u8 unknown1[11];
    u8 slot;
};
void sub_0813EDDC(u32, void *, s32);
s32 sub_0814A530(s32, struct PlayerStatModifierContext *, s32);
s32 Player_GetSpecialAttackPower(s32, s32);
void sub_0822D764(const void *, void *);
void sub_0813E608(struct Player *, struct PlayerStatModifierContext *, const void *);
s32 sub_08159030(struct PlayerActionEntry *item, struct PlayerEquipmentReference *reference)
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
    src = gUnk_02000710->party;
    dst = equipment;
    for (i = 3; i >= 0; i--)
        *dst++ = *src++;
    if (reference)
        equipment[reference->slot] = index;
    sub_0813EDDC(0, statValues, index);
    sub_0822D764(item, &descriptor);
    sub_0813E608(0, modifiers, &descriptor);
    value = (s16)gUnk_02000710->baseStats[2] + (s16)gUnk_02000710->statBonuses[2] + statValues[5];
    {
        u8 zero = 0;
        *override = zero;
        *entry = item;
        return sub_0814A530(zero, modifiers, value);
    }
}
