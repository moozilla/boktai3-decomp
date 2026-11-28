#include "global.h"
#include "player/player.h"

struct LinkAbilityState {
    s8 id;
    u8 enabled, stamp, cost;
};

s32 LinkPlayer_GetAbilityCostById(s32);
s32 LinkPlayer_IsAbilityIdValid(struct Player *, s32);

void LinkPlayer_InitAbilityState(struct Player *p)
{
    struct LinkAbilityState *ability = (struct LinkAbilityState *)&p->thresholdMode;

    ability->id = ((const u8 *)p->actionConfig)[0x3E];
    if (ability->id < 0) {
        ability->cost = 0;
        ability->enabled = 0;
        ability->stamp = 0;
    } else {
        ability->cost = LinkPlayer_GetAbilityCostById(ability->id);
        ability->enabled = LinkPlayer_IsAbilityIdValid(p, ability->id);
        ability->stamp = 0;
    }
}
