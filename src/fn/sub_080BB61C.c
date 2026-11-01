#include "global.h"
#include "scene_actor_reaction.h"
#include "monster_particle_random.h"
/* Skeleton_BeginKnockedFlight: on entry makes the body's colliders inert, launches the actor along its facing at a random speed (0x1C-0x2B) with a random upward speed, and shows its shadow; on landing (motion bit 0) stops, and once flags2D0 & 0x10 plays the bone sound (if enabled) and resets the actor (provisional name) */
void SceneActorPlacement_ResetActorState(void *, u16 *);
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
#define SCALE_AXIS(out, sample, speed)                                                             \
    do {                                                                                           \
        (out) = (sample) * (speed);                                                                \
    } while (0)
static inline void Polar(struct ContactPosition *out, u8 angle, s32 speed)
{
    s32 sample, product;
    sample = gUnk_086149C4[(angle + 0x40) & 0xFF];
    SCALE_AXIS(product, sample, speed);
    out->x = Scale12(product);
    out->y = 0;
    sample = gUnk_086149C4[angle];
    SCALE_AXIS(product, sample, speed);
    out->z = Scale12(product);
}
static inline void SetColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags |= mask;
}
static inline void ClearColliderFlags(struct ContactCollider *collider, u32 mask)
{
    collider->flags &= ~mask;
}
static inline u8 TestField(u32 mask, u32 *field)
{
    if (*field & mask)
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

void Skeleton_BeginKnockedFlight(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *context = actor->context;
    u32 origin;
    u8 taken = TakeFlag(&actor->field2B2);
    if (taken) {
        u32 mask = 4;
        SetColliderFlags((struct ContactCollider *)(context + 0x624), mask);
        SetColliderFlags((struct ContactCollider *)(context + 0x678), mask);
        SetColliderFlags(&actor->colliderA, mask);
        ClearColliderFlags(&actor->colliderB, mask);
        {
            u8 direction = actor->body.direction;
            s32 random;
            PARTICLE_RANDOM_CAPTURE_ORIGIN(origin);
            random = ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR);
            random &= 0xF;
            {
                u32 angle = direction;
                Polar(&actor->secondaryMotion, angle, random + 0x1C);
            }
        }
        actor->secondaryMotion.y =
            (ParticleRandom_ReadRelativeSample(origin, (const u16 *)RANDOM_TABLE_ADDR) & 0x1F) +
            0x18;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0x11);
        ActorShadow_Enable((u8 *)actor + 0x320);
    } else {
        if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
            actor->secondaryMotion.x = taken;
            actor->secondaryMotion.z = taken;
        }
        {
            u32 mask = 0x10;
            if (actor->flags2D0 & mask) {
                u16 argument;
                if (TestField(0x800, (u32 *)(context + 0x6DC)))
                    Sound_PlaySong(0x3AD);
                SetColliderFlags(&actor->colliderB, 4);
                argument = 1;
                SceneActorPlacement_ResetActorState(actor, &argument);
            }
        }
    }
}
