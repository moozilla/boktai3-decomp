#include "global.h"
#include "engine/video/proximity_volume.h"

void ProximityVolume_TranslateBounds(void *, struct ContactPosition *);
void SabataFrost_StartEmission(void *, struct ContactPosition *);
void ProximityVolume_SetPosition(struct ProximityVolume *volume, struct ContactPosition *position)
{
    struct ContactPosition *stored = &volume->position;

    *stored = *position;
    volume->active = 1;
    ProximityVolume_TranslateBounds(volume->probe, stored);
    SabataFrost_StartEmission(volume->emitter, stored);
}
