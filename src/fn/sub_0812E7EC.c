// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
struct SceneActorRegistryResetView {
    void *object;
    u8 unknown4[8];
    struct CommandReceiver receiver;
    u8 unknown34[0x50 - 0x34];
    u32 id;
    u8 unknown54[5];
    u8 direction;
    u8 unknown5A[2];
    struct CommandPosition {
        s16 x, y, z, pad;
    } position;
    u8 unknown64[0x98 - 0x64];
    u8 category;
    u8 unknown99[0xA8 - 0x99];
    s16 fieldA8, fieldAA;
    u8 unknownAC[0x104 - 0xAC];
    u32 word104;
    u8 unknown108[0x114 - 0x108];
    u32 flags114;
    u8 unknown118[0x21C - 0x118];
    void *target;
    u8 unknown220[0x2A8 - 0x220];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, unknown2B3[2], field2B5;
    u8 unknown2B6[0x2C0 - 0x2B6];
    u16 flags2C0;
    u8 unknown2C2[6];
    u16 field2C8;
    u8 unknown2CA[0x2D4 - 0x2CA];
    u16 flags2D4;
    u8 unknown2D6[0x2FC - 0x2D6];
    s16 x2FC, y2FE, z300;
    u8 unknown302[0x3CD - 0x302];
    u8 desiredDirection;
    u8 unknown3CE[6];
    u8 group[0x200];
};
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
#ifdef NONMATCHING
// fuzzy: 72.2%
/* Repeated behavior/control tails are merged; receiver and attachment lifetimes differ. */
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
static inline void SetBehavior(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    u32 state = 2;
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
#define SceneActorReaction_DispatchPendingCommands sub_0812E7EC
s32 SceneActorReaction_DispatchPendingCommands(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group;
    void *attachment;
    struct CommandReceiver *receiver;
    register u32 category asm("r10");
    s32 i;
    s16 packet[3];
    s32 y, z;
    if (!HasFlag(actor, 0x100))
        return 0;
    if (HasGroup(actor))
        group = (u8 *)&actor->group;
    else
        group = 0;
    attachment = 0;
    receiver = &actor->receiver;
    category = actor->category;
    i = 0;
    if (i >= receiver->counts[gSceneEventDispatchState->bank])
        return 1;
    do {
        struct ActorCommand *command = receiver->commands[gSceneEventDispatchState->bank][i];
        switch (command->command) {
            case 0:
                if (category == 18) {
                    actor->x2FC = command->args[0];
                    actor->y2FE = command->args[1];
                    actor->z300 = command->args[2];
                    SetBehavior(actor, 3);
                } else {
                    struct CommandRoute **route = (void *)(group + 0x1FC);
                    (*route)->destination.x = command->args[0];
                    (*route)->destination.y = command->args[1];
                    (*route)->destination.z = command->args[2];
                    if (command->args[3])
                        actor->flags2D4 |= 0x200;
                    else
                        actor->flags2D4 &= ~0x200;
                    (*route)->current = (*route)->destination;
                    SetBehavior(actor, 3);
                }
                break;
            case 1: {
                s32 direction = actor->direction;
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
                        actor->desiredDirection = ((u8)command->args[0] + 5 & 7) << 5;
                        SetBehavior(actor, 4);
                    } else
                        Trigger_ConsumePending(receiver, 1);
                } else if (delta > 1) {
                    actor->desiredDirection = command->args[0];
                    SetBehavior(actor, 4);
                } else
                    Trigger_ConsumePending(receiver, 1);
                break;
            }
            case 2:
                actor->direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehavior(actor, 2);
                break;
            case 4:
                SetBehavior(actor, 5);
                break;
            case 8:
                actor->direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehavior(actor, 6);
                break;
            case 9:
                actor->direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehavior(actor, 7);
                break;
            case 10:
                actor->direction = ((u8)command->args[0] + 5 & 7) << 5;
                SetBehavior(actor, 8);
                break;
            case 11:
                SetBehavior(actor, 9);
                break;
            case 5:
                packet[0] = 6;
                SceneActorRegistry_SendToActorId(category, packet, actor->id);
                break;
            case 6:
                packet[0] = 6;
                packet[1] = command->args[0];
                packet[2] = command->args[1];
                SceneActorRegistry_SendToActorId(category, packet, actor->id);
                break;
            case 7:
                if (category == 23 || category == 18)
                    packet[0] = 7;
                else
                    packet[0] = 8;
                packet[1] = command->args[0];
                SceneActorRegistry_SendToActorId(category, packet, actor->id);
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
                            0, &actor->position, actor->id, y, z, category, value, *initial, 0,
                            actor, &actor->id);
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
                            7, &actor->position, actor->id, z, 0, value, *initial, count,
                            &actor->id, actor);
                        break;
                    }
                    case 6:
                        SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                        attachment = ActorAttachment_UpdateFromDescriptor(attachment, &actor->id, 5,
                                                                          0, 0, 0, z, y);
                        break;
                    case 7:
                        SceneActorAttachment_SelectOffsets(category, 1, &y, &z);
                        attachment = ActorAttachment_UpdateFromDescriptor(attachment, &actor->id, 4,
                                                                          0, 0, 0, z, y);
                        break;
                    case 8: {
                        void **slot = (void *)(group + 0xA0);
                        *slot = ActorAttachment_ReleaseResource(*slot, 0, actor->id);
                        *slot = ActorAttachment_ReleaseResource(*slot, 1, actor->id);
                        *slot = ActorAttachment_ReleaseResource(*slot, 2, actor->id);
                        *slot = ActorAttachment_ReleaseResource(*slot, 4, actor->id);
                        *slot = ActorAttachment_ReleaseResource(*slot, 5, actor->id);
                        *slot = ActorAttachment_ReleasePartsResource(*slot, 3, actor->id);
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
