#include "global.h"
#include "scene_vec.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: retained pool cursors, loop-stride hoisting and collider temporary allocation.
struct PanelEffectSlot {
    void *update, *onComplete;
    u8 unknown8[0x64];
};
struct PanelCollisionSlot {
    void *update, *onComplete;
    u8 unknown8[0x38];
    struct SceneVec position;
    u8 unknown48[0x84];
    u8 colliderCC[0x54], collider120[0x54];
    struct SceneVec *anchor;
    struct SceneVec delta;
    u8 unknown180, tick;
    u8 unknown182[6];
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
static inline u32 Arg1(u32 k_, s8 index)
{
    return index & k_;
}

void ScenePanelEffect_UpdatePool(struct PanelEffectPool *pool)
{
    struct PanelEffectSlot *small = pool->small;
    u32 mask = 1;
    s32 remaining = 24;
    s8 index;
    struct PanelCollisionSlot *large;
    do {
        if (small->update)
            Callback_InvokeActor(small, small->update);
        if (!small->update)
            pool->smallMask &= ~mask;
        remaining--;
        small++;
        mask <<= 1;
    } while (remaining > 0);
    large = pool->large;
    remaining = 48;
    index = 0;
    do {
        if (large->update)
            Callback_InvokeActor(large, large->update);
        if (!large->update)
            pool->largeMask[index >> 5] &= ~(1 << Arg1(31, index));
        else {
            struct SceneVec *anchor = large->anchor;
            if (anchor) {
                large->position.x = anchor->x + large->delta.x;
                large->position.y = large->delta.y + anchor->y;
                large->position.z = anchor->z + large->delta.z;
            } else {
                large->position.x = large->delta.x + large->position.x;
                large->position.y = large->delta.y + large->position.y;
                {
                    typeof(large->delta.z) v1 = large->delta.z;
                    {
                        typeof(large->position.z) v2 = large->position.z;
                        large->position.z = v1 + v2;
                    }
                }
            }
            if (!((gUnk_03003A50 ^ remaining) & 1)) {
                Collider_SetPosition(large->colliderCC, &large->position, 0);
                Collider_SetPosition(large->collider120, &large->position, 0);
                Collider_Refresh(large->colliderCC);
            }
            large->tick++;
        }
        remaining--;
        large++;
        index++;
    } while (remaining > 0);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0818ECFC);
#endif
