// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
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
static inline s32 NextRandom(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    {
        register const u16 *table asm("r2") = gRandomTable;
        return *(const u16 *)(offset + (u32)table);
    }
}
static inline s32 NextRandomFirst(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline void Polar(struct ContactPosition *out, u8 angle, s32 inputSpeed)
{
    register s32 speed asm("r4") = inputSpeed;
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle] * speed);
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
    u8 taken = TakeFlag(&actor->field2B2);
    if (taken) {
        u32 mask = 4;
        SetColliderFlags((struct ContactCollider *)(context + 0x624), mask);
        SetColliderFlags((struct ContactCollider *)(context + 0x678), mask);
        SetColliderFlags(&actor->colliderA, mask);
        ClearColliderFlags(&actor->colliderB, mask);
        {
            u8 direction = actor->body.direction;
            register u32 observed asm("r12") = 0x0403B714;
            register s32 random asm("r1") = NextRandomFirst();
            asm volatile("" : : "r"(observed));
            random &= 0xF;
            {
                u32 angle = direction;
                Polar(&actor->secondaryMotion, angle, random + 0x1C);
            }
        }
        actor->secondaryMotion.y = (NextRandom() & 0x1F) + 0x18;
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
