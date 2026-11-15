// Holdout draft (no concessions), 74 rows. Tail is SceneActorBehavior_RecordDefeat's matched C.
// AGBCC_FORCE=early:6,zero:7,<byteZero>:8 leaves 50 rows of local-alloc rotation in the 0x2B2..0x2B1 store block.
#include "scene_actor_behavior.h"
#include "scene_actor_reset.h"
void SceneActorAttachment_ReplaceResponseSlot(struct SceneActorBehaviorView *, u32, u32, u32);
extern const u8 gUnk_0824E114[];
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
void SceneActorControl_OnIdle(u8 *);
#include "scene_actor_contact_list.h"
struct SceneActorBehaviorPlayerSlot {
    u8 unknown0[8];
    u8 slot;
};
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u16 *CounterAddress(u32 inputOffset)
{
    u8 *base = (void *)gGameState;
    u32 offset = inputOffset;
    return (void *)(base + offset);
}
static inline void IncrementFirst(u16 *input)
{
    u16 *counter = input;
    u32 value = *counter;
    if (*(s16 *)counter <= 9998) {
        *counter = value + 1;
    } else
        *counter = 9999;
}
void SceneActorTarget_ResetContactResponse(struct SceneActorBehaviorView *inputActor)
{
    struct SceneActorBehaviorView *early = inputActor;
    struct SceneActorRegistryControlGroup *group;
    struct SceneActorBehaviorView *actor;
    u32 slot;
    u32 zero;
    {
        u32 offset = 0x3D4;
        group = (void *)((u8 *)early + offset);
    }
    {
        struct SceneActorBehaviorGateView *gate = &early->gate;
        u32 mask = 8;
        zero = Test(gate->flags, mask);
        if (zero)
            return;
    }
    {
        u16 *flags;
        u32 ownership;
        u32 inputMask = 0x1000;
        u32 *actorFlags;
        u32 byteZero;
        ownership = inputMask;
        {
            u32 flagOffset = 0x2D4;
            flags = (void *)((u8 *)early + flagOffset);
        }
        if (*flags & inputMask) {
            u32 nodeOffset = 0x45C;
            void **node;
            node = (void *)((u8 *)early + nodeOffset);
            SceneActorContacts_ReleaseNode(*node);
            {
                u32 old = *flags;
                u32 mask;
                mask = ownership;
                *flags = old & ~mask;
            }
            *node = (void *)zero;
        }
        {
            struct SceneActorRegistryControl *route = &group->control;
            u32 mask;
            actorFlags = &early->flags2CC;
            mask = ~0x141C0;
            *actorFlags &= mask;
            route->counter = zero;
            byteZero = 0;
            route->field14 = zero;
            route->timer = zero;
        }
        SceneActorReaction_ResetResponseState((u8 *)group + 0x8C, gUnk_0824E114, 0x60D);
        {
            void *control = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            u32 one;
            u32 clearMask;
            {
                u32 offset = 0x2B2;
                one = 1;
                *((u8 *)early + offset) = one;
                offset -= 2;
                *((u8 *)early + offset) = byteZero;
            }
            {
                u32 offset = 0x2B5;
                *((u8 *)early + offset) = kind;
            }
            {
                u32 offset = 0x21C;
                *(void **)((u8 *)early + offset) = control;
                offset += 0xAC;
                *(u16 *)((u8 *)early + offset) = zero;
            }
            {
                u32 *field = &early->flags114;
                clearMask = ~1;
                *field &= clearMask;
            }
            ((struct SceneActorRegistryResetView *)early)->word104 = zero;
            {
                u32 state = 3;
                u32 offset = 0x2A8;
                *((u8 *)early + offset) = state;
            }
            {
                u32 offset = 0x2A9;
                *((u8 *)early + offset) = one;
                offset++;
                *((u8 *)early + offset) = byteZero;
            }
            {
                u32 offset = 0x2AC;
                *(u32 *)((u8 *)early + offset) = zero;
                offset += 5;
                *((u8 *)early + offset) = one;
            }
            {
                u32 mask = ~0x80;
                *actorFlags &= mask;
            }
            clearMask &= *flags;
            clearMask &= ~0x400;
            *flags = clearMask;
        }
        *((u8 *)group + 0x206) = 255;
    }
    actor = early;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 0, actor->category);
    if (Test(gEngineFlags, 0x4800))
        return;
    {
        u32 mask = 0x8000;
        u32 offset = 0x2D0;
        if (*(u32 *)((u32)actor + offset) & mask)
            return;
    }
    {
        struct SceneActorBehaviorGateView *gate = &actor->gate;
        slot = ((struct SceneActorBehaviorPlayerSlot *)gate->object)->slot;
    }
    {
        u16 *counter;
        u32 value;
        u32 result;
        if (slot == 0) {
            IncrementFirst(CounterAddress(0x884));
            counter = CounterAddress(0x530);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 1) {
            counter = CounterAddress(0x886);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 2) {
            counter = CounterAddress(0x888);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        } else if (slot == 3) {
            counter = CounterAddress(0x88A);
            value = *counter;
            if (*(s16 *)counter > 9998)
                goto capped;
            result = value + 1;
            *counter = result;
            return;
        }
        return;
    capped:
        *counter = 9999;
    }
}
