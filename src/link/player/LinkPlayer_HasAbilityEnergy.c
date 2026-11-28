#include "global.h"
#include "player/player.h"

s32 LinkPlayer_GetSelectedAbilityCost(struct Player *);

u32 LinkPlayer_HasAbilityEnergy(struct Player *p)
{
    s32 cost = LinkPlayer_GetSelectedAbilityCost(p);

    if (p->currentStatB >= cost)
        return 1;
    return 0;
}
