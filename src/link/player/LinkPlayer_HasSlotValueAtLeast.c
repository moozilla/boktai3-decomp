#include "global.h"
#include "player/player.h"
#include "scene/areas/area_effect_link.h"

static inline u8 LinkPlayer_GetSlotValue(struct Player *p)
{
    s32 slot = p->slot;

    if (gLinkSession == NULL || slot < 0)
        return 0;
    return gLinkSession->playerValues[slot];
}

u32 LinkPlayer_HasSlotValueAtLeast(struct Player *p, s32 minimum)
{
    if (LinkPlayer_GetSlotValue(p) < minimum)
        return 0;
    return 1;
}
