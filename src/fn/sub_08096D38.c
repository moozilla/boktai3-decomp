// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
struct GhostPlayer {
    u8 pad[0x30];
    struct ContactPosition position;
};
extern struct GhostPlayer *gUnk_02000580;
void Ghost_EmitAfterimage(struct SceneActorRegistryResetView *);
void sub_08098344(void *, void *);
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
struct GhostRoute {
    u8 pad[0x558];
    u32 flags;
    u32 distance;
    u8 pad560[6];
    u16 speed;
    struct ContactPosition points[9];
    u8 index;
    u8 pad5B1[0x5FC - 0x5B1];
    struct ContactPosition motion;
};
static inline s32 Scale(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * speed);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u32 RandomFirst(u32 bits)
{
    u32 index, value;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    index = gUnk_03005308 * 2;
    value = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    return value & bits;
}
static inline u16 Random(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u16 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400);
}
static inline u16 RandomMotion(void)
{
    u32 offset;
    const u16 *table;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    table = gUnk_0203B400;
    return *(const u16 *)(offset + (u32)table);
}
static inline u32 Facing(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}
void SceneActorControl_OnIdle(void);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void Control(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
{
    u32 zero;
    a->field2B2 = 1;
    zero = 0;
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
static inline void State(struct SceneActorRegistryResetView *a, u8 phase, u8 behavior)
{
    a->state2A8 = 0;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline void Add(struct ContactPosition *out, const struct ContactPosition *from)
{
    out->x = from->x + out->x;
    out->y = out->y + from->y;
    out->z = from->z + out->z;
}
static inline s32 Quarter(s32 n)
{
    if (n >= 0)
        return n >> 2;
    return -(-n >> 2);
}
static inline void ClearCollider(struct ContactCollider *c)
{
    struct SignedFlags {
        u8 pad[6];
        s32 bits : 16;
    };
    ((struct SignedFlags *)c)->bits &= ~4;
}
static inline void SetCollider(struct ContactCollider *c)
{
    c->flags |= 4;
}
// Remaining differences: inline ordering, flag/address lifetimes, and register allocation.
// confidence=low: follows a finite curved route, either withdrawing or moving toward the player.
inline static void ClearCollider_AdvanceApproachRoute(struct ContactCollider *c)
{
    struct SignedFlags {
        u8 pad[6];
        s32 bits : 16;
    };
    {
        u8 k1 = 4;
        ((struct SignedFlags *)c)->bits &= ~k1;
    }
}

inline static void Polar_1(struct ContactPosition *out, u32 angle)
{
    {
        typeof(out->x) *q1 = &out->x;
        *q1 = Scale(gUnk_086149C4[(angle + 64) & 255] * 728);
    }
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * 728);
}

void Ghost_AdvanceApproachRoute(struct SceneActorRegistryResetView *actor)
{
    struct GhostRoute *route = actor->context;
    struct ContactPosition target;
    struct ContactPosition delta;
    u32 *flags = &route->flags;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        Control(actor, actor->entryTarget228, 1);
        route->index = 0;
        if (Test(*flags, 4)) {
            u32 angle = (actor->fieldE9 + ((Random() & 63) + 128)) & 255;
            Polar_1(&target, angle);
            Add(&target, &actor->position);
        } else {
            target = gUnk_02000580->position;
        }
        Delta(&delta, &actor->position, &target);
        route->distance = Quarter((delta.x * delta.x) + (delta.z * delta.z));
        Polar(&route->motion, Facing(RandomMotion(), &actor->fieldE9) & 255, 32);
        sub_08098344(actor, &target);
        route->speed = 1;
        {
            u32 mask = 0x100;
            actor->flags2D4 = mask | actor->flags2D4;
        }
        actor->field2C2 = 32;
    }
    Delta(&delta, &actor->position, &route->points[route->index]);
    if (!Test(*flags, 4))
        ClearCollider_AdvanceApproachRoute(&actor->colliderB);
    Ghost_EmitAfterimage(actor);
    if (((delta.z * delta.z) + (delta.x * delta.x)) > 1024) {
        actor->direction = Math_Atan2(route->points[route->index].x - actor->position.x,
                                      route->points[route->index].z - actor->position.z);
        Polar(&delta, actor->direction, route->speed);
        actor->motion.x += delta.x;
        actor->motion.z += delta.z;
        route->motion = actor->motion;
    } else if ((++route->index) > 8) {
        actor->position.x = route->points[8].x;
        actor->position.z = route->points[8].z;
        actor->flags2D4 &= ~0x100;
        SetCollider(&actor->colliderB);
        if (Test(*flags, 4)) {
            State(actor, 0, 0);
            Control(actor, SceneActorControl_OnIdle, 16);
            *flags &= ~4;
        } else {
            State(actor, 2, 1);
            Control(actor, SceneActorControl_OnIdle, 16);
        }
        return;
    }
    if (actor->paletteOffset == 0) {
        if ((Mod(actor->counter2AC, 8) == 0) && (route->speed <= 19))
            ++route->speed;
    } else if ((Mod(actor->counter2AC, 4) == 0) && (route->speed <= 19))
        route->speed += 2;
}

#else
INCLUDE_ASM("asm/nonmatching", Ghost_AdvanceApproachRoute);
#endif
