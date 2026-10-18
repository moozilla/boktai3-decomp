#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
#define SceneVolume_ResetPendingSequence sub_0817B078
struct SceneVolume {
    u8 pad0[0x314];
    void *sequence;
    u8 pad318[0x1EC];
    u8 sequenceState;
    u8 pad505;
    u8 pendingReset;
    u8 pad507;
    u32 sequenceLength;
    u8 pad50C[0xF4];
    u8 sequenceBuffer[1];
};
void sub_0817B078(struct SceneVolume *volume)
{
    if (volume->pendingReset) {
        volume->sequenceState = 0;
        volume->pendingReset = 0;
        volume->sequenceLength = 0x34;
        volume->sequence = volume->sequenceBuffer;
    }
}
