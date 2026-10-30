#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
u8 *SceneBody_GetById(int);
void *Bee_SubVector(void *, void *, void *);

static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + 0x0203B400);
}
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? actor->flags2D4 & mask : actor->flags2D0 & mask;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
struct BeeVec {
    s16 x, y, z;
};
static inline void Diff(struct BeeVec *d, struct BeeVec *a, struct BeeVec *b)
{
    d->x = b->x - a->x;
    d->y = b->y - a->y;
    d->z = b->z - a->z;
}
static inline void Polar(struct BeeVec *out, u16 angle, s32 radius)
{
    out->x = Shift(gUnk_086149C4[(angle + 0x40) & 255] * radius, 12);
    out->y = 0;
    out->z = Shift(gUnk_086149C4[angle & 255] * radius, 12);
}

static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    u8 *taken = &a->field2B2;
    zero = 0;
    *taken = 1;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void Clear(u32 *zero, void *out)
{
    *zero = 0;
    CpuSet(zero, out, 0x05000002);
}

static inline void State(struct SceneActorRegistryResetView *a, u32 state, u32 phase, u32 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
inline static void Add_StartMoveToBody(struct BeeVec *a, struct BeeVec *b)
{
    a->x = a->x + b->x;
    a->y = a->y + b->y;
    a->z = a->z + b->z;
}

struct BeeMoveTarget {
    u8 unknown0[0xC];
    struct BeeVec position;
};
/* Tentative context view for body-directed movement. */
struct BeeMoveContext {
    u8 unknown0[0x428];
    u8 state;
    u8 unknown429[0x11];
    u16 distance;
    u16 bodyId;
    u16 parameter;
    struct BeeMoveTarget *target;
    u8 unknown444[0x3C];
    struct ContactPosition motion;
};
void Bee_StartMoveToBody(struct SceneActorRegistryResetView *actor, u16 *args)
{
    struct BeeMoveContext *body;
    volatile u32 fill;
    struct BeeVec delta;
    struct BeeVec offset;
    u32 angle;

    u16 parameter;
    if (IsUnavailable(actor))
        return;
    if (!ActorFlag(actor, 0x4000, 0x2D4))
        return;
    body = actor->context;
    {
        u16 value = args[1];
        body->bodyId = value;
    }
    body->target = (void *)SceneBody_GetById(body->bodyId);
    parameter = args[2];
    body->parameter = parameter;
    body->state = 0;
    Diff(&delta, (struct BeeVec *)(&actor->body.position), &body->target->position);
    actor->tracking.heading = Math_Atan2(delta.x, delta.z);
    angle = ((Rand() & 0x3F) + actor->tracking.heading) & 0xFF;
    Polar(&offset, angle, 0x200);
    body->distance = 0x500;
    Add_StartMoveToBody(&offset, (struct BeeVec *)(&actor->body.position));
    sub_0809345C(actor, &offset);
    Bee_SubVector(actor, &actor->body.position, &offset);
    Clear(&fill, &body->motion);
    State(actor, 2, 0, 10);
    Control(actor, actor->entryTarget228, 1);
}
