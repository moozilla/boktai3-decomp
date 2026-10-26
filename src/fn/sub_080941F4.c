#include "global.h"
#include "scene_actor_control_phase.h"
#include "engine.h"

extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void sub_0809345C(void *, void *);
void *Bee_SubVector(void *, void *, void *);

struct Sub080941F4Anchor {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
struct Sub080941F4Route {
    struct ContactPosition points[9];
    u8 index;
    u8 unknown429[0x3];
    u16 speed;
    u8 unknown42E[0xC];
    u16 timer;
    u16 unknown43C;
    u16 polarSpeed;
    struct Sub080941F4Anchor *anchor;
    u8 unknown444[0x3C];
    struct ContactPosition motion;
};

static inline s32 Sub080941F4Scale(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline s32 Sub080941F4Shift5(s32 n)
{
    if (n >= 0)
        return n >> 5;
    return -((-n) >> 5);
}
static inline void Sub080941F4Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Sub080941F4Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Sub080941F4Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline void Sub080941F4Delta(struct ContactPosition *out, const struct ContactPosition *from,
                                    const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u16 Sub080941F4RandomMotion(void)
{
    u32 offset;
    const u16 *table;

    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = gUnk_0203B400;
    return *(const u16 *)(offset + (u32)table);
}
static inline u32 Sub080941F4Facing(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}
#ifdef NONMATCHING
// fuzzy: 25.5%
// scope shape 205 (differences once low registers are set aside)
// confidence=low: steps along nine route points toward an anchor, then retargets after the route ends.
void sub_080941F4(struct SceneActorRegistryResetView *actor)
{
    struct Sub080941F4Route *route = actor->context;
    struct ContactPosition delta, target, step;
    struct ContactPosition *pos = &actor->position;
    struct ContactPosition *anchor;
    s32 n;

    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = *(void **)((u8 *)actor + 0x228);

        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = 1;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        route->index = 0;
        anchor = &route->anchor->position;
        Sub080941F4Delta(&delta, pos, anchor);
        actor->fieldE9 = Math_Atan2(delta.x, delta.z);
        Sub080941F4Polar(&target,
                         Sub080941F4Facing(Sub080941F4RandomMotion(), &actor->fieldE9) & 255,
                         route->polarSpeed);
        route->timer = 0x500;
        target.x += pos->x;
        target.y += pos->y;
        target.z += pos->z;
        sub_0809345C(actor, &target);
        Bee_SubVector(actor, pos, &target);
        ZeroFill8(&route->motion);
    }
    Sub080941F4Delta(&step, pos, &route->points[route->index]);
    anchor = &route->anchor->position;
    Sub080941F4Delta(&delta, pos, anchor);
    actor->direction = actor->fieldE9 = Math_Atan2(delta.x, delta.z);
    if ((s16)route->timer > 0 && step.x * step.x + step.z * step.z > 1024) {
        n = Sub080941F4Shift5((s16)route->timer);
        if (n < 1)
            n = 1;
        actor->motion.x += Div(step.x, n);
        actor->motion.z += Div(step.z, n);
        route->motion = actor->motion;
        route->timer -= actor->counter2AC;
    } else {
        route->index++;
        if (route->index > 8) {
            pos->x = route->points[8].x;
            pos->z = route->points[8].z;
            anchor = &route->anchor->position;
            Sub080941F4Delta(&delta, pos, anchor);
            actor->fieldE9 = Math_Atan2(delta.x, delta.z);
            Sub080941F4Polar(&target,
                             Sub080941F4Facing(Sub080941F4RandomMotion(), &actor->fieldE9) & 255,
                             route->polarSpeed);
            target.x += pos->x;
            target.y += pos->y;
            target.z += pos->z;
            sub_0809345C(actor, &target);
            Bee_SubVector(actor, pos, &target);
            route->timer = route->speed << 5;
            route->index = 0;
            actor->counter2AC = 0;
            return;
        }
        pos->x = route->points[route->index - 1].x;
        pos->z = route->points[route->index - 1].z;
    }
    if (actor->counter2AC <= 19)
        actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080941F4);
#endif
