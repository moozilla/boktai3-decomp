#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
struct SceneVolume {
    u8 pad0[0x4E2];
    u8 pendingEvent;
};
s32 SceneVolume_ConsumePendingEvent(struct SceneVolume *volume)
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
