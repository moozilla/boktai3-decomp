#include "global.h"

extern u32 gUnk_020004BC;
void ActorMarkerPool_RemoveMarker(s32);

s32 ActorMarkerPool_Destroy(void)
{
    s32 i;
    for (i = 0; i <= 7; i++)
        ActorMarkerPool_RemoveMarker(i);
    return gUnk_020004BC = 0;
}
