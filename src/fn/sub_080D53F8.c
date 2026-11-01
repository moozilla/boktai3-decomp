#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_turn.h"

// Tentative: consumes route probes and selects navigation or a descriptor turn.

struct MummyNavigationContext {
    u8 unknown0[0x6D2];
    u16 pursuitDelay;
    u8 unknown6D4[0x1E];
    u16 flags;
};

s32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, s32, s32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, s32, s32, s32);
u8 sub_0807AF94(void *);
u8 Mummy_RequestStraightRoute(void *);

static inline u32 Mummy_MotionBits(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline void Mummy_ApplyDuration(struct SceneActorRegistryResetView *a)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (a->flags2D4 & mask) {
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

struct MummyProbeGroup {
    struct SceneActorChaseGroup *group;
};
static inline struct MummyProbeGroup Mummy_ProbeGroup(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct MummyProbeGroup owner;
    owner.group = SceneActorChase_GroupOf(actor);
    return owner;
}
static inline void Mummy_BeginProbeTurn(void *object, u32 saved, const u16 *randomSamples)
{
    struct SceneActorRegistryResetView *actor = object;
    struct MummyProbeGroup owner = Mummy_ProbeGroup(object);
    struct SceneActorChaseGroup *group = owner.group;
    if (!Mummy_MotionBits(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&actor->group.common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 2;
    }
}

static inline void Mummy_SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    SceneActorChase_ResetStateEntry(actor);
}

void Mummy_UpdateProbeNavigation(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct MummyNavigationContext *body = actor->context;
    s32 speed;
    const u16 *randomSamples = gRandomTable;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u16 *field = &body->flags;
        u32 mask = ~0x1000;
        u32 value = *field;
        mask &= value;
        *field = mask;
    }
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    speed = Callback_InvokeActor(actor, actor->animationQuery.opaque);
    Mummy_ApplyDuration(actor);
    if (NavMesh_ConsumeProbeEvent(group->route)) {
        if (!SceneActorBehavior_RequestDescriptorTurn(actor, group->route, 32, 32, speed))
            goto increment;
        if (!SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
            if (sub_0807AF94(actor))
                return;
            if (SceneActorChase_TestField((u16 *)&group->common.gate.flags, 1)) {
                u32 saved = ((actor->unknown3CD[0] + 5) & 7) << 5;
                Mummy_BeginProbeTurn(actor, saved, randomSamples);
                return;
            }
        }
        Mummy_SetBehavior(actor, 13);
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, group->route, 32, 32, speed)) {
        if (SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[0])->flags,
                                   8) &&
            !body->pursuitDelay) {
            if (Mummy_RequestStraightRoute(actor))
                return;
        }
        Mummy_SetBehavior(actor, 14);
        return;
    }
increment:
    actor->counter2AC++;
}
