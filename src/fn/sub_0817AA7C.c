#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
struct SceneVolume {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[0x3CA];
    s16 activityTimer;
    u8 pad46C[0x84];
    u16 activity;
};
s32 SceneVolume_GetActivityState(struct SceneVolume *volume)
{
    s32 result;
    if (volume->activityTimer > 0) {
    active:
        return 1;
    }
    {
        u32 mask = 1;
        if (mask & volume->flags) {
            if (volume->activity == 0)
                goto active;
            result = 2;
        } else {
            result = 0;
        }
    }
    return result;
}
