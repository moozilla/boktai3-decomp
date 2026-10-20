#include "global.h"
#include "player.h"
u32 LinkPlayer_HasAbilityEnergy(struct Player *);
#define Player_GetActionIndex sub_0823AB5C
s32 Player_GetActionIndex(struct Player *p)
{
    if (p->actionSelectionStamp != 0 && LinkPlayer_HasAbilityEnergy(p)
        && (u8)(p->statModifierContext.statBOverrideEnabled - 3) > 1)
        return (s8)p->thresholdMode;
    return -1;
}
