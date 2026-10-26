#define SceneSlot_GetOrCreateRegistry ActorMarkerPool_EnsureSingleton
#define gSceneSlotRegistry            gPanelSpritePool
#include "global.h"
#include "ram.h"
struct SceneSlot {
    u32 flags;
    u8 unknown4[0x38];
    void *object;
    u32 occupied;
    u8 unknown44[0x1C];
};
struct SceneSlotRegistry {
    u8 unknown0[0x34];
    struct SceneSlot slots[8];
};
struct SceneSlotRegistry *ActorMarkerPool_EnsureSingleton(void);
s32 SceneSlot_RegisterObject(void *object)
{
    struct SceneSlotRegistry *existing = gSceneSlotRegistry;
    struct SceneSlotRegistry *registry;
    s32 i;
    if (!existing) {
        registry = SceneSlot_GetOrCreateRegistry();
        if (!registry)
            return -1;
    } else {
        registry = existing;
    }
    for (i = 0; i < 8; i++) {
        if (!registry->slots[i].occupied) {
            registry->slots[i].object = object;
            registry->slots[i].flags &= ~1;
            registry->slots[i].occupied = 1;
            return i;
        }
    }
    return -1;
}
