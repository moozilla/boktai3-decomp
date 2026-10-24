// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining: unavailable-result register, context address lifetimes and argument-store scheduling.
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_control_phase.h"
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000580[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
u8 *SceneBody_GetById(int);
void *Bee_SubVector(void *, void *, void *);

static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
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
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(const u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400);
}
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
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

static inline void Add(struct BeeVec *a, struct BeeVec *b)
{
    a->x = b->x + a->x;
    a->y = a->y + b->y;
    a->z = b->z + a->z;
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

void Bee_StartMoveToBody(struct SceneActorRegistryResetView *actor, u16 *args)
{
    register u8 *body asm("r8");
    volatile u32 fill;
    struct BeeVec delta;
    struct BeeVec offset;
    u32 angle;
    typeof(body) c1;
    typeof(args[2]) t1;
    if (IsUnavailable(actor))
        return;
    if (!ActorFlag(actor, 0x4000, 0x2D4))
        return;
    body = actor->context;
    c1 = body;
    {
        u16 value = args[1];
        *((u16 *)(c1 + 0x43C)) = value;
    }
    *((u8 **)(c1 + 0x440)) = SceneBody_GetById(*((u16 *)(c1 + 0x43C)));
    t1 = args[2];
    *((u16 *)(c1 + 0x43E)) = t1;
    c1[0x428] = 0;
    Diff(&delta, (struct BeeVec *)(&actor->position),
         (struct BeeVec *)((*((u8 **)(c1 + 0x440))) + 0xC));
    actor->fieldE9 = Math_Atan2(delta.x, delta.z);
    angle = ((Rand() & 0x3F) + actor->fieldE9) & 0xFF;
    Polar(&offset, angle, 0x200);
    *((u16 *)(body + 0x43A)) = 0x500;
    Add_StartMoveToBody(&offset, (struct BeeVec *)(&actor->position));
    sub_0809345C(actor, &offset);
    Bee_SubVector(actor, &actor->position, &offset);
    Clear(&fill, body + 0x480);
    State(actor, 2, 0, 10);
    Control(actor, actor->entryTarget228, 1);
}

#else
INCLUDE_ASM("asm/nonmatching", Bee_StartMoveToBody);
#endif
