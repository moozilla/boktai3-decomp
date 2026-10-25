// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_route.h"
extern u8 *gUnk_02000580[];
static inline u8 Take(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
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
static inline void Scale(struct SceneActorRegistryResetView *actor, u16 *flags)
{
    u16 *scale;
    {
        u32 mask = 0xA02F;
        if (*flags & mask) {
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

void Ghost_UpdateEnergyDrainAnimation(void);
void SceneActorControl_OnIdle(void);
void Sound_PlaySong(u32);
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void ClearCollider(struct ContactCollider *c)
{
    struct ColliderFlags {
        u8 pad[6];
        s32 flags : 16;
    };
    ((struct ColliderFlags *)c)->flags &= ~4;
}
static inline void State(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SaveInitialPlayer(struct ContactPosition *out, u8 **players)
{
    u8 **source = players;
    *out = *(struct ContactPosition *)(*source + 0x30);
}
#ifdef NONMATCHING
// fuzzy: 94.9%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: player-pointer copies, control-field offsets and animation-scale allocation.
// confidence=low: follows the player's displacement while draining, then returns to idle.
inline static void ClearCollider_FollowDrainedPlayer(struct ContactCollider *c)
{
    struct ColliderFlags {
        u8 pad[6];
        s32 flags : 16;
    };
    {
        u8 k1 = 4;
        ((struct ColliderFlags *)c)->flags &= ~k1;
    }
}

void Ghost_FollowDrainedPlayer(struct SceneActorRegistryResetView *actor)
{
    register u16 *flags asm("r6") = &actor->flags2D4;
    u8 *context = actor->context;
    struct ContactPosition delta;
    u8 taken = Take(&actor->field2B1);
    if (taken) {
        Control(actor, Ghost_UpdateEnergyDrainAnimation, 0x23);
        SaveInitialPlayer((void *)(context + 0x5EC), gUnk_02000580);
        {
            u32 mask = 0x100;
            *flags = mask | (*flags);
        }
        {
            u32 mask = 0x2000000;
            actor->flags2CC |= mask;
        }
        actor->direction = actor->fieldE9;
    }
    {
        u32 scale = 64;
        actor->field1EA = scale;
        Delta(&delta, (struct ContactPosition *)(context + 0x5EC),
              (void *)((*gUnk_02000580) + 0x30));
        actor->motion.x += delta.x;
        actor->motion.z += delta.z;
        *((struct ContactPosition *)(context + 0x5EC)) =
            *((struct ContactPosition *)((*gUnk_02000580) + 0x30));
        Scale(actor, flags);
    }
    if (actor->field2B0 || ((*((u16 *)(gUnk_02000580[0] + 0x428))) == 0)) {
        Sound_PlaySong(0x3CC);
        State(actor, 3);
        Control(actor, SceneActorControl_OnIdle, 0x10);
        ClearCollider_FollowDrainedPlayer(&actor->colliderA);
    } else {
        ++actor->counter2AC;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Ghost_FollowDrainedPlayer);
#endif
