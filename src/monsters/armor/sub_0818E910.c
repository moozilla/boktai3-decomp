#include "global.h"

#define Callback_Invoke           Callback_InvokeActor
#define gSceneEffectPoolCallbacks gUnk_08611D14

struct SceneEffectPosition {
    u32 words[2];
};
struct SceneEffectEntry {
    u8 unknown0[0x11E];
    u8 callbackIndex;
    u8 unknown11F[0x1E4 - 0x11F];
};
struct SceneEffectPool {
    struct SceneEffectEntry entries[3];
    struct SceneEffectPosition position;
    u8 unknown5B4[0x5C2 - 0x5B4];
    u8 tick;
    u8 count;
};
extern void *gSceneEffectPoolCallbacks[];
s32 Callback_Invoke(void *, void *);

void SceneEffectPool_Update(struct SceneEffectPool *pool, struct SceneEffectPosition *position)
{
    s32 i;
    if (position)
        pool->position = *position;
    for (i = 0; i < pool->count; i++) {
        void *callback = gSceneEffectPoolCallbacks[pool->entries[i].callbackIndex];
        if (callback)
            Callback_Invoke(&pool->entries[i], callback);
    }
    pool->tick++;
}
