#include "global.h"
#include "player/player.h"

u32 LinkPlayer_HasAbilityEnergy(struct Player *);

s32 Player_GetActionIndex(struct Player *p)
{
    if (p->actionSelectionStamp != 0 && LinkPlayer_HasAbilityEnergy(p) &&
        (p->statModifierContext.statBOverrideEnabled < 3 ||
         p->statModifierContext.statBOverrideEnabled > 4))
        return (s8)p->thresholdMode;
    return -1;
}
