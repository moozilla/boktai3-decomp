#include "scene_actor_displacement.h"
#include "scene_actor_control_phase.h"
#include "player.h"
#include "engine.h"
u32 SceneActor_GetDirectionFlags(u32);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);
void SceneActor_ResetCollisionReaction(struct SceneActorRegistryResetView *);
void SceneActorAnimation_SelectEntryFlags(struct SceneActorAnimationView *, s32, u32);
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void SetFlags(u16 *flags, u16 mask)
{
    *flags = mask | *flags;
}
static inline void ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline void SetBehavior(struct SceneActorRegistryResetView *actor, u32 b)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void Mimic_ConvergeAndTurn(struct SceneActorRegistryResetView *actor)
{
    s16 *px, *pz;
    s32 x, z;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectEntryFlags((void *)actor, 3,
                                             SceneActor_GetDirectionFlags(actor->body.direction));
        {
            u16 mask = 0x10;
            u16 *flags = &actor->flags2D6;
            *flags = mask | *flags;
        }
    }
    x = SceneActorDisplacement_ReadAxis(actor->reactionStart.x, &actor->reactionEnd.x, &px);
    z = SceneActorDisplacement_ReadAxis(actor->reactionStart.z, &actor->reactionEnd.z, &pz);
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *px += x;
    *pz += z;
    if (AtRest(x, z) && actor->field2B3) {
        u16 *motion = &actor->flags2D4;
        u16 *flags;
        u32 hit;
        struct SceneActorChaseTracking *tracking;
        ClearBits(motion, 0x20);
        hit = gEngineFlags & 0x4800;
        flags = motion;
        if (Test(hit, hit)) {
            if (!SceneActorTarget_TrySelectNearestParticipant(actor)) {
                SceneActor_ResetCollisionReaction(actor);
                return;
            }
        } else {
            actor->tracking.target = (void *)((u8 *)gPlayers[0] + 0x24);
        }
        tracking = &actor->tracking;
        if (tracking->target) {
            struct ContactPosition d;
            Delta(&d, &actor->body.position, &tracking->target->position);
            actor->body.direction = actor->tracking.heading = Math_Atan2(d.x, d.z);
            SetFlags(&actor->group.common.gate.flags, 4);
            SetFlags(flags, 0x80);
            actor->group.common.gate.counter = actor->group.common.gate.duration;
            ClearBits((u16 *)((u8 *)actor->context + 0x65C), 1);
            {
                u8 mask = 8;
                tracking->flags |= mask;
            }
            SetBehavior(actor, 2);
        } else {
            SetBehavior(actor, 0);
        }
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC = mask | actor->flags2CC;
    }
}
