#include "global.h"
#include "scene_vec.h"
#include "ram.h"
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
// confidence=low: dispatches two effect pools and advances live collision-effect positions.
#define ScenePanelEffect_UpdatePool sub_0818ECFC
static inline void ScenePanelEffect_InvokeCallback(void *slot, void *callback)
{
    if (callback)
        Callback_InvokeActor(slot, callback);
}

static inline void RefreshCollisionSlots(struct PanelCollisionSlot *large,
                                         struct SceneVec *position, s32 remaining)
{
    if (!((gSceneEffectPalettePhase ^ remaining) & 1)) {
        Collider_SetPosition(large->colliderCC, position, 0);
        Collider_SetPosition(large->collider120, position, 0);
        Collider_Refresh(large->colliderCC);
    }
}
static inline struct SceneVec *UpdatePosition(struct PanelCollisionSlot *large,
                                              struct PanelMotion *motion, struct SceneVec *position)
{
    struct SceneVec *anchor = motion->anchor;
    if (anchor) {
        position->x = anchor->x + motion->delta.x;
        position->y = anchor->y + motion->delta.y;
        position->z = anchor->z + motion->delta.z;
        return &large->position;
    }
    position->x += motion->delta.x;
    position->y += motion->delta.y;
    position->z += motion->delta.z;
    return &large->position;
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
            s32 bit = (index & 31);
            pool->largeMask[word] &= ~(1 << bit);
        } else {
            struct SceneVec *updated = UpdatePosition(large, motion, position);
            RefreshCollisionSlots(large, updated, remaining);
            motion->tick++;
        }
        remaining--;
        motion = (struct PanelMotion *)((u8 *)motion + stride);
        position = (struct SceneVec *)((u8 *)position + stride);
        large = (struct PanelCollisionSlot *)((u8 *)large + stride);
        index++;
    } while (remaining > 0);
}
