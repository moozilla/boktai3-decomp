#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
#define SceneVolume_ConsumePendingEvent sub_0817B058
struct SceneVolume {
    u8 pad0[0x4E2];
    u8 pendingEvent;
};
s32 sub_0817B058(struct SceneVolume *volume)
{
    s32 result;
    if (volume->pendingEvent != 1) {
        result = 0;
    } else {
        volume->pendingEvent = 0;
        result = 1;
    }
    return result;
}
