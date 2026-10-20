#include "global.h"
#include "scene_vec.h"
#define SpriteController_UpdateInterpolatedPosition sub_081C3C1C
// The F3E4 move/scale setup at 081C371C installs this callback.
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct SpriteController {
    u8 pad0[0x3C];
    u8 scaleX, scaleY;
    u8 pad3E[0x12];
    s16 screenX, screenY, screenZ;
    u8 pad56[0x3E];
    u16 fromScale, toScale;
    u8 pad98[2];
    u16 duration;
    u8 pad9C[0x3C];
    u32 done;
    s16 fromX, fromY, fromZ, fromPad, toX, toY, toZ, toPad;
    u8 padEC[0xC];
    s32 timer;
};
extern u16 gUnk_03005418[];
static inline s32 shift8(s32 value)
{
    if (value >= 0)
        return value >> 8;
    return -(-value >> 8);
}
static inline s32 interpolate(s32 from, s32 to, s32 timer, s32 duration)
{
    if (timer <= 0)
        return from;
    if (timer >= duration)
        return to;
    return from + shift8(((to - from) << 8) * timer / duration);
}
static inline s32 scale(s32 value)
{
    s32 result = value;
    if (result >= 0)
        result >>= 5;
    else
        result = -(-result >> 5);
    return result;
}
static inline void project(s16 *out, struct PackedVec *v)
{
    struct SceneVec *pos = (struct SceneVec *)v;
    s32 x = v->x;
    s32 worldZ = pos->z;
    s32 z, y;
    out[0] = scale(3 * (x - worldZ));
    {
        s32 sum = x + worldZ;
        z = scale(3 * sum);
    }
    {
        struct SceneVec *yp = pos;
        y = scale(3 * yp->y);
    }
    out[1] = z - y;
    z += y;
    out[0] = out[0] - gUnk_03005418[0] + 120;
    out[1] = out[1] - gUnk_03005418[1] + 80;
    z -= gUnk_03005418[2];
    out[2] = z;
}
void SpriteController_UpdateInterpolatedPosition(struct SpriteController *p)
{
    struct PackedVec v;
    s32 result = interpolate(p->fromScale, p->toScale, p->timer, p->duration);
    p->scaleX = result;
    p->scaleY = result;
    v.x = interpolate(p->fromX, p->toX, p->timer, p->duration);
    v.y = interpolate(p->fromY, p->toY, p->timer, p->duration);
    v.z = interpolate(p->fromZ, p->toZ, p->timer, p->duration);
    project(&p->screenX, &v);
    {
        s32 t = ++p->timer;
        s32 duration = p->duration;
        if (t > duration)
            p->done = 1;
    }
}
