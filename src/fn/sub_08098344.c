#include "global.h"
#include "contact_actor.h"
#include "scene_actor_reset.h"

// confidence=low: fills route->points[0..8] with a curve from the actor to a target point.
struct Route08098344 {
    u8 pad[0x568];
    struct ContactPosition points[9];
    u8 pad5B0[0x5FC - 0x5B0];
    struct ContactPosition motion;
};

static inline s32 Shift08098344(s32 n)
{
    if (n >= 0)
        return n >> 10;
    return -((-n) >> 10);
}

static inline s32 Blend08098344(s32 p0, s32 p1, s32 m0, s32 m1, s32 w00, s32 w01, s32 w10, s32 w11)
{
    return Shift08098344(m0 * (p0 * w00) + w01 * p1 + m1 * w10 + w11);
}

#ifdef NONMATCHING
// fuzzy: 18.9%
// scope shape 215 (differences once low registers are set aside)
// confidence=low: fills route->points[0..8] with a curve from the actor to a target point.
void sub_08098344(void *arg0, void *arg1)
{
    struct SceneActorRegistryResetView *actor = arg0;
    const struct ContactPosition *to = arg1;
    struct Route08098344 *route = actor->context;
    struct ContactPosition from;
    struct ContactPosition target;
    struct ContactPosition delta;
    s32 step;
    s32 mx;
    s32 mz;
    s32 i;

    from = actor->position;
    from.y = 0;
    target = *to;
    target.y = 0;
    step = Div(0x400, 8);
    mx = route->motion.x << 7;
    mz = route->motion.z << 7;
    delta.x = target.x - from.x;
    delta.y = target.y - from.y;
    delta.z = target.z - from.z;
    for (i = 0; i < 8; i++) {
        struct ContactPosition acc;
        s32 t = i + 1;
        s32 tt = t * step;
        s32 a = Shift08098344(tt * tt);
        s32 b = Shift08098344(a * tt);
        s32 h00 = 2 * b - 3 * a + 1;
        s32 h01 = 3 * a - 2 * b;
        s32 h10 = b - 2 * a + tt;
        s32 h11 = b - a;
        ZeroFill8(&acc);
        route->points[i].x = from.x + Blend08098344(acc.x, delta.x, mx, mz, h00, h01, h10, h11);
        route->points[i].y = Blend08098344(acc.y, delta.y, mx, mz, h00, h01, h10, h11);
        route->points[i].z = from.z + Blend08098344(acc.z, delta.z, mx, mz, h00, h01, h10, h11);
    }
    route->points[8] = target;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08098344);
#endif
