#ifndef GUARD_ORBIT_MOTION_H
#define GUARD_ORBIT_MOTION_H

#include "engine.h"

struct OrbitVector {
    s16 x, y, z;
};
struct OrbitWavePair {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};

/* Steering uses base as its target, offset as velocity, speed as the turn
 * limit, and radius as movement speed. Orbit modes reuse those fields. */
enum OrbitMotionMode {
    ORBIT_MOTION_OFFSET = 0,
    ORBIT_MOTION_HALF = 1,
    ORBIT_MOTION_QUARTER = 2,
    ORBIT_MOTION_EIGHTH = 3,
    ORBIT_MOTION_SIXTEENTH = 4,
    ORBIT_MOTION_STEP = 5,
    ORBIT_MOTION_STEER_XZ = 6,
    ORBIT_MOTION_STEER_XY = 7,
    ORBIT_MOTION_ARC = 8,
    ORBIT_MOTION_CROSSED_SINES = 9,
    ORBIT_MOTION_WAVE_APPROACH = 10,
    ORBIT_MOTION_INTERPOLATE = 12,
};

struct OrbitMotion {
    struct OrbitVector *position;
    struct OrbitVector start;
    struct OrbitVector base, offset;
    struct OrbitVector center;
    s16 startAngle, endAngle;
    u8 mode, state, angle, speed;
    s16 radius;
    s16 radius2;
    s16 radius3;
    u16 duration;
    s32 timer;
};
static inline s32 scale8(s32 n)
{
    if (n >= 0)
        return n >> 8;
    return -((-n) >> 8);
}
static inline s32 scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline s32 interpolate(s32 end, s32 duration, s32 start, s32 time)
{
    if (time <= 0)
        return start;
    if (time >= duration)
        return end;
    return start + scale8(((end - start) << 8) * time / duration);
}
static inline void polar(struct OrbitVector *v, s32 angle, s32 radius)
{
    v->x = scale12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale12(gUnk_086149C4[angle] * radius);
}
static inline void add(struct OrbitVector *out, struct OrbitVector *a, struct OrbitVector *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

s32 SceneScriptObject_Initialize(void *, u32);
void SceneScriptTask_Update(void *);
void DungeonProp_Destroy(void *);
u16 Sqrt(u32);
s32 Div(s32, s32);

#endif
