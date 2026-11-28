#include "global.h"
#include "engine/video/proximity_volume.h"

void ProximityVolume_SetParameter(struct ProximityVolume *volume, u32 damage)
{
    if (volume)
        volume->damage = damage;
}
