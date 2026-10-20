#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.1%
// Remaining differences are register allocation in the random-motion setup.
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
extern u8 *gUnk_02000580[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
void sub_08093644(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
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

void Bee_DisableMovement(void *);
void Bee_BeginMoveToPosition(void *, void *);
// confidence=low: initializes a small random movement and returns to the player when it stops.
void Bee_ReturnToPlayerWhenStopped(struct SceneActorRegistryResetView *actor)
{
    u32 zero;
    s32 length;
    struct ContactPosition *motion;
    if (Take(&actor->field2B1)) {
        u8 *body = actor->context;
        u32 angle;
        Control(actor, actor->entryTarget228, 1);
        Clear(&zero, body + 0x480);
        angle = (u8)Rand();
        *(s16 *)((u8 *)actor + 0x2EC) = Shift(gUnk_086149C4[angle], 6);
        *(s16 *)((u8 *)actor + 0x2F0) = Shift(gUnk_086149C4[(angle + 64) & 255], 6);
    }
    motion = (struct ContactPosition *)((u8 *)actor + 0x2EC);
    length = motion->x * motion->x + motion->z * motion->z;
    if (!length) {
        Bee_DisableMovement(actor);
        Clear(&zero, (u8 *)actor + 0x2EC);
        Bee_BeginMoveToPosition(actor, gUnk_02000580[0] + 0x30);
        State(actor, 2, 0);
    }
    actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", Bee_ReturnToPlayerWhenStopped);
#endif
