#include "global.h"
#include "render.h"
#include "ram.h"
struct Camera {
    u8 pad[0x7e];
    u16 shake;
    u16 offsetX;
    u16 offsetZ;
};
struct Vec {
    s16 x, y, z;
};
static inline s32 scale(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}
static inline s32 scaleHeight(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void project(struct Vec *out, struct CameraPosition *in)
{
    s32 x;
    s32 z, h, y;
    x = in->x;
    z = in->z;
    out->x = scale(x - z);
    {
        s32 sum = x + z;
        h = scale(sum);
    }
    y = scaleHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
void sub_082260D8(void)
{
    if (((struct Camera *)gActiveCamera)) {
        gCamera.position.x -= ((struct Camera *)gActiveCamera)->offsetX;
        gCamera.position.z -= ((struct Camera *)gActiveCamera)->offsetZ;
        project(&gCameraOrigin, &gCamera.position);
        ((struct Camera *)gActiveCamera)->shake = 0;
        ((struct Camera *)gActiveCamera)->offsetX = 0;
        ((struct Camera *)gActiveCamera)->offsetZ = 0;
    }
}
