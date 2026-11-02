#include "global.h"
#include "ram.h"
#include "player_clone.h"

struct PlayerCloneSlot *PlayerClone_FindFreeSlot(struct PlayerCloneManager *, s32 *);
struct ContactPosition *Player_GetEffectivePosition(u32);
void PlayerClone_SetBehaviorCallback(void *, void *);
void PlayerClone_UpdateBehavior(void);
void *PlayerClone_CreatePool(void);
static inline void PlayerClone_ClearVelocity(const u32 *zero, struct ContactPosition *velocity)
{
    CpuSet(zero, velocity, 0x05000002);
}
s32 sub_08201160(struct PlayerCloneConfiguration *resource, struct ContactPosition *pos)
{
    struct {
        u32 fill;
        s32 index;
    } scratch;
    struct PlayerCloneManager *pool;
    struct PlayerCloneSlot *slot;
    pool = gDirectionalSpritePool;
    if (!pool) {
        PlayerClone_CreatePool();
    } else {
        slot = PlayerClone_FindFreeSlot(pool, &scratch.index);
        if (slot) {
            struct PlayerCloneConfiguration *handler = &slot->configuration;
            struct SceneInputWidget *display;
            u32 zero;
            struct ContactPosition *tmp;
            Memory_Copy(handler, resource, sizeof(*resource));
            zero = 0;
            slot->timer = zero;
            slot->state = zero;
            slot->unknown330 = zero;
            slot->phase = zero;
            display = &slot->animation;
            Anim_LoadKeyframe(display, &slot->resource, 0, 1);
            slot->behaviorValue = resource->behaviorValue;
            slot->animation.attribute1A = 2;
            slot->animation.position = *pos;
            slot->previousPosition = *pos;
            slot->position.x = pos->x;
            slot->position.y = pos->y + 0x80;
            slot->position.z = pos->z;
            tmp = Player_GetEffectivePosition(0);
            slot->targetPosition = *tmp;
            {
                struct ContactPosition *target = &slot->velocity;
                scratch.fill = zero;
                PlayerClone_ClearVelocity(&scratch.fill, target);
            }
            display->flags |= 1;
            PlayerClone_SetBehaviorCallback(handler, PlayerClone_UpdateBehavior);
            pool->activeSlots |= 1 << scratch.index;
            pool->count++;
            return scratch.index;
        }
        return -1;
    }
    return -1;
}
