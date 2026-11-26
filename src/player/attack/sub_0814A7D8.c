#include "global.h"

#include "player/player.h"
#include "player/player_slot_status.h"
s32 Player_CalculateBaseAttackPower(struct Player *, s32);
s32 sub_08249240(void *, s32, void *);
s32 Div(s32, s32);

s32 Player_CalculateTotalAttackPower(struct Player *p, s32 adjustment)
{
    s32 i;
    s32 power = Player_CalculateBaseAttackPower(p, adjustment);
    for (i = 0; i < 3; i++) {
        if (p->statModifierContext.statModifierCallbacks[i] != 0)
            power += sub_08249240(&p->statModifierContext, power,
                                  p->statModifierContext.statModifierCallbacks[i]);
    }
    if (p != 0 && Player_ReadStatusBits(&p->statBonusFlags, 0x08000000))
        power += Div(p->currentStatA * 20, p->maximumStatA);
    return power;
}
