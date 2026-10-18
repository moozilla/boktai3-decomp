#define Camera_CalculateDistanceScale sub_0822776C
#include "global.h"
struct CameraPosition {
    s16 x, y, z;
};
struct CameraDistanceScale {
    struct CameraPosition *first, *second;
    u8 unknown08[8];
    s32 minimum, maximum, divisor;
};
u16 Sqrt(u32);
s32 Div(s32, s32);
s32 sub_0822776C(struct CameraDistanceScale *scale)
{
    struct CameraPosition *first = scale->first;
    struct CameraPosition *second = scale->second;
    s32 x = second->x - first->x;
    s32 z = second->z - first->z;
    s32 value = Div(Sqrt(x * x + z * z), scale->divisor);
    if (value < scale->minimum)
        value = scale->minimum;
    if (value > scale->maximum)
        value = scale->maximum;
    return value;
}
