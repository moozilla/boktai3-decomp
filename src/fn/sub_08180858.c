#include "global.h"
struct ProximityVolume {
    u8 unknown0[0x38];
    u16 latch;
};
u32 ProximityVolume_ClearLatch(struct ProximityVolume *volume)
{
    volume->latch = 0;
}
