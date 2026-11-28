#include "global.h"
#include "player/player.h"

u32 LinkPlayer_GetAbilityCostById(s32);

u32 LinkPlayer_GetSelectedAbilityCost(struct Player *p)
{
    if ((s8)p->thresholdMode < 0)
        return 0;
    return LinkPlayer_GetAbilityCostById((s8)p->thresholdMode);
}
