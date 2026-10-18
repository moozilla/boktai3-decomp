#define Player_CalculateAlternateActionStat Player_GetSpecialAttackPower
#include "global.h"
#include "player.h"
void Inventory_GetEquipmentDescriptor(s32, void *);
s32 Player_CalculateActionStat(struct Player *, struct PlayerStatModifierContext *, s32);
s32 Player_GetSpecialAttackPower(struct Player *, s32);
s32 Player_CalculatePreviewActionStat(struct Player *p, s32 bonus,
                                      struct PlayerActionConfig *config,
                                      struct PlayerActionEntry *entry, s32 mode)
{
    u8 buffer[32];
    u8 *q = (u8 *)p + 0x32C;
    s32 sum = *(s16 *)(q + 10) + p->derivedStats[2] + bonus;
    switch (mode) {
        case 0:
        case 1:
        case 2: {
            u16 savedDuration;
            u8 savedMode;
            struct PlayerActionEntry *savedEntry;
            s32 result;
            Inventory_GetEquipmentDescriptor(config->kind, buffer);
            savedDuration = p->statModifierContext.actionDuration;
            savedMode = p->statModifierContext.statBOverrideEnabled;
            savedEntry = p->statModifierContext.actionEntry;
            p->statModifierContext.actionDuration = config->duration;
            p->statModifierContext.statBOverrideEnabled = mode;
            p->statModifierContext.actionEntry = entry;
            result = Player_CalculateActionStat(p, &p->statModifierContext, sum);
            p->statModifierContext.actionDuration = savedDuration;
            p->statModifierContext.statBOverrideEnabled = savedMode;
            p->statModifierContext.actionEntry = savedEntry;
            return result;
        }
        case 4:
            return Player_GetSpecialAttackPower(p, config->kind);
        default:
            return 1;
    }
}
