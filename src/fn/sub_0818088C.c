#define ProximityVolume_SetParameter sub_0818088C
#include "global.h"
struct ProximityVolume {
    u8 unknown0[0x3A];
    u16 parameter;
};
void sub_0818088C(struct ProximityVolume *volume, u32 parameter)
{
    if (volume)
        volume->parameter = parameter;
}
