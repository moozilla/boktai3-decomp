#include "global.h"
#include "ram.h"

void ActorMarkerPool_RemoveMarker(s32);

s32 ActorMarkerPool_Destroy(void)
{
    s32 i;
    for (i = 0; i <= 7; i++)
        ActorMarkerPool_RemoveMarker(i);
    return gPanelSpritePool = 0;
}
