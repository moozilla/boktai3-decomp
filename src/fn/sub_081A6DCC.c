#include "global.h"

struct CameraOffsetVector {
    s16 x, y, z;
};
struct CameraPackedOffset {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
void Camera_SetOffset(s32, u16 *);
static inline int scaleOffset(int n)
{
    return n >= 0 ? n >> 8 : -((-n) >> 8);
}

void Camera_BlendOffset(struct CameraOffsetVector *a, struct CameraOffsetVector *b, s32 weight)
{
    struct CameraPackedOffset result;
    if (a && b) {
        if (weight < 0)
            weight = 0;
        if (weight > 256)
            weight = 256;
        result.x = scaleOffset(a->x * weight + b->x * (256 - weight));
        result.y = scaleOffset(a->y * weight + b->y * (256 - weight));
        result.z = scaleOffset(a->z * weight + b->z * (256 - weight));
        Camera_SetOffset(8, (u16 *)&result);
    }
}
