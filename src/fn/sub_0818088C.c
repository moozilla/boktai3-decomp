#include "global.h"
struct ProximityVolume {
    u8 unknown0[0x3A];
    u16 parameter;
};
void ProximityVolume_SetParameter(struct ProximityVolume *volume, u32 parameter)
{
    if (volume)
        volume->parameter = parameter;
}
