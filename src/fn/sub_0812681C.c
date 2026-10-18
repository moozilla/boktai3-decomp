#include "global.h"
#include "scene_vec.h"
#define SceneStatusIndicator_GetOrCreateManager BlinkingEffect_EnsurePool
#define gSceneStatusIndicatorManager            gUnk_02000158
struct SceneStatusIndicator {
    u32 flags;
    u8 unknown4[11], mode, unknown10[8];
    union {
        struct SceneVec vector;
        u32 words[2];
    } position;
    u8 unknown20[8];
    u16 state;
    u8 active, index;
    u16 reset, timer;
};
struct SceneStatusIndicatorPosition {
    u32 words[2];
};
struct SceneStatusIndicatorManager {
    u8 unknown0[0x18];
    void *resource;
};
extern struct SceneStatusIndicatorManager *gUnk_02000158;
struct SceneStatusIndicatorManager *BlinkingEffect_EnsurePool(void);
struct SceneStatusIndicator *SceneStatusIndicator_Allocate(struct SceneStatusIndicatorManager *);
void sub_08217DE0(struct SceneStatusIndicator *, void *, u32);
void Effect_SetOrigin(struct SceneStatusIndicator *, s32, s32);
void sub_08217EEC(struct SceneStatusIndicator *, void *, u32);
struct SceneStatusIndicator *
SceneStatusIndicator_Create(struct SceneStatusIndicatorPosition *position)
{
    struct SceneStatusIndicatorManager *manager = gUnk_02000158;
    struct SceneStatusIndicator *entry;
    u32 one;
    if (!manager) {
        manager = BlinkingEffect_EnsurePool();
        if (!manager)
            return 0;
    }
    entry = SceneStatusIndicator_Allocate(manager);
    if (!entry)
        return 0;
    entry->active = 1;
    entry->state = 0;
    entry->timer = 0;
    one = 1;
    entry->reset = one;
    *(struct SceneStatusIndicatorPosition *)&entry->position = *position;
    entry->position.vector.y += 30;
    sub_08217DE0(entry, manager->resource, 0);
    Effect_SetOrigin(entry, -8, -8);
    sub_08217EEC(entry, manager->resource, 52);
    entry->flags |= 1;
    entry->mode = one;
    return entry;
}
