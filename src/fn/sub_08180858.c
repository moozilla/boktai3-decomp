#define ProximityVolume_ClearLatch sub_08180858
#include "global.h"
struct ProximityVolume {
    u8 unknown0[0x38];
    u16 latch;
};
u32 sub_08180858(struct ProximityVolume *volume)
{
    volume->latch = 0;
}
