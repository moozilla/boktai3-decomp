#include "global.h"
#include "player_clone.h"
#include "ram.h"
struct CloneMotionCursor {
    s32 speed;
    u8 unknown4[0x34];
    struct ContactPosition position;
};
void PlayerClone_BeginActiveReactions(void)
{
    struct PlayerCloneManager *manager = gDirectionalSpritePool;
    u32 slotStride = sizeof(struct PlayerCloneSlot);
    if (manager) {
        struct PlayerCloneSlot *s = manager->slots;
        s32 i = 0;
        u32 resetValue = 0;
        u32 *flags = &s->flags;
        struct CloneMotionCursor *motion = (void *)(&s->configuration.speed);
        u32 timer = 660;
        do {
            if ((manager->activeSlots & (1 << i)) != 0) {
                s->previousPosition = motion->position;
                s->position.x = motion->position.x;
                s->position.y = motion->position.y + 128;
                s->position.z = motion->position.z;
                flags[1] = 5;
                motion->speed = resetValue;
                {
                    u16 one = 1;
                    *flags |= one;
                }
                {
                    struct PlayerCloneCollider *collider = &s->collider;
                    collider->flags |= 4;
                }
                PLAYER_CLONE_START_IDLE(flags[6], resetValue, &s->animation, &s->resource);
                s->timer = timer;
            }
            timer += 30;
            i++;
            flags = (void *)(((u8 *)flags) + slotStride);
            motion = (void *)(((u8 *)motion) + slotStride);
            s++;
        } while (i <= 2);
    }
}
