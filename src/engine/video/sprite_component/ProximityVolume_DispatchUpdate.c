#include "global.h"
#include "engine/video/sprite_component.h"
s32 Callback_InvokeActor(void *, void *);
u32 ProximityVolume_DispatchUpdate(struct SpriteComponent *volume)
{
    if (volume->update)
        Callback_InvokeActor(volume, volume->update);
    return 0;
}
