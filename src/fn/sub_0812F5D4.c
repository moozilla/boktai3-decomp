#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_turn.h"
#include "debug.h"
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, s32, s32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, s32, s32, s32);
u8 sub_0807AF94(void *);
static inline u32 ContactRoute_FieldBits(u32 mask, u16 *field)
{
    return *field & mask;
}

struct ContactRouteFlagOwner {
    u16 *field;
};
static inline u32 ContactRoute_QueryFlags(struct SceneActorRegistryResetView *actor, u32 mask,
                                          struct ContactRouteFlagOwner *owner)
{
    u16 *field = &actor->flags2D4;
    u32 bits = *field & mask;
    owner->field = field;
    return bits;
}

static inline void ContactRoute_ApplyDuration(struct SceneActorRegistryResetView *a, u32 mask,
                                              struct ContactRouteFlagOwner *owner)
{
    u16 *scale;
    if (ContactRoute_QueryFlags(a, mask, owner)) {
        u16 *p = &a->field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &a->field2C2;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!a->drawableKind)
        Anim_SetDurationScale(((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence,
                              *scale);
    else
        Anim_SetFrameDurationScale(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile,
                                   *scale);
}

static inline u16 ContactRoute_AdvanceRandomCursor(u32 *cursor, u16 wrap)
{
    u32 index = (*cursor + 1) & wrap;
    *cursor = index;
    return index;
}

static inline u16 ContactRoute_RandomSampleBits(u16 sample, u16 mask)
{
    return sample & mask;
}

#define CONTACT_ROUTE_READ_RANDOM_SAMPLE(out_, samples_, index_)                                   \
    do {                                                                                           \
        (out_) = (samples_)[index_];                                                               \
    } while (0)
#define ContactRoute_NextSample(table_, seed_, wrap_, bits_)                                       \
    ({                                                                                             \
        const struct SceneActorChaseRandomTable *table_view_ = (table_);                           \
        const u16 *samples_ = table_view_->samples;                                                \
        u32 *cursor_ = (seed_);                                                                    \
        u32 wrap_mask_ = (wrap_);                                                                  \
        u32 result_mask_ = (bits_);                                                                \
        u16 index_ = ContactRoute_AdvanceRandomCursor(cursor_, wrap_mask_);                        \
        u16 sample_;                                                                               \
        /* The cursor helper stores the wrapped value. */                                          \
        CONTACT_ROUTE_READ_RANDOM_SAMPLE(sample_, samples_, index_);                               \
        sample_ = ContactRoute_RandomSampleBits(sample_, result_mask_);                            \
        sample_;                                                                                   \
    })

static inline u8 ContactRoute_HeadingFromSample(u16 sample)
{
    return sample << 6;
}

static inline u32 ContactRoute_RandomDirectionExcept(u32 current, const u16 *randomSamples)
{
    s32 tries;
    struct SceneActorChaseRandomTable table = {randomSamples};
    for (tries = 0; tries < 4; tries++) {
        u8 direction =
            ContactRoute_HeadingFromSample(ContactRoute_NextSample(&table, &gRandomIndex, 1023, 3));
        if (direction != current)
            return direction;
    }
    return current;
}

static inline u8 ContactRoute_CurrentDirection(const struct SceneActorRegistryResetView *actor)
{
    return actor->body.direction;
}

static inline void ContactRoute_BeginProbeTurn(void *object, u32 saved, u16 *motionFlags)
{
    struct SceneActorChaseGroup *group = object;
    struct SceneActorRegistryResetView *actor = object;
    group = (void *)((u8 *)group + (u32) & ((struct SceneActorRegistryResetView *)0)->group);
    if (!ContactRoute_FieldBits(0x400, motionFlags) &&
        !SceneActorChase_TestField((u16 *)&actor->group.common.gate.flags, 4)) {
        group->turn.direction =
            ContactRoute_RandomDirectionExcept(ContactRoute_CurrentDirection(actor), gRandomTable);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 2;
    }
}

// Select the contact-facing route behavior.
static inline void ContactRoute_SetTurnBehavior(struct SceneActorRegistryResetView *actor,
                                                u32 behavior)
{
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void ContactRoute_SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior,
                                            u32 counter)
{
    actor->behavior2AA = behavior;
    actor->counter2AC = counter;
    actor->field2B1 = 1;
}

void SceneActorReaction_UpdateContactRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    s32 speed, event;
    struct ContactRouteFlagOwner motionFlags;
    u32 durationMask;
    if (SceneActorControl_TakeStateEntry(actor)) {
        DEBUG_PRINT();
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if (sub_0812F3CC(actor))
        return;
    speed = Callback_InvokeActor(actor, actor->animationQuery.opaque);
    durationMask = 0xA02F;
    ContactRoute_ApplyDuration(actor, durationMask, &motionFlags);
    event = NavMesh_ConsumeProbeEvent(group->route);
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, group->route, 32, 32, speed)) {
            if (!SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
                if (sub_0807AF94(actor))
                    return;
                if (SceneActorChase_TestField((u16 *)&group->common.gate.flags, 1)) {
                    u32 saved = ((actor->unknown3CD[0] + 5) & 7) << 5;
                    ContactRoute_BeginProbeTurn(actor, saved, motionFlags.field);
                    return;
                }
            }
            ContactRoute_SetTurnBehavior(actor, 13);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, group->route, 32, 32, speed)) {
        ContactRoute_SetBehavior(actor, 14, event);
        return;
    }
    actor->counter2AC++;
}
