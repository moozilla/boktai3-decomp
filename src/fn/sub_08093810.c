#include "global.h"
#ifdef NONMATCHING
// fuzzy: 80.5%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences include stack-vector addressing, waypoint address reuse and registers.
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "engine.h"
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000580[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 v, u32 m)
{
    if (v & m)
        return 1;
    return 0;
}
static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + (u32)gUnk_0203B400);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Shift(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Shift(gUnk_086149C4[angle & 255] * speed, 12);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Add(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = from->x + out->x;
    out->y = out->y + from->y;
    out->z = from->z + out->z;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    u32 offset;
    u8 *taken = &a->field2B2;
    zero = 0;
    *taken = 1;
    offset = 0x2B0;
    *((u8 *)a + offset) = zero;
    offset += 5;
    *((u8 *)a + offset) = kind;
    a->target = cb;
    offset += 0x13;
    *(u16 *)((u8 *)a + offset) = zero;
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
static inline void State(struct SceneActorRegistryResetView *a, u32 phase, u32 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

struct MotionPair {
    u32 x, z;
};

// confidence=low: follows a randomized route and returns to the Bee behavior selector.
void Bee_FollowWanderRoute(struct SceneActorRegistryResetView *actor)
{
    struct {
        u32 zero;
        struct ContactPosition dest;
        struct ContactPosition delta;
    } buf;
    u8 *body = actor->context;
    u32 k1 = 0x3D4;
    if (Take(&actor->field2B1)) {
        Control(actor, actor->entryTarget228, 1);
        body[0x428] = 0;
        if (Test(*((u32 *)(body + k1)), 8)) {
            {
                u8 k2 = 128;
                Polar(&buf.dest, ((*(((u8 *)actor) + 0xE9)) + ((Rand() & 63) + k2)) & 255, 320);
            }
            *((u32 *)(body + k1)) &= ~8;
            {
                s32 variant = Bee_GetMovementVariant(actor);
                *((u16 *)(body + 0x43A)) = 6 << variant;
            }
        } else {
            Polar(&buf.dest, ((Rand() & 63) + (*(((u8 *)actor) + 0xE9))) & 255, 512);
            {
                s32 variant = Bee_GetMovementVariant(actor);
                *((u16 *)(body + 0x43A)) = 40 << variant;
            }
        }
        Add(&buf.dest, &actor->position);
        sub_0809345C(actor, &buf.dest);
        Bee_SubVector(actor, &actor->position, &buf.dest);
        Clear(&buf.zero, body + 0x480);
    }
    Delta(&buf.delta, &actor->position,
          (struct ContactPosition *)((body + 0x3E0) + (body[0x428] * 8)));
    ((u8 *)actor)[0x59] = *(((u8 *)actor) + 0xE9);
    if (((*((s16 *)(body + 0x43A))) > 0) &&
        (((buf.delta.x * buf.delta.x) + (buf.delta.z * buf.delta.z)) > 1024)) {
        s32 steps = Shift(*((s16 *)(body + 0x43A)), 5);
        {
            u8 k3 = 1;
            if (steps < k3)
                steps = 1;
        }
        *((u16 *)(((u8 *)actor) + 0x64)) +=
            Div((*((s16 *)((body + 0x3E0) + (body[0x428] * 8)))) - actor->position.x, steps);
        *((u16 *)(((u8 *)actor) + 0x68)) +=
            Div((*((s16 *)((body + 0x3E4) + (body[0x428] * 8)))) - actor->position.z, steps);
        *((struct MotionPair *)(body + 0x480)) = *((struct MotionPair *)(((u8 *)actor) + 0x64));
        *((u16 *)(body + 0x43A)) -= actor->counter2AC;
    } else if ((++body[0x428]) > 8) {
        actor->position.x = *((u16 *)(body + 0x420));
        actor->position.z = *((u16 *)(body + 0x424));
        Polar(&buf.dest, ((Rand() & 63) + (*(((u8 *)actor) + 0xE9))) & 255, 512);
        Add(&buf.dest, &actor->position);
        sub_0809345C(actor, &buf.dest);
        Bee_SubVector(actor, &actor->position, &buf.dest);
        {
            s32 variant = Bee_GetMovementVariant(actor);
            *((u16 *)(body + 0x43A)) = (*((u16 *)(body + 0x42C))) << variant;
        }
        body[0x428] = 0;
        actor->counter2AC = 0;
        return;
    } else {
        actor->position.x = *((u16 *)((body + 0x3E0) + ((body[0x428] - 1) * 8)));
        actor->position.z = *((u16 *)((body + 0x3E4) + ((body[0x428] - 1) * 8)));
    }
    if (((s32)actor->counter2AC) <= 19)
        actor->counter2AC++;
    if (body[0x42A])
        body[0x42A]--;
    else if ((*((u32 *)(((u8 *)actor) + 0xF0))) <= 0xFFFE) {
        if (body[0x3DC])
            State(actor, 2, 1);
        else
            State(actor, 2, 2);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Bee_FollowWanderRoute);
#endif
