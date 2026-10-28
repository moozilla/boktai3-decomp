#include "global.h"
#include "render.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u8 pad[8];
    u32 flags;
    u8 padc[20];
    struct Vec position;
    u8 pad28[12];
    u8 scaleX, scaleY;
    u16 angle;
};
struct Effect {
    u32 pad0;
    u8 state, entered, pad6, mode;
    u32 pad8, timer;
    u16 duration;
    u8 index, pad13[5];
    struct Vec position;
    u8 pad20[4];
    struct Sprite sprite;
};
static inline int project(int input)
{
    int v = input * 3;
    if (v >= 0)
        v = v >> 5;
    else
        v = -((-v) >> 5);
    return v;
}
void FloatingEffect_Project(struct Effect *p)
{
    struct Sprite *s = &p->sprite;
    struct Vec *out = &p->sprite.position;
    struct Vec *in = &p->position;
    int xy;
    int height, x = in->x, z = in->z;
    out->x = project(x - z);
    xy = project(x + z);
    height = project(in->y);
    {
        int y = xy - height, z = xy + height;
        out->x = out->x - (*(struct Vec *)gCameraOrigin).x + 120;
        out->y = y - (*(struct Vec *)gCameraOrigin).y + 80;
        out->z = z - (*(struct Vec *)gCameraOrigin).z;
    }
    s->position.x -= 8;
    s->position.y -= 8;
}
