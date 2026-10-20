#include "global.h"
#include "scene_actor_reaction.h"
/* Skeleton_BeginBoneScatter: on entry faces a cardinal direction, spawns two bone debris bursts (sub_08127590) above the body at random headings, becomes category 5, restarts its variant motion and re-arms the body's attack collider; hands over to the threshold reaction (kind 0x3E) once field2B3 is set (provisional name) */
#define Skeleton_BeginBoneScatter sub_080BA268
void sub_08127590(struct ContactPosition *, struct ContactPosition *, u32, u32, u32, u32, u32, u32,
                  u32, u32, u32, u32, u32, u32, u32);
void sub_080B7350(struct SceneActorRegistryResetView *);
void Skeleton_InitVariantMotion(struct SceneActorRegistryResetView *);
void SceneActorReaction_ApplyThreshold(void);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
extern const s16 gUnk_086149C4[];

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline s32 Div64(s32 n)
{
    if (n >= 0)
        return n >> 6;
    return -((-n) >> 6);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xFF] * speed);
}
static inline s32 NextRandom(void)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)gUnk_0203B400);
}
static inline void Sum(struct ContactPosition *out, const struct ContactPosition *a,
                       const struct ContactPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline void Add(struct ContactPosition *a, struct ContactPosition *b)
{
    Sum(a, b, a);
}

void sub_080BA268(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    if (TakeFlag(&actor->field2B2)) {
        struct ContactPosition facingStep, velocity, position;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0x16);
        {
            u32 facing = Div64((actor->direction + 0x10) & 0xFF) << 6;
            Polar(&facingStep, facing, 0x14);
        }
        {
            u32 heading = NextRandom() & 0xFF;
            Polar(&velocity, heading, 10);
        }
        velocity.y = 0x1E;
        position = actor->position;
        position.y += 0x104;
        Add(&position, &velocity);
        sub_08127590(&position, &velocity, 0xD45E, 0x1DC3, 2, 0x1E, 2, 0, 0x13C, 0, 0, 0, 0, 0, 0);
        {
            u32 heading = NextRandom() & 0xFF;
            Polar(&velocity, heading, 10);
        }
        velocity.y = 0x1E;
        position.y += 0xDC;
        Add(&position, &velocity);
        sub_08127590(&position, &velocity, 0xD45E, 0x1DC3, 3, 0x1E, 2, 0, 0x13C, 0, 0, 0, 0, 0, 0);
        actor->category = 5;
        sub_080B7350(actor);
        Skeleton_InitVariantMotion(actor);
        Collider_SetAttackProperties(context + 0x678, *(u16 *)((u8 *)actor + 0xBA),
                                     *(u16 *)((u8 *)actor + 0xC2), 0, *(u32 *)((u8 *)actor + 0xD8),
                                     *(u16 *)((u8 *)actor + 0xCA));
    }
    if (actor->field2B3) {
        void *callback = (void *)SceneActorReaction_ApplyThreshold;
        u32 kind = 0x3E;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
    }
}
