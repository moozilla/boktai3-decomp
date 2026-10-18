#define ProximityVolume_DispatchUpdate sub_0818083C
#include "global.h"
struct ProximityVolume {
    void *update;
};
s32 Callback_InvokeActor(void *, void *);
u32 sub_0818083C(struct ProximityVolume *volume)
{
    if (volume->update)
        Callback_InvokeActor(volume, volume->update);
    return 0;
}
