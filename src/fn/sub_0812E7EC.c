#include "scene_actor_chase_reaction.h"
struct ActorCommand {
    u8 unknown0[6];
    u8 command, unknown7;
    s16 args[4];
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
struct SceneActorCommandFields {
    u8 unknown0[0xC];
    struct CommandReceiver receiver;
    u8 unknown34[0x2FC - 0x34];
    struct ContactPosition destination;
    u8 unknown304[0x3CD - 0x304];
    u8 desiredDirection;
};
#define CommandFields(actor) ((struct SceneActorCommandFields *)(actor))
extern struct CommandScene *gSceneEventDispatchState;
void SceneActorControl_OnIdle(void);
u32 Trigger_ConsumePending(void *, u32);
void SceneMessages_SetPendingTrigger(void *, void *);
void SceneActorRegistry_SendToActorId(u32, void *, u16);
void sub_08078BB4(void *, void *, u32, u32, u32);
void SceneActorAttachment_SelectOffsets(u8, u8, s32 *, s32 *);
void *ActorAttachment_CreateValueSpriteSlot(u32, void *, u32, s32, s32, u32, void *, s32, u32,
                                            void *, void *);
void *ActorAttachment_CreatePartsSlotWithCount(u32, void *, u32, s32, u32, void *, s32, u32, void *,
                                               void *);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, u32, u32, s32, s32);
void *ActorAttachment_ReleaseResource(void *, u32, u32);
void *ActorAttachment_ReleasePartsResource(void *, u32, u32);
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
// confidence=low: dispatches pending receiver commands and attachment requests.
#ifdef NONMATCHING
// fuzzy: 76.0%
// scope shape 202 (differences once low registers are set aside)
s32 SceneActorReaction_DispatchPendingCommands(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorChaseGroup *group;
    void *attachment;
    struct CommandReceiver *receiver;
    u32 category;
    s32 i;
    s16 packet[3];
    s32 y, z;
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
    if (i >= receiver->counts[gSceneEventDispatchState->bank])
        return 1;
    do {
        struct ActorCommand *command = receiver->commands[gSceneEventDispatchState->bank][i];
        switch (command->command) {
            case 0:
                if (category == 18) {
                    CommandFields(actor)->destination.x = command->args[0];
                    CommandFields(actor)->destination.y = command->args[1];
                    CommandFields(actor)->destination.z = command->args[2];
                    SetBehaviorAndIdle(actor, 2, 3);
                } else {
                    struct CommandRoute **route = (void *)&group->route;
                    (*route)->destination.x = command->args[0];
                    (*route)->destination.y = command->args[1];
                    (*route)->destination.z = command->args[2];
                    if (command->args[3])
                        actor->flags2D4 |= 0x200;
                    else
                        actor->flags2D4 &= ~0x200;
                    (*route)->current = (*route)->destination;
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
                delta = command->args[0] - direction;
                if (delta < 0)
                    delta = -delta;
                if (category == 23) {
                    if (delta) {
                        CommandFields(actor)->desiredDirection = ((u8)command->args[0] + 5 & 7)
                                                                 << 5;
                        SetBehaviorAndIdle(actor, 2, 4);
                    } else
                        Trigger_ConsumePending(receiver, 1);
                } else if (delta > 1) {
                    CommandFields(actor)->desiredDirection = command->args[0];
                    SetBehaviorAndIdle(actor, 2, 4);
                } else
                    Trigger_ConsumePending(receiver, 1);
                break;
            }
            case 2:
                actor->body.direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehaviorAndIdle(actor, 2, 2);
                break;
            case 4:
                SetBehaviorAndIdle(actor, 2, 5);
                break;
            case 8:
                actor->body.direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehaviorAndIdle(actor, 2, 6);
                break;
            case 9:
                actor->body.direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehaviorAndIdle(actor, 2, 7);
                break;
            case 10:
                actor->body.direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehaviorAndIdle(actor, 2, 8);
                break;
            case 11:
                SetBehaviorAndIdle(actor, 2, 9);
                break;
            case 5:
                packet[0] = 6;
                SceneActorRegistry_SendToActorId(category, packet, actor->body.id);
                break;
            case 6:
                packet[0] = 6;
                packet[1] = command->args[0];
                packet[2] = command->args[1];
                SceneActorRegistry_SendToActorId(category, packet, actor->body.id);
                break;
            case 7:
                if (category == 23 || category == 18)
                    packet[0] = 7;
                else
                    packet[0] = 8;
                packet[1] = command->args[0];
                SceneActorRegistry_SendToActorId(category, packet, actor->body.id);
                break;
            case 3:
                switch ((u32)command->args[0]) {
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
                        s16 *value = (s16 *)&actor->fieldAA;
                        s16 *initial = &actor->fieldA8;
                        *value = command->args[1];
                        *initial = command->args[2];
                        SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                        attachment = ActorAttachment_CreateValueSpriteSlot(
                            0, &actor->body.position, actor->body.id, y, z, category, value,
                            *initial, 0, actor, &actor->body.id);
                        break;
                    }
                    case 5: {
                        s16 *value = (s16 *)&actor->fieldAA;
                        s16 *initial = &actor->fieldA8;
                        s32 count;
                        *value = command->args[1];
                        *initial = command->args[2];
                        count = command->args[3];
                        SceneActorAttachment_SelectOffsets(category, 3, &y, &z);
                        attachment = ActorAttachment_CreatePartsSlotWithCount(
                            7, &actor->body.position, actor->body.id, z, 0, value, *initial, count,
                            &actor->body.id, actor);
                        break;
                    }
                    case 6:
                        SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                        attachment = ActorAttachment_UpdateFromDescriptor(
                            attachment, &actor->body.id, 5, 0, 0, 0, z, y);
                        break;
                    case 7:
                        SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                        attachment = ActorAttachment_UpdateFromDescriptor(
                            attachment, &actor->body.id, 4, 0, 0, 0, z, y);
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
                Trigger_ConsumePending(receiver, 1);
                break;
        }
        SceneMessages_SetPendingTrigger(receiver, command);
        ++i;
    } while (i < receiver->counts[gSceneEventDispatchState->bank]);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812E7EC);
#endif
