#include "orbit_motion.h"

void *DungeonProp_CreateFromScript(u32 scriptCaller)
{
    void *task = Task_Create(8, 0x140);
    if (task) {
        Task_SetCallbacks(task, SceneScriptTask_Update, DungeonProp_Destroy);
        if (sub_081B866C(task, scriptCaller) < 0) {
            Task_Kill(task);
            return 0;
        }
    }
    return task;
}

s32 SceneMover_SteerXZ(struct OrbitMotion *motion)
{
    s32 result = 0;
    struct OrbitVector *destination = motion->position;
    struct OrbitVector *position = &motion->base;
    s32 angle;
    s32 x;
    s32 z;
    s32 distanceSquared;
    s32 nextAngle;
    s32 delta;
    s32 oldAngle;
    {
        s32 deltaX = motion->base.x - destination->x;
        s32 deltaOther = position->z - destination->z;
        angle = Math_Atan2(deltaX, deltaOther);
    }
    {
        struct OrbitVector *currentDestination = motion->position;
        x = currentDestination->x - motion->base.x;
        z = currentDestination->z - position->z;
    }
    distanceSquared = x * x + z * z;
    if (distanceSquared <= motion->radius * motion->radius) {
        struct OrbitVector *velocity;
        u16 shortDistance = Sqrt(distanceSquared);
        u32 distance;
        velocity = &motion->offset;
        distance = shortDistance;
        velocity->x = scale12(gUnk_086149C4[(angle + 64) & 255] * distance);
        velocity->y = 0;
        velocity->z = scale12(gUnk_086149C4[angle & 255] * distance);
        motion->state = 2;
        result = 1;
    }
    {
        struct OrbitVector *velocity, *destination, *savedVelocity;
        oldAngle = motion->angle;
        delta = (angle - oldAngle) & 255;
        if (delta > 128 && delta < 256 - motion->speed)
            nextAngle = oldAngle - motion->speed;
        else if (delta <= 128 && delta > motion->speed)
            nextAngle = motion->angle + motion->speed;
        else
            nextAngle = angle;
        motion->angle = nextAngle;
        velocity = &motion->offset;
        {
            s32 speed = motion->radius;
            s32 scaledSpeed = gUnk_086149C4[(nextAngle + 64) & 255] * speed;
            savedVelocity = velocity;
            velocity->x = scale12(scaledSpeed);
            velocity->y = 0;
            velocity->z = scale12(gUnk_086149C4[nextAngle & 255] * speed);
        }
        destination = motion->position;
        destination->x += motion->offset.x;
        destination->y += savedVelocity->y;
        destination->z += savedVelocity->z;
    }
    return result;
}

s32 SceneMover_SteerXY(struct OrbitMotion *motion)
{
    s32 result = 0;
    struct OrbitVector *destination = motion->position;
    struct OrbitVector *position = &motion->base;
    s32 angle;
    s32 x;
    s32 z;
    s32 distanceSquared;
    s32 nextAngle;
    s32 delta;
    s32 oldAngle;
    {
        s32 deltaX = motion->base.x - destination->x;
        s32 deltaOther = position->y - destination->y;
        angle = Math_Atan2(deltaX, deltaOther);
    }
    {
        struct OrbitVector *currentDestination = motion->position;
        x = motion->base.x - currentDestination->x;
        z = position->y - currentDestination->y;
    }
    distanceSquared = x * x + z * z;
    if (distanceSquared <= motion->radius * motion->radius) {
        struct OrbitVector *velocity;
        u16 shortDistance = Sqrt(distanceSquared);
        u32 distance;
        velocity = &motion->offset;
        distance = shortDistance;
        velocity->x = scale12(gUnk_086149C4[(angle + 64) & 255] * distance);
        velocity->y = scale12(gUnk_086149C4[angle & 255] * distance);
        velocity->z = 0;
        motion->state = 2;
        result = 1;
    }
    {
        struct OrbitVector *velocity, *destination, *savedVelocity;
        oldAngle = motion->angle;
        delta = (angle - oldAngle) & 255;
        if (delta > 128 && delta < 256 - motion->speed)
            nextAngle = oldAngle - motion->speed;
        else if (delta <= 128 && delta > motion->speed)
            nextAngle = motion->angle + motion->speed;
        else
            nextAngle = angle;
        motion->angle = nextAngle;
        velocity = &motion->offset;
        {
            s32 speed = motion->radius;
            s32 scaledSpeed = gUnk_086149C4[(nextAngle + 64) & 255] * speed;
            savedVelocity = velocity;
            velocity->x = scale12(scaledSpeed);
            velocity->y = scale12(gUnk_086149C4[nextAngle & 255] * speed);
            velocity->z = 0;
        }
        destination = motion->position;
        destination->x += motion->offset.x;
        destination->y += savedVelocity->y;
        destination->z += savedVelocity->z;
    }
    return result;
}

s32 OrbitMotion_UpdateAngle(struct OrbitMotion *motion)
{
    struct OrbitVector delta;
    motion->angle = interpolate(motion->endAngle, motion->duration, motion->startAngle, motion->timer);
    polar(&delta, motion->angle, motion->radius);
    add(motion->position, &motion->center, &delta);
    if ((u32)++motion->timer > motion->duration) {
        motion->state = 2;
        motion->timer = 0;
        return 1;
    }
    return 0;
}

static inline s32 absolute(s32 n)
{
    if (n < 0)
        n = -n;
    return n;
}

s32 OrbitMotion_UpdateFoldedOrbit(struct OrbitMotion *motion)
{
    s32 angle = (256 - absolute(256 - (motion->angle + motion->speed * motion->timer))) & 255;
    polar(&motion->offset, angle, motion->radius);
    add(motion->position, &motion->offset, motion->position);
    if ((u32)(++motion->timer * motion->speed) > 512) {
        motion->state = 2;
        motion->timer = 0;
        return 1;
    }
    return 0;
}

static inline s32 OrbitMotion_HalfTowardZero(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}



static inline void OrbitMotion_ProjectCrossedPair(struct OrbitVector *v, struct OrbitWavePair *motion)
{
    s32 screenY;
    s32 screenX;
    screenX = motion->x;
    screenY = motion->y;
    {
        s32 projectedX = OrbitMotion_HalfTowardZero(((screenX + screenY) << 5) / 3);
        s16 *destinationX = &v->x;
        *destinationX = projectedX;
    }
    v->y = 0;
    v->z = OrbitMotion_HalfTowardZero(((screenY - (s16)screenX) << 5) / 3);
}

s32 OrbitMotion_UpdateCrossedSines(struct OrbitMotion *motion)
{
    struct OrbitWavePair pair;
    pair.x = scale12(motion->radius * gUnk_086149C4[motion->timer & 255]);
    pair.y = scale12(motion->radius2 * gUnk_086149C4[(motion->timer * 2) & 255]);
    OrbitMotion_ProjectCrossedPair(&motion->offset, &pair);
    add(motion->position, &motion->offset, &motion->base);
    if (((u32)(++motion->timer)) > 512) {
        motion->timer = 0;
        return 1;
    }
    return 0;
}

void OrbitMotion_UpdateAbsoluteSines(struct OrbitMotion *motion)
{
    motion->offset.x = absolute(scale12(motion->radius * gUnk_086149C4[(motion->angle + motion->speed * motion->timer) & 255]));
    motion->offset.y =
        absolute(scale12(motion->radius2 * gUnk_086149C4[(motion->angle + motion->speed * motion->timer) & 255]));
    motion->offset.z =
        absolute(scale12(motion->radius3 * gUnk_086149C4[(motion->angle + motion->speed * motion->timer) & 255]));
}

s32 OrbitMotion_InterpolatePosition(struct OrbitMotion *motion)
{
    struct OrbitVector *out = motion->position;
    struct OrbitVector *start = &motion->start;
    struct OrbitVector *end = &motion->base;
    if ((u32)motion->timer >= motion->duration) {
        *out = *end;
        motion->state = 2;
        return 1;
    }
    out->x = interpolate(end->x, motion->duration, start->x, motion->timer);
    out->y = interpolate(end->y, motion->duration, start->y, motion->timer);
    out->z = interpolate(end->z, motion->duration, start->z, motion->timer);
    ++motion->timer;
    return 0;
}

static inline void difference(struct OrbitVector *out, struct OrbitVector *a, struct OrbitVector *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline s32 square(s32 n)
{
    return n * n;
}

static inline void normalize(struct OrbitVector *v, s32 scale)
{
    s32 sum = square(v->x) + square(v->y);
    s32 length = Sqrt(sum + square(v->z));
    if (length) {
        v->x = Div(v->x * scale, length);
        v->y = Div(v->y * scale, length);
        v->z = Div(v->z * scale, length);
    }
}

s32 OrbitMotion_CalculateArcCenter(struct OrbitVector *out, struct OrbitVector *start,
                                   struct OrbitVector *end, s32 sweep)
{
    struct OrbitVector delta;
    s32 distanceSquared;
    s32 direction;
    s32 angle;
    s32 radius;
    if (!out || !start || !end)
        return 0;
    if (sweep < 0) {
        direction = -1;
        sweep = -sweep;
    } else {
        direction = 1;
    }
    sweep %= 256;
    difference(&delta, end, start);
    distanceSquared = square(delta.x) + square(delta.z);
    if (sweep == 0) {
        *out = *start;
        return 0;
    }
    if (sweep == 128) {
        add(out, start, end);
        out->x /= 2;
        out->z /= 2;
        out->y /= 2;
        return Sqrt(distanceSquared) / 2;
    }
    if (sweep < 128)
        angle = (128 - sweep) >> 1;
    else
        angle = (sweep - 128) >> 1;
    if (sweep > 128)
        direction = -direction;
    if (direction < 0)
        angle = (256 - angle) & 255;
    out->x =
        scale12(gUnk_086149C4[(angle + 64) & 255] * delta.x - gUnk_086149C4[angle & 255] * delta.z);
    out->z =
        scale12(gUnk_086149C4[angle & 255] * delta.x + gUnk_086149C4[(angle + 64) & 255] * delta.z);
    out->y = 0;
    radius = Sqrt(distanceSquared) / 2;
    radius = (radius << 12) / gUnk_086149C4[(angle + 64) & 255];
    normalize(out, radius);
    add(out, start, out);
    return radius;
}
