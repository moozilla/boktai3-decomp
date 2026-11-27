#include "engine/video/proximity_volume.h"

void *SabataFrost_Create(void);
void *SceneMotion_InitState(struct ProximityVolume *motion)
{
    void *task;
    ZeroFill8(&motion->position);
    task = SabataFrost_Create();
    motion->emitter = task;
    motion->active = 0;
#ifdef UBFIX
    return task;
#else
    /* The ROM retains the value from SabataFrost_Create in r0. */
#endif
}
