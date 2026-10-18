#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
struct SceneVolume {
    u8 pad0[0xAE];
    u8 initialMode;
    u8 padAF[0x43A];
    u8 mode;
    u8 pad4EA;
    u8 phase;
};
void SceneVolume_BeginPhase(struct SceneVolume *volume, u32 phase)
{
    volume->mode = volume->initialMode;
    volume->phase = phase;
}
