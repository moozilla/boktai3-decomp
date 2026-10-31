#include "scene_actor_chase_reaction.h"
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
struct CommandReceiver {
    u8 unknown0[6];
    u8 counts[2];
    struct ActorCommand *commands[2][4];
};
struct CommandScene {
    u8 unknown0[0x24];
    u8 bank;
};
struct CommandRoute {
    u8 unknown0[0xC];
    struct CommandPositionRoute {
        s16 x, y, z, pad;
    } current;
    u8 unknown14[8];
    struct CommandPositionRoute destination;
};
struct CommandAttachmentValues {
    s32 initial : 16;
    s32 value : 16;
};
struct SceneActorCommandFields {
    u8 unknown0[0xC];
    struct CommandReceiver receiver;
    u8 unknown34[0xA8 - 0x34];
    struct CommandAttachmentValues attachmentValues;
    u8 unknownAC[0x2FC - 0xAC];
    struct ContactPosition destination;
    u8 unknown304[0x3CD - 0x304];
    u8 desiredDirection;
};
#define CommandFields(actor) ((struct SceneActorCommandFields *)(actor))
extern struct CommandScene *gSceneEventDispatchState;
void SceneActorControl_OnIdle(void);
u32 Trigger_ConsumePending(void *, u32);
void SceneMessages_SetPendingTrigger(void *, void *);
void SceneActorRegistry_SendToActorId(u8, void *, u16);
void sub_08078BB4(void *, void *, u32, u32, u32);
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

static inline u8 HasFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}
static inline u8 HasGroup(struct SceneActorRegistryResetView *actor)
{
    if (actor->object == (void *)1)
        return 1;
    return 0;
}
static inline void SetBehaviorAndIdle(struct SceneActorRegistryResetView *actor, u32 state,
                                      u32 behavior)
{
    u8 *field = &actor->state2A8;
    u32 zero = 0;
    *field = state;
    actor->phase2A9 = zero;
    actor->behavior2AA = behavior;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
    {
        void *control = SceneActorControl_OnIdle;
        u32 kind = 0x10;
        actor->field2B2 = 1;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = control;
        actor->field2C8 = zero;
        {
            u32 *flags = &actor->flags114;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = zero;
    }
}
/* Command payloads and attachment offsets reuse this local workspace. */
union CommandWorkspace {
    s16 packet[3];
    struct {
        s32 x, y, z;
    } offset;
};
// confidence=low: dispatches pending receiver commands and attachment requests.
s32 SceneActorReaction_DispatchPendingCommands(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct CommandReceiver *receiver = NULL;
    struct SceneActorChaseGroup *group;
    void *attachment;
    union CommandWorkspace workspace;
    u32 category;
    s32 i;
    if (!HasFlag(actor, 0x100))
        return 0;
    if (HasGroup(actor))
        group = &actor->group;
    else
        group = 0;
    attachment = 0;
    receiver = &CommandFields(actor)->receiver;
    category = actor->category;
    i = 0;
    if (i < receiver->counts[gSceneEventDispatchState->bank]) {
        do {
            struct CommandScene *scene = gSceneEventDispatchState;
            struct ActorCommand *command = receiver->commands[scene->bank][i];
            switch (command->command) {
                case 0:
                    if (category == 18) {
                        CommandFields(actor)->destination.x = command->data.args[0];
                        CommandFields(actor)->destination.y = command->data.args[1];
                        CommandFields(actor)->destination.z = command->data.args[2];
                        SetBehaviorAndIdle(actor, 2, 3);
                    } else {
                        ((struct CommandRoute *)group->route)->destination.x =
                            command->data.args[0];
                        ((struct CommandRoute *)group->route)->destination.y =
                            command->data.args[1];
                        ((struct CommandRoute *)group->route)->destination.z =
                            command->data.args[2];
                        if (command->data.args[3])
                            Command_SetMotionFlag(
                                0x200, (struct SceneActorChaseFlags16 *)&actor->flags2D4);
                        else
                            SceneActorChase_ClearFlagComponent(
                                (struct SceneActorChaseFlags16 *)&actor->flags2D4, 0x200);
                        ((struct CommandRoute *)group->route)->current =
                            ((struct CommandRoute *)group->route)->destination;
                        SetBehaviorAndIdle(actor, 2, 3);
                    }
                    break;
                case 1: {
                    s32 direction = actor->body.direction;
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
                            CommandFields(actor)->desiredDirection =
                                ((u8)command->data.args[0] + 5 & 7) << 5;
                            SetBehaviorAndIdle(actor, 2, 4);
                        } else
                            Trigger_ConsumePending(&CommandFields(actor)->receiver, 1);
                    } else if (delta > 1) {
                        CommandFields(actor)->desiredDirection = command->data.args[0];
                        SetBehaviorAndIdle(actor, 2, 4);
                    } else
                        Trigger_ConsumePending(&CommandFields(actor)->receiver, 1);
                    break;
                }
                case 2:
                    actor->body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 2);
                    break;
                case 4:
                    SetBehaviorAndIdle(actor, 2, 5);
                    break;
                case 8:
                    actor->body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 6);
                    break;
                case 9:
                    actor->body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 7);
                    break;
                case 10:
                    actor->body.direction = ((u8)command->data.args[0] + 5 & 7) << 5;
                    SetBehaviorAndIdle(actor, 2, 8);
                    break;
                case 11:
                    SetBehaviorAndIdle(actor, 2, 9);
                    break;
                case 5:
                    workspace.packet[0] = 6;
                    SceneActorRegistry_SendToActorId(category, workspace.packet, actor->body.id);
                    break;
                case 6:
                    workspace.packet[0] = 6;
                    workspace.packet[1] = command->data.args[0];
                    workspace.packet[2] = command->data.args[1];
                    SceneActorRegistry_SendToActorId(category, workspace.packet, actor->body.id);
                    break;
                case 7:
                    if (category == 23 || category == 18)
                        workspace.packet[0] = 7;
                    else
                        workspace.packet[0] = 8;
                    workspace.packet[1] = command->data.args[0];
                    SceneActorRegistry_SendToActorId(category, workspace.packet, actor->body.id);
                    break;
                case 3:
                    switch ((u32)command->data.args[0]) {
                        case 1:
                        case 2:
                            sub_08078BB4(actor, attachment, 0, 0, category);
                            break;
                        case 0:
                            sub_08078BB4(actor, attachment, 0, 1, category);
                            break;
                        case 3:
                            sub_08078BB4(actor, attachment, 0, 2, category);
                            break;
                        case 4: {
                            s16 *value;
                            s16 *initial;
                            s32 initialValue;
                            CommandFields(actor)->attachmentValues.value =
                                command->data.attachment.value;
                            value = (s16 *)&actor->fieldAA;
                            CommandFields(actor)->attachmentValues.initial =
                                command->data.attachment.initial;
                            initial = &actor->fieldA8;
                            SceneActorAttachment_SelectOffsets(category, 1, &workspace.offset.y,
                                                               &workspace.offset.z);
                            initialValue = *initial;
                            attachment = ActorAttachment_CreateValueSpriteSlot(
                                0, &actor->body.position, actor->body.id, workspace.offset.y,
                                workspace.offset.z, category, value, initialValue, 0, actor,
                                &actor->body.id);
                            break;
                        }
                        case 5: {
                            s16 *value;
                            s16 *initial;
                            s32 initialValue;
                            s32 count;
                            CommandFields(actor)->attachmentValues.value =
                                command->data.attachment.value;
                            value = (s16 *)&actor->fieldAA;
                            CommandFields(actor)->attachmentValues.initial =
                                command->data.attachment.initial;
                            initial = &actor->fieldA8;
                            count = command->data.attachment.count;
                            SceneActorAttachment_SelectOffsets(category, 3, &workspace.offset.y,
                                                               &workspace.offset.z);
                            initialValue = *initial;
                            attachment = ActorAttachment_CreatePartsSlotWithCount(
                                7, &actor->body.position, actor->body.id, workspace.offset.z, 0,
                                value, initialValue, count, &actor->body.id, actor);
                            break;
                        }
                        case 6:
                            SceneActorAttachment_SelectOffsets(category, 1, &workspace.offset.y,
                                                               &workspace.offset.z);
                            attachment = ActorAttachment_UpdateFromDescriptor(
                                attachment, &actor->body.id, 5, 0, 0, 0, workspace.offset.z,
                                workspace.offset.y);
                            break;
                        case 7:
                            SceneActorAttachment_SelectOffsets(category, 1, &workspace.offset.y,
                                                               &workspace.offset.z);
                            attachment = ActorAttachment_UpdateFromDescriptor(
                                attachment, &actor->body.id, 4, 0, 0, 0, workspace.offset.z,
                                workspace.offset.y);
                            break;
                        case 8: {
                            void **slot = &group->attachment;
                            *slot = ActorAttachment_ReleaseResource(*slot, 0, actor->body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 1, actor->body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 2, actor->body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 4, actor->body.id);
                            *slot = ActorAttachment_ReleaseResource(*slot, 5, actor->body.id);
                            *slot = ActorAttachment_ReleasePartsResource(*slot, 3, actor->body.id);
                            break;
                        }
                    }
                    Trigger_ConsumePending(&CommandFields(actor)->receiver, 1);
                    break;
            }
            SceneMessages_SetPendingTrigger(receiver, command);
            ++i;
        } while (i < receiver->counts[gSceneEventDispatchState->bank]);
    }
    return 1;
}
