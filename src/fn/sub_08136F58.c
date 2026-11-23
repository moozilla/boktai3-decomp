#include "scene_actor_control_phase.h"
#include "scene_actor_terrain_reaction.h"
#include "scene_actor_placement.h"
#include "scene_actor_chase_shared.h"
#include "scene_actor_reset.h"
s32 Callback_InvokeActor(void *, void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void sub_08136F48(void *, struct SceneActorRegistryControl *);


static inline void SetControl(struct ChaseActor *actor, void *callback, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}


#define GROUP_FIELD(g, off, T) (*(T *)((u8 *)(g) + (off)))



static inline u8 MoveTo(struct ChaseActor *actor, struct SceneActorRegistryControlGroup *group,
                        u32 blocked)
{
    void *shape = GROUP_FIELD(group, 0x1fc, void *);
    struct ContactPosition *position = &actor->base.body.position;
    void *destination = GROUP_FIELD(group, blocked, void *);
    if (Nav_IsPositionInMesh(shape, position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->base.body.motion, 8);
            return 1;
        }
    }
    actor->base.motionFlags &= ~blocked;
    return 0;
}

// confidence=low: scene actor wait state: on entry clears mode flags; after 60 idle frames walks to the state5D4 destination.
void sub_08136F58(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorControl_TakeStateEntry(actor)) {
        SetControl(actor, actor->base.onStanding, 0);
        SceneActorChase_AndWordFlags(&actor->base.behaviorFlags, ~0x80);
        SceneActorChase_AndWordFlags(&actor->base.statusFlags, ~0x1000000);
        SceneActorPlacement_ClearFlags(&actor->base.motionFlags, ~0x200);
        actor->base.field2C2 = 0x40;
    }
    if (((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) == 0) {
        u32 *counter = &actor->base.counter2AC;
        if ((*counter) == 30) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
            sub_08136F48(actor, &group->control);
        }
        if (((s32)(*counter)) > 60) {
            u32 bit;
            SceneActorTerrain_EnterState(actor, 0, 0, 7);
            SetControl(actor, actor->base.onAlternateEntry, 1);
            // The loop around this assignment doubles bit's use weight in
            // global allocation, which gives it r8 ahead of group (rasolve).
            do {
                bit = 0x200;
            } while (0);
            (*((struct ContactPosition **)(((u8 *)group) + bit)))->pad = 1;
            if (!MoveTo(actor, group, bit)) {
                Callback_InvokeActor(actor, *((void **)(((u8 *)group) + 0x224)));
            } else {
                struct SceneActorRegistryControl *control = &group->control;
                u32 mask = 0x200000;
                if (actor->base.behaviorFlags & mask) {
                    SceneActorChase_AndWordFlags(&actor->base.statusFlags, ~0x1000000);
                    {
                        u16 k1 = 0x200;
                        actor->base.motionFlags &= ~k1;
                    }
                    control->mode = 0;
                    control->counter = 0;
                    control->timer = 0;
                }
            }
        } else {
            (*counter)++;
        }
    }
}
