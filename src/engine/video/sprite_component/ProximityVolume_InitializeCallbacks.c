#include "global.h"
#include "engine/video/proximity_volume.h"

u32 SceneMotion_InitState(struct ProximityVolume *);
void ProximityVolume_Update(struct ProximityVolume *);
void ProximityVolume_IdleCallback(struct ProximityVolume *);
static inline void SetCallbacks(struct ProximityVolume *volume,
                                void (*update)(struct ProximityVolume *),
                                void (*destroy)(struct ProximityVolume *))
{
    volume->update = update;
    volume->destroy = destroy;
}

u32 ProximityVolume_InitializeCallbacks(struct ProximityVolume *volume)
{
    u32 result;
    volume->update = NULL;
    volume->destroy = NULL;
    result = SceneMotion_InitState(volume);
    SetCallbacks(volume, ProximityVolume_Update, ProximityVolume_IdleCallback);
    return result;
}
