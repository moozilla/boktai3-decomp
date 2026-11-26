#include "global.h"
struct O {
    u8 f[8];
    u32 fl;
};
void ResultGame_ShowTargetMarker(u8 *p)
{
    ((struct O *)(p + 0x264))->fl &= ~1;
}
