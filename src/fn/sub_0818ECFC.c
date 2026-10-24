#include "global.h"
#include "scene_vec.h"
// MATCHING_CONCESSION: register-pinning
struct PanelEffectSlot {
    void *update, *onComplete;
    u8 unknown8[0x64];
};
struct PanelMotion {
    struct SceneVec *anchor;
    struct SceneVec delta;
    u8 unknown180, tick;
    u8 unknown182[6];
};
struct PanelCollisionSlot {
    void *update, *onComplete;
    u8 unknown8[0x38];
    struct SceneVec position;
    u8 unknown48[0x84];
    u8 colliderCC[0x54], collider120[0x54];
    struct PanelMotion motion;
};
struct PanelEffectPool {
    struct PanelEffectSlot small[24];
    u32 smallMask;
    struct PanelCollisionSlot large[48];
    u32 largeMask[2];
    void *resource;
};
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
extern u32 gUnk_03003A50;
// confidence=low: dispatches two effect pools and advances live collision-effect positions.
#define ScenePanelEffect_UpdatePool sub_0818ECFC
static inline void ScenePanelEffect_InvokeCallback(void *slot, void *callback)
{
    if (callback)
        Callback_InvokeActor(slot, callback);
}
static inline u32 ScenePanelEffect_MaskIndex(u32 k_, s32 index)
{
    return index & k_;
}

void ScenePanelEffect_UpdatePool(struct PanelEffectPool *pool)
{
    struct PanelEffectSlot *small = pool->small;
    u32 mask = 1;
    s32 remaining = 24;
    s32 index;
    struct PanelCollisionSlot *large;
    struct PanelMotion *motion;
    struct SceneVec *position;
    u32 stride = sizeof(*large);
    do {
        ScenePanelEffect_InvokeCallback(small, small->update);
        if (!small->update)
            pool->smallMask &= ~mask;
        remaining--;
        small++;
        mask <<= 1;
    } while (remaining > 0);
    large = pool->large;
    remaining = 48;
    index = 0;
    motion = &large->motion;
    position = &large->position;
    do {
        ScenePanelEffect_InvokeCallback(large, large->update);
        if (!large->update) {
            s32 word = index >> 5;
            s32 bit = ScenePanelEffect_MaskIndex(31, index);
            pool->largeMask[word] &= ~(1 << bit);
        } else {
            struct SceneVec *anchor = motion->anchor;
            register struct SceneVec *colliderPosition asm("r5");
            if (anchor) {
                position->x = anchor->x + motion->delta.x;
                position->y = anchor->y + motion->delta.y;
                position->z = anchor->z + motion->delta.z;
            } else {
                position->x = position->x + motion->delta.x;
                position->y = position->y + motion->delta.y;
                position->z = position->z + motion->delta.z;
            }
            colliderPosition = &large->position;
            if (!((gUnk_03003A50 ^ remaining) & 1)) {
                Collider_SetPosition(large->colliderCC, colliderPosition, 0);
                Collider_SetPosition(large->collider120, colliderPosition, 0);
                Collider_Refresh(large->colliderCC);
            }
            motion->tick++;
        }
        remaining--;
        motion = (struct PanelMotion *)((u8 *)motion + stride);
        position = (struct SceneVec *)((u8 *)position + stride);
        large = (struct PanelCollisionSlot *)((u8 *)large + stride);
        index++;
    } while (remaining > 0);
}
