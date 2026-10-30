#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "scene_actor_route.h"
#include "player.h"

struct GhostDrainContext {
    u8 unknown0[0x5EC];
    struct ContactPosition playerPosition;
};
static inline u8 Ghost_TakeStateEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void Ghost_SetControl(struct SceneActorRegistryResetView *a, void *cb, u32 kind)
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
static inline void Ghost_UpdateAnimationScale(struct SceneActorRegistryResetView *actor, u16 *flags)
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
static inline void Ghost_SubtractPositions(struct ContactPosition *out,
                                           const struct ContactPosition *from,
                                           const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Ghost_SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Ghost_SavePlayerPosition(struct ContactPosition *out, struct Player **players)
{
    struct Player **source = players;
    *out = *(struct ContactPosition *)&(*source)->position;
}
/* Follow the drained player's displacement until the drain finishes. */
static inline void Ghost_ClearDrainCollider(struct ContactCollider *c)
{
    struct ColliderFlags {
        u8 pad[6];
        s32 flags : 16;
    };
    {
        u8 mask = 4;
        ((struct ColliderFlags *)c)->flags &= ~mask;
    }
}

static inline u16 *Ghost_BeginPlayerFollow(struct SceneActorRegistryResetView *actor,
                                           struct GhostDrainContext *context)
{
    u8 taken = Ghost_TakeStateEntry(&actor->field2B1);
    if (taken) {
        Ghost_SetControl(actor, Ghost_UpdateEnergyDrainAnimation, 0x23);
        Ghost_SavePlayerPosition(&context->playerPosition, gPlayers);
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = 0x100;
            *flags = mask | (*flags);
        }
        {
            u32 mask = 0x2000000;
            actor->flags2CC |= mask;
        }
        actor->body.direction = actor->tracking.heading;
    }
    return &actor->flags2D4;
}

void Ghost_FollowDrainedPlayer(struct SceneActorRegistryResetView *actor)
{
    struct GhostDrainContext *context = actor->context;
    struct ContactPosition delta;
    u16 *flags = Ghost_BeginPlayerFollow(actor, context);
    {
        u32 scale = 64;
        actor->field1EA = scale;
        Ghost_SubtractPositions(&delta, &context->playerPosition,
                                (struct ContactPosition *)&gPlayers[0]->position);
        actor->body.motion.x += delta.x;
        actor->body.motion.z += delta.z;
        context->playerPosition = *((struct ContactPosition *)&gPlayers[0]->position);
        Ghost_UpdateAnimationScale(actor, flags);
    }
    if (actor->field2B0 || (gPlayers[0]->currentStatB == 0)) {
        Sound_PlaySong(0x3CC);
        Ghost_SetBehavior(actor, 3);
        Ghost_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        Ghost_ClearDrainCollider(&actor->colliderA);
    } else {
        ++actor->counter2AC;
    }
}
