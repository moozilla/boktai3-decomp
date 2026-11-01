#include "global.h"
// confidence=low: settles reaction displacement and restores normal actor control.
#ifdef NONMATCHING
// fuzzy: 87.7%
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_displacement.h"
#include "display_sequence.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline void Reaction_ClearHalf(u16 *field, s32 mask)
{
    *field &= mask;
}
static inline void Reaction_ClearWord(u32 *field, s32 mask)
{
    *field &= mask;
}

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

static inline u32 Recovery_StatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u32 Recovery_ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline void Recovery_ClearStatusFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D0 &= mask;
}
static inline void Recovery_SetStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2D0 = mask | actor->flags2D0;
}
static inline void Recovery_ClearTrackingFlags(struct SceneActorChaseTracking *tracking, u32 bits)
{
    u32 mask = ~bits;
    tracking->flags &= mask;
}

static inline void Recovery_AdjustMotionFlags(struct SceneActorChaseFlags16 *field, u16 set,
                                              u32 clear)
{
    u16 value = set;
    u32 result;
    value |= field->value;
    result = value;
    result &= ~clear;
    field->value = result;
}

static inline void Recovery_ClearRoute(struct SceneActorRegistryResetView *actor,
                                       struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearActorFlags(actor, 0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}

static inline u8 Recovery_ActorCollisionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    return SceneActorChase_TestField(&actor->flags2C0, bits);
}

void SceneActorReaction_SettleDisplacement(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 counter;
    s16 *priorX, *priorZ;
    s32 x;
    s32 z;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 category = actor->category;
        u32 *word;
        struct SceneActorChaseFlags16 *motion;
        actor->body.direction = (((actor->body.direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 6);
        word = &actor->flags2D0;
        if (!Recovery_StatusFlags(actor, 2)) {
            if (*word & 1) {
                SceneActorAttachment_ReplaceResponseSlot(actor, 3, 2, category);
                Recovery_ClearStatusFlags(actor, 1);
            } else
                SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0, category);
            Recovery_SetStatusFlags(actor, 2);
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
            motion = (void *)&actor->flags2D4;
            Recovery_AdjustMotionFlags(motion, bits, 0x17A9);
        }
        {
            struct SceneActorRegistryControl *control;
            actor->field2C2 = 64;
            actor->group.common.gate.flags = 0;
            actor->group.common.gate.counter = 0;
            SceneActorChase_ClearFlagComponent(motion, 0x380);
            Recovery_ClearTrackingFlags(&actor->tracking, 0x18);
            control = &actor->group.common.control;
            if (Recovery_ActorFlags(actor, 0x200000)) {
                Recovery_ClearStatusFlags(actor, 0x1000000);
                SceneActorChase_ClearFlagComponent(motion, 0x200);
                control->mode = 0;
                control->counter = 0;
                control->timer = 0;
            }
            SceneActorChase_ClearFlagComponent(motion, 0x400);
            actor->group.turn.turnsRemaining = -1;
        }
        SceneActorChase_ClearActorFlags(actor, 0x80);
        actor->field2C2 = 64;
        Recovery_ClearTrackingFlags(&actor->tracking, 0x18);
        Recovery_ClearStatusFlags(actor, 0x1000000);
        Recovery_ClearRoute(actor, &group->common.control);
    }
    if (!actor->drawableKind) {
        struct DisplaySequenceState *status = (void *)((u8 *)actor->drawable + 0x48);
        if (((status->index == 2) && (status->elapsed == (status->duration != 0))) &&
            (status->duration != 1))
            status->elapsed = 0;
    }
    counter = (s16)actor->fieldAA;
    if (((u32)(counter - 30)) <= 89) {
        s32 remainder = Mod(counter, 30);
        s32 limit = 15 - Div(counter, 10);
        if (remainder < limit) {
            SceneActorChase_SetFlagComponent((void *)&actor->flags2D6, 0x20);
        }
    }
    x = SceneActorDisplacement_ReadAxis(actor->reactionStart.x, &actor->reactionEnd.x, &priorX);
    z = SceneActorDisplacement_ReadAxis(actor->reactionStart.z, &actor->reactionEnd.z, &priorZ);
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
    *priorX += x;
    *priorZ += z;
    if (Reaction_MotionStopped(x, z) && (counter <= 0)) {
        struct ContactCollider *collider;
        actor->fieldAA = 0;
        actor->field2B6 = (Reaction_ThirtySecond(actor->body.direction) + 7) & 7;
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, group->field5E0, 8);
        Reaction_ClearWord(&actor->flags2D0, ~2);
        Reaction_ClearHalf(&actor->flags2D4, ~0x3A);
        SceneActorChase_SetState(actor, 3, 1, 0);
        actor->body.direction = ((actor->body.direction + 32) >> 6) << 6;
        collider = &actor->colliderA;
        if (Recovery_ActorCollisionFlags(actor, 0x40)) {
            collider->flags &= 0xFEFF;
        } else if (SceneActorChase_MotionFlags(actor, 0x2016)) {
            collider->flags &= 0xFEFF;
        } else {
            u32 old = collider->flags;
            u32 mask = 0x100;
            collider->flags = mask | old;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_SettleDisplacement);
#endif
