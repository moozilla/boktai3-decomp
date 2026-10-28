#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#include "player.h"
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void sub_0809345C(void *, void *);
void Bee_SubVector(void *, void *, void *);
s32 Bee_GetMovementVariant(void *);
#ifdef NONMATCHING
// fuzzy: 90.6%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: segment address lifetimes and register allocation.
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 *p, u32 m)
{
    if (*p & m)
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
    return gUnk_0203B400[gRandomIndex];
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
static inline void State(struct SceneActorRegistryResetView *a)
{
    u32 behavior = 2;
    a->state2A8 = 0;
    a->phase2A9 = 0;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}

struct SavedPair {
    u32 x, z;
};
struct SavedMotion {
    u8 pad[12];
    struct ContactPosition position;
};

void Crow_UpdateDirectionalPose(void);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
void TentacleActor_SelectWanderRoute(void *);
void TentacleActor_AdvanceMotionPhase(void *, s32);
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_UpdateOscillation(void *, s32, s32);
static inline u8 Approach(struct SceneActorRegistryResetView *actor, struct SavedMotion *saved,
                          u8 *speedp)
{
    u8 *motion = (u8 *)actor + 0x50;
    s32 speed = *speedp;
    s16 dx = saved->position.x - *(u16 *)(motion + 12);
    s16 dz = saved->position.z - *(u16 *)(motion + 16);
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion[9] = Math_Atan2(dx, dz);
        *(u16 *)(motion + 20) += Shift(gUnk_086149C4[(motion[9] + 64) & 255] * speed, 12);
        *(u16 *)(motion + 24) += Shift(gUnk_086149C4[motion[9]] * speed, 12);
    }
    return 0;
}
static inline void Scale(struct SceneActorRegistryResetView *actor)
{
    u16 *scale;
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *p = &actor->field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
}

// MATCHING_CONCESSION: register-pinning
static inline void AddVectors(struct ContactPosition *out, const struct ContactPosition *a,
                              const struct ContactPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = b->z + a->z;
}
inline static void AddVectors_ReturnToCellCenter(struct ContactPosition *out,
                                                 const struct ContactPosition *a,
                                                 const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

void TentacleBody_ReturnToCellCenter(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    s32 speed;
    s32 difference;
    s32 index;
    u8 *direction;
    struct ContactPosition *motion;
    struct ContactPosition target;
    if (Take(&actor->field2B1)) {
        {
            u32 cell = actor->terrainCell;
            asm("" : "+r"(cell));
            *((u16 *)(body + 0x87a)) = cell;
        }
        SceneActorTerrain_GetCenter(*((u16 *)(body + 0x87a)), (void *)(body + 0x924));
    }
    Callback_InvokeActor(actor, *((void **)(((u8 *)actor) + 0x270)));
    speed = Callback_InvokeActor(actor, *((void **)(((u8 *)actor) + 0x260)));
    Scale(actor);
    difference = (*(((u8 *)actor) + 0x3cd)) - actor->direction;
    direction = &actor->direction;
    if (difference > 128)
        difference += -256;
    else if (difference < (-128))
        difference += 256;
    {
        s32 magnitude = (difference < 0) ? (-difference) : (difference);
        motion = &actor->motion;
        if (magnitude > 8) {
            s32 x = (u8)Shift(speed * gUnk_086149C4[((*(((u8 *)actor) + 0x59)) + 64) & 255], 12);
            s32 z = Shift(speed * gUnk_086149C4[*(((u8 *)actor) + 0x59)], 12);
            motion->x += x;
            actor->motion.z += z;
            motion = &actor->motion;
        }
    }
    AddVectors_ReturnToCellCenter(&target, &actor->motion, &actor->position);
    index = SceneActorTerrain_GetCellIndex(&target);
    if ((*((u16 *)(body + 0x87a))) != index) {
        Delta(&target, &target, (void *)(body + 0x924));
        motion->x += Div(target.x, 10);
        actor->motion.z += Div(target.z, 10);
    }
    sub_080A9DB0(actor, 12, 0xa410);
    if (((difference < 0) ? (-difference) : (difference)) <= 7) {
        if ((*((u16 *)(body + 0x87a))) == index)
            State(actor);
    } else if (difference > 0) {
        *(((u8 *)actor) + 0x59) += (((64 - actor->field2C2) >> 4) << 1) + 1;
    } else {
        *(((u8 *)actor) + 0x59) -= (((64 - actor->field2C2) >> 4) << 1) + 1;
    }
    actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", TentacleBody_ReturnToCellCenter);
#endif
