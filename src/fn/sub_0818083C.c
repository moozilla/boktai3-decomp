#include "global.h"
struct ProximityVolume {
    void *update;
};
s32 Callback_InvokeActor(void *, void *);
u32 ProximityVolume_DispatchUpdate(struct ProximityVolume *volume)
{
    if (volume->update)
        Callback_InvokeActor(volume, volume->update);
    return 0;
}
