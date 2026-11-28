#include "global.h"
#include "player/player.h"
#include "scene/areas/area_effect_link.h"

u8 sub_0823A518(struct Player *p)
{
    s32 slot = p->slot;

    if (gLinkSession == NULL || slot < 0)
        return 0;
    return gLinkSession->playerValues[slot];
}
