#include "global.h"
// confidence=low: settles reaction displacement and restores normal actor control.
#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_reaction.h"
#include "scene/motion/scene_actor_displacement.h"
#include "engine/video/display_sequence.h"
#include "scene/motion/scene_actor_turn.h"
#include "scene/motion/scene_actor_chase_shared.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u8, u8, u8);

static inline s32 Reaction_ThirtySecond(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}

static inline u8 Reaction_MotionStopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline void Recovery_ClearTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = ~bits;
    tracking->flags &= mask;
}

static inline void Recovery_ClearRoute(struct ChaseActor *actor,
                                       struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearActorFlags(actor, 0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}

static inline u8 Recovery_ActorCollisionFlags(struct ChaseActor *actor, u32 bits)
{
    return SceneActorChase_TestField(&actor->base.placementFlags, bits);
}

void SceneActorReaction_SettleDisplacement(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 counter;
    s16 *priorX, *priorZ;
    s32 x;
    s32 z;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 category = actor->base.category;
        struct SceneActorChaseFlags16 *motion;
        actor->base.body.direction = (((actor->base.body.direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 6);
        if (!SceneActorChase_StatusFlags(actor, 2)) {
            if (SceneActorChase_StatusFlags(actor, 1)) {
                SceneActorAttachment_ReplaceResponseSlot(actor, 3, 2, category);
                SceneActorChase_ClearStatusFlags(actor, 1);
            } else
                SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0, category);
            SceneActorChase_SetStatusFlags(actor, 2);
            switch (category) {
                case 4:
                    Sound_PlaySong(237);
                    break;

                case 1:
                    Sound_PlaySong(254);
                    break;

                case 3:
                    SceneActorPlacement_PlayVariantEventSound((void *)actor, 2);
                    break;

                case 25:
                    Sound_PlaySong(460);
                    break;

                case 5:
                case 6:
                case 7:
                    Sound_PlaySong(316);
                    break;

                case 21:
                    Sound_PlaySong(316);
                    break;
            }
        }
        {
            u16 bits = 2;
            motion = (void *)&actor->base.motionFlags;
            SceneActorChase_SetFlagComponent(motion, bits);
            SceneActorChase_ClearFlagComponent(motion, 0x17A9);
        }
        {
            struct SceneActorRegistryControl *control;
            actor->base.field2C2 = 64;
            actor->group.common.gate.flags = 0;
            actor->group.common.gate.counter = 0;
            SceneActorChase_ClearFlagComponent(motion, 0x380);
            Recovery_ClearTrackingFlags(&actor->base.tracking, 0x18);
            control = &actor->group.common.control;
            if (SceneActorTurn_ActorFlags(actor, 0x200000)) {
                SceneActorChase_ClearStatusFlags(actor, 0x1000000);
                SceneActorChase_ClearFlagComponent(motion, 0x200);
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
            SceneActorChase_ClearFlagComponent(motion, 0x400);
            actor->group.turn.turnsRemaining = -1;
        }
        SceneActorChase_ClearActorFlags(actor, 0x80);
        actor->base.field2C2 = 64;
        Recovery_ClearTrackingFlags(&actor->base.tracking, 0x18);
        SceneActorChase_ClearStatusFlags(actor, 0x1000000);
        Recovery_ClearRoute(actor, &group->common.control);
    }
    if (!actor->base.drawableKind) {
        struct DisplaySequenceState *status = &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
        if (((status->index == 2) && (status->elapsed == (status->duration != 0))) &&
            (status->duration != 1))
            status->elapsed = 0;
    }
    counter = (s16)actor->base.attachmentTimer;
    if (((u32)(counter - 30)) <= 89) {
        s32 remainder = Mod(counter, 30);
        s32 limit = 15 - Div(counter, 10);
        if (remainder < limit) {
            SceneActorChase_SetReactionFlags(actor, 32);
        }
    }
    x = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.x, &actor->base.reactionEnd.x,
                                        &priorX);
    z = SceneActorDisplacement_ReadAxis(actor->base.reactionStart.z, &actor->base.reactionEnd.z,
                                        &priorZ);
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    actor->base.body.motion.x += x;
    actor->base.body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (Reaction_MotionStopped(x, z) && (counter <= 0)) {
        struct ContactCollider *collider;
        actor->base.attachmentTimer = 0;
        actor->base.reactionDirection = (Reaction_ThirtySecond(actor->base.body.direction) + 7) & 7;
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, group->animationEndControl, 8);
        SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 2);
        SceneActorChase_ClearMotionFlagComponent(actor, 0x3A);
        SceneActorChase_SetState(actor, 3, 1, 0);
        actor->base.body.direction = ((actor->base.body.direction + 32) >> 6) << 6;
        collider = &actor->base.colliderA;
        if (Recovery_ActorCollisionFlags(actor, 0x40)) {
            collider->flags &= 0xFEFF;
        } else if (SceneActorChase_QueryMotionFlagComponent(actor, 0x2016)) {
            collider->flags &= 0xFEFF;
        } else {
            collider->flags |= 0x100;
        }
    }
}
