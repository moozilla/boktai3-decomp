#include "global.h"

// MATCHING_CONCESSION: register-pinning
#define gPlayerCloneManager gUnk_020005F8
struct ClonePosition {
    u16 x, y, z, pad;
};
struct CloneAnimationState {
    u32 flags;
    u8 unknown04[0x34];
    struct ClonePosition position;
};
struct CloneColliderState {
    u8 unknown00[6];
    u16 flags;
};
struct PlayerCloneReactionSlot {
    u32 timer;
    u8 unknown004[0x10];
    struct ClonePosition position;
    struct ClonePosition previousPosition;
    u8 unknown024[0x5c];
    struct CloneColliderState collider;
    u8 unknown088[0x118];
    u32 animationFlags;
    u8 unknown1a4[0x14];
    u8 animation[0x20];
    struct ClonePosition animationPosition;
    u8 unknown1e0[0x98];
    u8 resource[0x20];
    u8 unknown298[0x6c];
    u32 flags;
    u32 state;
    u8 unknown30c[0x10];
    u32 phase;
    u8 unknown320[0x24];
};
struct PlayerCloneManager {
    u8 unknown00[0x18];
    u32 activeSlots;
    struct PlayerCloneReactionSlot slots[3];
};
extern struct PlayerCloneManager *gUnk_020005F8;
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void PlayerClone_BeginActiveReactions(void)
{
    struct PlayerCloneManager *manager = gUnk_020005F8;
    u32 slotStride = 0x344;
    if (manager) {
        struct PlayerCloneReactionSlot *s = manager->slots;
        s32 i = 0;
        // Keep the shared reset value separate from the loop index.
        register u32 resetValue asm("r10") = 0;
        u32 *flags = &s->flags;
        struct CloneAnimationState *animationFlags = (void *)(&s->animationFlags);
        u32 timer = 660;
        do {
            if (manager->activeSlots & (1 << i)) {
                s->previousPosition = animationFlags->position;
                s->position.x = animationFlags->position.x;
                s->position.y = animationFlags->position.y + 128;
                s->position.z = animationFlags->position.z;
                flags[1] = 5;
                animationFlags->flags = resetValue;
                {
                    u16 one = 1;
                    *flags |= one;
                }
                {
                    struct CloneColliderState *collider = &s->collider;
                    collider->flags |= 4;
                }
                flags[6] = resetValue;
                Anim_LoadKeyframe(s->animation, s->resource, 0, 1);
                s->timer = timer;
            }
            timer += 30;
            i++;
            flags = (void *)(((u8 *)flags) + slotStride);
            animationFlags = (void *)(((u8 *)animationFlags) + slotStride);
            s++;
        } while (i <= 2);
    }
}
