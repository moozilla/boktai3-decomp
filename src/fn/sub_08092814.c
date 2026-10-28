// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];
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

static inline s32 Shift(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline u32 Rand(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
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
        u32 o1;
        Control(actor, actor->entryTarget228, 1);
        Clear(&zero, body + 0x480);
        angle = (u8)Rand();
        {
            const s16 *tab1 = gUnk_086149C4;
            s32 vx;
            s16 *q;
            s32 t;
            t = tab1[angle];
            asm("" ::"r"(tab1));
            vx = Shift(t, 6);
            o1 = 0x2EC;
            q = (s16 *)((u8 *)actor + o1);
            *q = vx;
        }
        {
            const s16 *tab = gUnk_086149C4;
            u32 idx;
            u32 msk;
            s32 vz;
            register u32 o asm("r2");
            register s16 *q asm("r0");
            idx = angle + 64;
            msk = 255;
            idx &= msk;
            vz = Shift(tab[idx], 6);
            o = 0x2F0;
            asm("" : "+r"(o));
            q = (s16 *)((u8 *)actor + o);
            *q = vz;
        }
    }
    {
        register u32 mo asm("r6") = 0x2EC;
        u8 *ma;
        asm("" : "+r"(mo));
        ma = (u8 *)actor + mo;
        motion = (struct ContactPosition *)ma;
    }
    length = motion->x * motion->x + motion->z * motion->z;
    if (!length) {
        Bee_DisableMovement(actor);
        Clear(&zero, (u8 *)motion);
        Bee_BeginMoveToPosition(actor, ((u8 **)gPlayers)[0] + 0x30);
        State(actor, 2, 0);
    }
    actor->counter2AC++;
}
