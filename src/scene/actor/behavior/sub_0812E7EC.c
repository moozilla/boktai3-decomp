#include "scene/actor/scene_actor_behavior.h"
#include "scene/actor/scene_event_dispatch.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/motion/scene_actor_chase_reaction.h"
#include "scene/actor/scene_actor_status.h"
struct ActorCommand {
    u8 unknown0[6];
    u8 command, unknown7;
    union {
        s16 args[4];
        struct {
            s32 kind : 16, value : 16, initial : 16, count : 16;
        } attachment;
    } data;
};
void SceneActorControl_OnIdle(void);
u32 Trigger_ConsumePending(void *, u32);
void SceneActorRegistry_SendToActorId(u8, void *, u16);
void SceneActorAttachment_ApplySlotCommand(struct ChaseActor *, void *, u32, u32, u32);
void SceneActorAttachment_SelectOffsets(u8, u8, s32 *, s32 *);
void *ActorAttachment_CreateValueSpriteSlot(u32, void *, u32, s32, s32, u32, void *, s32, u32,
                                            void *, void *);
void *ActorAttachment_CreatePartsSlotWithCount(u32, void *, u32, s32, u32, void *, s32, u32, void *,
                                               void *);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, u32, u32, s32, s32);
void *ActorAttachment_ReleaseResource(void *, u32, u32);
void *ActorAttachment_ReleasePartsResource(void *, u32, u32);
static inline void Command_SetMotionFlag(u32 bits, struct SceneActorChaseFlags16 *field)
{
    field->value = bits | field->value;
}

static inline u8 HasGroup(struct ChaseActor *actor)
{
    if (actor->base.object == (void *)1)
        return 1;
    return 0;
}
static inline void SetBehaviorAndIdle(struct ChaseActor *actor, u32 state, u32 behavior)
{
    SceneActorTerrain_EnterState(actor, state, 0, behavior);
    SceneActorTerrain_InstallControl(actor, SceneActorControl_OnIdle, 0x10);
}
/* Command payloads and attachment offsets reuse this local workspace. */
union CommandWorkspace {
    s16 packet[3];
    struct {
        s32 x, y, z;
    } offset;
};
// confidence=low: dispatches pending receiver commands and attachment requests.
s32 SceneActorReaction_DispatchPendingCommands(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct SceneEventReceiver *receiver = NULL;
    struct SceneActorChaseGroup *group;
    void *attachment;
    union CommandWorkspace workspace;
    u32 category;
    s32 i;
    if (!ActorStatus_BodyFlags(actor, 0x100))
        return 0;
    if (HasGroup(actor))
        group = &actor->group;
    else
        group = 0;
    attachment = 0;
    receiver = (struct SceneEventReceiver *)actor->base.receiver;
    category = actor->base.category;
    i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *scene = gSceneEventDispatchState;
            struct ActorCommand *command = (void *)receiver->pending[scene->phase][i];
            switch (command->command) {
                case 0:
                    if (category == 18) {
                        actor->base.previousPosition.x = command->data.args[0];
                        actor->base.previousPosition.y = command->data.args[1];
                        actor->base.previousPosition.z = command->data.args[2];
                        SetBehaviorAndIdle(actor, 2, 3);
                    } else {
                        ((struct SceneActorBehaviorDestination *)group->route)->destination.x =
                            command->data.args[0];
                        ((struct SceneActorBehaviorDestination *)group->route)->destination.y =
                            command->data.args[1];
                        ((struct SceneActorBehaviorDestination *)group->route)->destination.z =
                            command->data.args[2];
                        if (command->data.args[3])
                            Command_SetMotionFlag(
                                0x200, (struct SceneActorChaseFlags16 *)&actor->base.motionFlags);
                        else
                            SceneActorChase_ClearFlagComponent(
                                (struct SceneActorChaseFlags16 *)&actor->base.motionFlags, 0x200);
                        ((struct SceneActorBehaviorDestination *)group->route)->position =
                            ((struct SceneActorBehaviorDestination *)group->route)->destination;
                        SetBehaviorAndIdle(actor, 2, 3);
                    }
                    break;
                case 1: {
                    s32 direction = actor->base.body.direction;
                    s32 delta;
                    if (direction >= 0)
                        direction >>= 5;
                    else
                        direction = -((-direction) >> 5);
                    direction = (direction + 3) & 7;
                    delta = command->data.args[0] - direction;
                    if (delta < 0)
                        delta = -delta;
                    if (category == 23) {
                        if (delta) {
                            actor->base.nextDirection =
                                ((u8)command->data.args[0] + 5 & 7) << 5;
                            SetBehaviorAndIdle(actor, 2, 4);
                        } else
                            Trigger_ConsumePending(((struct SceneEventReceiver *)actor->base.receiver), 1);
                    } else if (delta > 1) {
                        actor->base.nextDirection = command->data.args[0];
                        SetBehaviorAndIdle(actor, 2, 4);
                    } else
                        Trigger_ConsumePending(((struct SceneEventReceiver *)actor->base.receiver), 1);
                    break;
                }
                case 2:
                    actor->base.body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 2);
                    break;
                case 4:
                    SetBehaviorAndIdle(actor, 2, 5);
                    break;
                case 8:
                    actor->base.body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 6);
                    break;
                case 9:
                    actor->base.body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 7);
                    break;
                case 10:
                    actor->base.body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 8);
                    break;
                case 11:
                    SetBehaviorAndIdle(actor, 2, 9);
                    break;
                case 5:
                    workspace.packet[0] = 6;
                    SceneActorRegistry_SendToActorId(category, workspace.packet,
                                                     actor->base.body.id);
                    break;
                case 6:
                    workspace.packet[0] = 6;
                    workspace.packet[1] = command->data.args[0];
                    workspace.packet[2] = command->data.args[1];
                    SceneActorRegistry_SendToActorId(category, workspace.packet,
                                                     actor->base.body.id);
                    break;
                case 7:
                    if (category == 23 || category == 18)
                        workspace.packet[0] = 7;
                    else
                        workspace.packet[0] = 8;
                    workspace.packet[1] = command->data.args[0];
                    SceneActorRegistry_SendToActorId(category, workspace.packet,
                                                     actor->base.body.id);
                    break;
                case 3:
                    switch ((u32)command->data.args[0]) {
                        case 1:
                        case 2:
                            SceneActorAttachment_ApplySlotCommand(actor, attachment, 0, 0,
                                                                  category);
                            break;
                        case 0:
                            SceneActorAttachment_ApplySlotCommand(actor, attachment, 0, 1,
                                                                  category);
                            break;
                        case 3:
                            SceneActorAttachment_ApplySlotCommand(actor, attachment, 0, 2,
                                                                  category);
                            break;
                        case 4: {
                            s16 *value;
                            s16 *initial;
                            s32 initialValue;
                            actor->base.attachmentTimer =
                                command->data.attachment.value;
                            value = (s16 *)&actor->base.attachmentTimer;
                            actor->base.attachmentRestart =
                                command->data.attachment.initial;
                            initial = &actor->base.attachmentRestart;
                            SceneActorAttachment_SelectOffsets(category, 1, &workspace.offset.y,
                                                               &workspace.offset.z);
                            initialValue = *initial;
                            attachment = ActorAttachment_CreateValueSpriteSlot(
                                0, &actor->base.body.position, actor->base.body.id,
                                workspace.offset.y, workspace.offset.z, category, value,
                                initialValue, 0, actor, &actor->base.body.id);
                            break;
                        }
                        case 5: {
                            s16 *value;
                            s16 *initial;
                            s32 initialValue;
                            s32 count;
                            actor->base.attachmentTimer =
                                command->data.attachment.value;
                            value = (s16 *)&actor->base.attachmentTimer;
                            actor->base.attachmentRestart =
                                command->data.attachment.initial;
                            initial = &actor->base.attachmentRestart;
                            count = command->data.attachment.count;
                            SceneActorAttachment_SelectOffsets(category, 3, &workspace.offset.y,
                                                               &workspace.offset.z);
                            initialValue = *initial;
                            attachment = ActorAttachment_CreatePartsSlotWithCount(
                                7, &actor->base.body.position, actor->base.body.id,
                                workspace.offset.z, 0, value, initialValue, count,
                                &actor->base.body.id, actor);
                            break;
                        }
                        case 6:
                            SceneActorAttachment_SelectOffsets(category, 1, &workspace.offset.y,
                                                               &workspace.offset.z);
                            attachment = ActorAttachment_UpdateFromDescriptor(
                                attachment, &actor->base.body.id, 5, 0, 0, 0, workspace.offset.z,
                                workspace.offset.y);
                            break;
                        case 7:
                            SceneActorAttachment_SelectOffsets(category, 1, &workspace.offset.y,
                                                               &workspace.offset.z);
                            attachment = ActorAttachment_UpdateFromDescriptor(
                                attachment, &actor->base.body.id, 4, 0, 0, 0, workspace.offset.z,
                                workspace.offset.y);
                            break;
                        case 8: {
                            void **slot = &group->attachment;
                            *slot = ActorAttachment_ReleaseResource(*slot, 0, actor->base.body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 1, actor->base.body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 2, actor->base.body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 4, actor->base.body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 5, actor->base.body.id);
                            *slot =
                                ActorAttachment_ReleasePartsResource(*slot, 3, actor->base.body.id);
                            break;
                        }
                    }
                    Trigger_ConsumePending(((struct SceneEventReceiver *)actor->base.receiver), 1);
                    break;
            }
            SceneMessages_SetPendingTrigger(receiver, (struct SceneEventCommand *)command);
            ++i;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
    return 1;
}
