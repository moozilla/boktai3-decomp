#include "global.h"
#include "player/player.h"

s32 Player_CalculateActionStat(struct Player *, struct PlayerStatModifierContext *, s32);
s32 Player_GetSpecialAttackPower(struct Player *, s32, s32);

s32 Player_CalculateBaseAttackPower(struct Player *p, s32 bonus)
{
    struct EquipmentFlagState *equipment = (struct EquipmentFlagState *)p->actionFlagState;
    s32 sum = (s16)equipment->bonus[2] + p->derivedStats[2] + bonus;

    switch (p->statModifierContext.statBOverrideEnabled) {
        case 0:
        case 1:
        case 2:
            return Player_CalculateActionStat(p, &p->statModifierContext, sum);
        case 4:
            return Player_GetSpecialAttackPower(p, p->statModifierContext.actionKind, sum);
        default:
            return 1;
    }
}
