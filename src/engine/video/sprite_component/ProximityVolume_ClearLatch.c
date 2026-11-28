#include "global.h"
#include "engine/video/proximity_volume.h"

u32 ProximityVolume_ClearLatch(struct ProximityVolume *volume)
{
    volume->active = 0;
}
