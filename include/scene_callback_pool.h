#ifndef FOUNDRY_SCENE_CALLBACK_POOL_H
#define FOUNDRY_SCENE_CALLBACK_POOL_H

#include "contact_swarm.h"

struct NativeEffectVector {
    s32 x, y, z, w;
};
struct ScenePoolSlot {
    void (*callback)(struct ScenePoolSlot *, u32);
    u8 collider[0x5C];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState animation;
    void *data;
    struct NativeEffectVector position, velocity;
    s32 height, speed;
    u16 lifetime;
    u8 angle, state;
    u16 id;
    u8 entered, active;
    s32 direction;
    u32 timer;
};
struct SceneCallbackPool {
    u8 task[0x18];
    struct ScenePoolSlot slots[16];
    u32 frame;
};
typedef char SceneCallbackPoolSizeCheck[(sizeof(struct SceneCallbackPool) == 0xD9C) ? 1 : -1];

#endif
