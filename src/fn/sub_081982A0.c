#include "global.h"
#include "game_state.h"

// Register allocation: the original keeps y in r4 and the output pointer in r5; here they swap.
struct V {
    s16 x, y, z;
};
struct P {
    u32 x : 16, y : 16, z : 16, w : 16;
};
extern struct V gCameraOrigin;
void ShadowActor_UpdateTransition(void *);
s32 __divsi3(s32, s32);
static inline s32 sh(s32 v, s32 n)
{
    s32 r;
    if (v >= 0)
        r = v >> n;
    else
        r = -((-v) >> n);
    return r;
}
static inline void project(struct V *out, struct V *in)
{
    s32 a, b, c, d, x = in->x, z = in->z;
    out->x = sh(3 * (x - z), 5);
    a = sh(3 * (x + z), 5);
    b = sh(3 * in->y, 5);
    c = a - b;
    d = a + b;
    out->x = out->x - gCameraOrigin.x + 120;
    out->y = c - gCameraOrigin.y + 80;
    out->z = d - gCameraOrigin.z;
}
struct Q {
    u8 pad[0x48];
    s16 x, y, z;
};
void sub_081982A0(u8 *p)
{
    struct P in;
    struct V out;
    s32 new_var;
    u8 *actor, *a2;
    s32 y, a, b, c;
    struct V *world;
    ShadowActor_UpdateTransition(p);
    actor = gGameState;
    in.x = *(u16 *)(actor + 0x30);
    in.y = *(u16 *)(actor + 0x32);
    in.z = *(u16 *)(actor + 0x34);
    project(&out, (struct V *)&in);
    {
        if (*(u32 *)(p + 0x30) & 4)
            out.x = 0xe0;
        else
            out.x = 0x10;
    }
    a2 = gGameState;
    world = (struct V *)&in;
    y = *(s16 *)(a2 + 0x32);
    a = ({
            struct V *q = &out;
            q->x;
        }) +
        gCameraOrigin.x - 0x78;
    b = out.y + gCameraOrigin.y - 0x50;
    c = __divsi3((a + b) << 5, 3) + y;
    world->x = sh(c, 1);
    new_var = y;
    world->y = new_var;
    c = __divsi3((b - a) << 5, 3) + new_var;
    world->z = sh(c, 1);
    ((struct Q *)p)->x = in.x;
    ((struct Q *)p)->y = in.y;
    ((struct Q *)p)->z = in.z;
}
