#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

/* The delayed response shares its first eight bytes with saved control. */
struct GolemDelayedResponse {
    struct SceneActorChaseSavedControl saved;
    u8 state, phase, behavior, kind;
    s32 delay;
    void *control;
};

static inline void Golem_PrepareDelay(struct GolemDelayedResponse *response, void *control,
                                      u32 behavior, s32 duration)
{
    response->delay = duration;
    response->control = control;
    response->state = 0;
    response->phase = 0;
    response->behavior = behavior;
}

static inline u8 Golem_TakeStateEntry(struct SceneActorRegistryResetView *actor)
{
    return SceneActorControl_TakeFlag(&actor->field2B1);
}

u8 Golem_InvokeEnabledBehavior(struct SceneActorRegistryResetView *actor);
u8 SceneActorTarget_TrySelectNearestParticipant(struct SceneActorRegistryResetView *);

static inline void Golem_SetRouteFlags(u32 *field, u32 bits)
{
    *field = bits | *field;
}

/* Restore pursuit or a home route when the active control completes. */
void Golem_ResumeRouteAfterControl(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    Golem_TakeStateEntry(actor);
    if (Golem_InvokeEnabledBehavior(actor))
        return;
    if (actor->field2B0) {
        u16 blocked;
        SceneActorChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        ProximityVolume_TranslateBounds(group->route, &actor->body.position);
        blocked = SceneActorChase_MotionFlags(actor, 0x80);
        if (blocked) {
            Golem_PrepareDelay((void *)&group->saved, group->common.gate.control, 12, 30);
            if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
                if (!SceneActorTarget_TrySelectNearestParticipant(actor))
                    return;
            } else
                SceneActorChase_TrackPlayer(actor);
            if (SceneActorChase_Arrive(actor))
                actor->field2C2 = 0x20;
        } else {
            u32 bit = 0x200;
            u16 returning = SceneActorChase_MotionFlags(actor, bit);
            u32 *flags = &actor->flags2CC;
            if (returning) {
                struct SceneActorRegistryControl *control = &group->common.control;
                if (SceneActorChase_Bits32(flags, 0x200000)) {
                    SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x1000000);
                    SceneActorChase_ClearMotionFlagComponent(actor, bit);
                    control->mode = 0;
                    control->counter = 0;
                    control->timer = 0;
                }
                SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
            }
            Golem_SetRouteFlags(flags, 0x10);
            actor->field2C2 = 0x40;
            Golem_PrepareDelay((void *)&group->saved, actor->entryTarget228, 7, 60);
            SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
            group->destination->pad = 1;
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->body.position,
                                          group->destination))
                Callback_InvokeActor(actor, group->fallback);
        }
    } else
        actor->counter2AC++;
}
