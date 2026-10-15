// MATCHING_CONCESSION: register-pinning
#include "scene_actor_update.h"
void sub_0807CA44(struct SceneActorRegistryActor *);
static inline u8 TestFlag(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u8 TestValue(u32 value)
{
    if (value)
        return 1;
    return 0;
}
#define SceneActorBehavior_DispatchPendingFlags sub_0807A714
s32 SceneActorBehavior_DispatchPendingFlags(struct SceneActorRegistryActor *actor)
{
    register u32 mask asm("r0");
    u32 offset, first;
    register struct SceneActorRegistryActor *base asm("r5") = actor;
    register u16 *flags asm("r6");
    register u8 *phase asm("r4");
    struct SceneActorRegistryActor *updateActor;

    {
        u32 inactive;
        if (!base)
            goto available;
        {
            u32 value = 0x40;
            if (base->flags2D4 & value)
                goto inactive;
        }
        {
            u32 value = 0x20000;
            offset = 0x2D0;
            asm("" : "+r"(offset));
            if (*(u32 *)((u8 *)base + offset) & value)
                goto inactive;
        }
        if (base->health > 0)
            goto available;
    inactive:
        inactive = 1;
        goto checked;
    available:
        inactive = 0;
    checked:
        asm("" : "+r"(inactive));
        if (inactive)
            goto unavailable;
    }
    if (!base->flags2D6)
        goto unavailable;
    updateActor = base;
    offset = 0x3D4;
    asm("" : "+r"(offset));
    phase = (u8 *)base + offset;
    {
        register u32 bit asm("r2") = 4;
        register u16 *pointer asm("r1");
        pointer = &base->flags2D6;
        first = *pointer & bit;
        flags = pointer;
        first = TestValue(first);
    }
    if (first)
        mask = ~0x987;
    else if (TestFlag(flags, 2)) {
        register u32 stateOffset asm("r2");
        u32 controlOffset;
        u32 zero;
        {
            u32 value = 3;
            u8 *address;
            stateOffset = 0x2A8;
            address = (u8 *)base + stateOffset;
            zero = 0;
            *address = value;
        }
        offset = 0x2A9;
        *((u8 *)base + offset) = zero;
        base->behavior2AA = zero;
        stateOffset += 4;
        *(u32 *)((u8 *)base + stateOffset) = zero;
        offset += 8;
        *((u8 *)base + offset) = 1;
        stateOffset -= 0xA4;
        {
            register void **address asm("r0");
            address = (void *)(phase + stateOffset);
            phase = *address;
        }
        stateOffset = 7;
        offset++;
        *((u8 *)base + offset) = 1;
        controlOffset = 0x2B0;
        *((u8 *)base + controlOffset) = zero;
        offset += 3;
        *((u8 *)base + offset) = stateOffset;
        controlOffset -= 0x94;
        *(const void **)((u8 *)base + controlOffset) = phase;
        base->field2C8 = zero;
        offset = 0x114;
        asm("" : "+r"(offset));
        {
            u32 value = ~1;
            u32 *address = (void *)((u8 *)base + offset);
            asm("" : "+r"(value));
            *address &= value;
        }
        base->field104 = zero;
        {
            register s16 *address asm("r0") =
                &((struct SceneActorUpdateStatePair *)base)->values[0];
            s32 value;
            value = *address;
            address++;
            asm("" : "+r"(address));
            *address = value;
        }
        mask = ~0x983;
    } else if (TestFlag(flags, 0x800)) {
        sub_0807CA44(updateActor);
        mask = ~0x981;
    } else if (TestFlag(flags, 1))
        mask = ~0x181;
    else {
        void *callback;
        if (TestFlag(flags, 0x80)) {
            offset = 0x23C;
            asm("" : "+r"(offset));
            callback = *(void **)((u8 *)base + offset);
        } else if (TestFlag(flags, 0x100))
            callback = *(void **)((u8 *)base + 0x5EC);
        else
            goto unavailable;
        if (callback)
            sub_0824923C(base, callback);
        mask = ~0x180;
    }
    mask &= *flags;
    *flags = mask;
    return 1;
unavailable:
    return 0;
}
